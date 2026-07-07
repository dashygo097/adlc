#include "adl/Transforms/Proc/Passes.h"

#include "adl/Dialect/Proc/IR/ProcOps.h"

#include "mlir/IR/Block.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/Pass/Pass.h"

#include "llvm/ADT/SmallVector.h"

#include <cstdint>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace mlir;

namespace adl::proc {

#define GEN_PASS_DEF_EXPANDPROCDECODER
#include "adl/Transforms/Proc/Passes.h.inc"

namespace {

struct FieldExtractInfo {
  std::string name;
  uint64_t srcMsb = 0;
  uint64_t srcLsb = 0;
  uint64_t instMsb = 0;
  uint64_t instLsb = 0;

  [[nodiscard]] auto width() const -> uint64_t { return srcMsb - srcLsb + 1; }
};

struct ExtractedFieldPart {
  FieldExtractInfo info;
  Value value;
};

struct FieldGroup {
  std::string name;
  llvm::SmallVector<ExtractedFieldPart, 4> parts;
};

struct PacketCandidate {
  std::string inst;
  Value matched;
};

auto getString(DictionaryAttr dict, llvm::StringRef name)
    -> std::optional<std::string> {
  auto attr = dict.getAs<StringAttr>(name);
  if (!attr) {
    return std::nullopt;
  }

  return attr.getValue().str();
}

auto getI64(DictionaryAttr dict, llvm::StringRef name)
    -> std::optional<uint64_t> {
  auto attr = dict.getAs<IntegerAttr>(name);
  if (!attr) {
    return std::nullopt;
  }

  if (attr.getValue().getBitWidth() > 64) {
    return std::nullopt;
  }

  return attr.getValue().getZExtValue();
}

auto parseField(DictionaryAttr dict) -> std::optional<FieldExtractInfo> {
  std::optional<std::string> name = getString(dict, "name");
  std::optional<uint64_t> instMsb = getI64(dict, "inst_msb");
  std::optional<uint64_t> instLsb = getI64(dict, "inst_lsb");

  if (!name.has_value() || !instMsb.has_value() || !instLsb.has_value()) {
    return std::nullopt;
  }

  if (*instMsb < *instLsb) {
    return std::nullopt;
  }

  FieldExtractInfo info;
  info.name = *name;
  info.instMsb = *instMsb;
  info.instLsb = *instLsb;

  std::optional<uint64_t> srcMsb = getI64(dict, "src_msb");
  std::optional<uint64_t> srcLsb = getI64(dict, "src_lsb");
  std::optional<uint64_t> srcWidth = getI64(dict, "src_width");

  if (srcMsb.has_value() && srcLsb.has_value()) {
    if (*srcMsb < *srcLsb) {
      return std::nullopt;
    }

    info.srcMsb = *srcMsb;
    info.srcLsb = *srcLsb;
    return info;
  }

  if (srcWidth.has_value() && *srcWidth != 0) {
    info.srcMsb = *srcWidth - 1;
    info.srcLsb = 0;
    return info;
  }

  return std::nullopt;
}

auto collectDecodeEntries(DecoderOp decoder)
    -> llvm::SmallVector<DecodeEntryOp, 32> {
  llvm::SmallVector<DecodeEntryOp, 32> entries;

  decoder.walk([&](DecodeEntryOp entry) -> void { entries.push_back(entry); });

  return entries;
}

auto eraseExistingExpandedOps(DecoderOp decoder) -> void {
  llvm::SmallVector<Operation *, 32> toErase;

  decoder.walk([&](Operation *op) -> void {
    if (mlir::isa<MatchOp, ExtractFieldOp, AssembleFieldOp, DecodePacketOp,
                  SelectPacketOp>(op)) {
      toErase.push_back(op);
    }
  });

  for (Operation *op : toErase) {
    op->erase();
  }
}

auto getOrCreateInstArgument(OpBuilder &builder, DecoderOp decoder)
    -> FailureOr<Value> {
  Region &body = decoder.getBody();

  if (body.empty()) {
    builder.createBlock(&body);
  }

  if (std::next(body.begin()) != body.end()) {
    decoder.emitError() << "expected proc.decoder to contain exactly one block";
    return failure();
  }

  Block &block = body.front();

  if (block.getNumArguments() == 0) {
    Type instType = IntegerType::get(builder.getContext(), decoder.getWidth());
    block.addArgument(instType, decoder.getLoc());
  }

  if (block.getNumArguments() != 1) {
    decoder.emitError() << "expected proc.decoder block to have exactly one "
                           "instruction argument";
    return failure();
  }

  return block.getArgument(0);
}

auto getOrCreateGroup(llvm::SmallVectorImpl<FieldGroup> &groups,
                      llvm::StringRef name) -> FieldGroup & {
  for (FieldGroup &group : groups) {
    if (group.name == name) {
      return group;
    }
  }

  groups.push_back(FieldGroup{.name = name.str()});
  return groups.back();
}

auto getAssembledWidth(const FieldGroup &group) -> FailureOr<uint64_t> {
  uint64_t maxMsb = 0;

  for (const ExtractedFieldPart &part : group.parts) {
    maxMsb = std::max(maxMsb, part.info.srcMsb);
  }

  uint64_t width = maxMsb + 1;
  if (width == 0 || width > std::numeric_limits<unsigned>::max()) {
    return failure();
  }

  std::vector<bool> covered(width, false);

  for (const ExtractedFieldPart &part : group.parts) {
    if (part.info.srcMsb >= width || part.info.srcLsb > part.info.srcMsb) {
      return failure();
    }

    for (uint64_t bit = part.info.srcLsb; bit <= part.info.srcMsb; ++bit) {
      if (covered[bit]) {
        return failure();
      }

      covered[bit] = true;
    }
  }

  for (bool bitCovered : covered) {
    if (!bitCovered) {
      return failure();
    }
  }

  return width;
}

auto buildSliceDictionary(OpBuilder &builder, const FieldExtractInfo &field)
    -> DictionaryAttr {
  llvm::SmallVector<NamedAttribute, 8> attrs;

  attrs.push_back(builder.getNamedAttr(
      "inst_lsb", builder.getI64IntegerAttr(field.instLsb)));
  attrs.push_back(builder.getNamedAttr(
      "inst_msb", builder.getI64IntegerAttr(field.instMsb)));
  attrs.push_back(
      builder.getNamedAttr("src_lsb", builder.getI64IntegerAttr(field.srcLsb)));
  attrs.push_back(
      builder.getNamedAttr("src_msb", builder.getI64IntegerAttr(field.srcMsb)));

  return builder.getDictionaryAttr(attrs);
}

auto buildSliceArray(OpBuilder &builder, const FieldGroup &group) -> ArrayAttr {
  llvm::SmallVector<Attribute, 4> slices;

  for (const ExtractedFieldPart &part : group.parts) {
    slices.push_back(buildSliceDictionary(builder, part.info));
  }

  return builder.getArrayAttr(slices);
}

auto buildPartValues(const FieldGroup &group) -> llvm::SmallVector<Value, 4> {
  llvm::SmallVector<Value, 4> values;

  for (const ExtractedFieldPart &part : group.parts) {
    values.push_back(part.value);
  }

  return values;
}

auto buildStringArray(OpBuilder &builder,
                      const llvm::SmallVectorImpl<std::string> &values)
    -> ArrayAttr {
  llvm::SmallVector<Attribute, 8> attrs;

  for (const std::string &value : values) {
    attrs.push_back(builder.getStringAttr(value));
  }

  return builder.getArrayAttr(attrs);
}

auto buildInstArray(OpBuilder &builder,
                    const llvm::SmallVectorImpl<PacketCandidate> &candidates)
    -> ArrayAttr {
  llvm::SmallVector<Attribute, 8> attrs;

  for (const PacketCandidate &candidate : candidates) {
    attrs.push_back(builder.getStringAttr(candidate.inst));
  }

  return builder.getArrayAttr(attrs);
}

auto buildMatchValues(const llvm::SmallVectorImpl<PacketCandidate> &candidates)
    -> llvm::SmallVector<Value, 8> {
  llvm::SmallVector<Value, 8> values;

  for (const PacketCandidate &candidate : candidates) {
    values.push_back(candidate.matched);
  }

  return values;
}

auto finalizeFieldGroups(OpBuilder &builder,
                         llvm::SmallVectorImpl<FieldGroup> &groups,
                         DecodeEntryOp entry,
                         llvm::SmallVectorImpl<Value> &packetValues,
                         llvm::SmallVectorImpl<std::string> &packetFieldNames)
    -> LogicalResult {
  for (const FieldGroup &group : groups) {
    packetFieldNames.push_back(group.name);

    if (group.parts.size() == 1) {
      packetValues.push_back(group.parts.front().value);
      continue;
    }

    FailureOr<uint64_t> width = getAssembledWidth(group);
    if (failed(width)) {
      entry.emitError() << "cannot assemble split field '" << group.name << "'";
      return failure();
    }

    Type resultType =
        IntegerType::get(builder.getContext(), static_cast<unsigned>(*width));
    llvm::SmallVector<Value, 4> values = buildPartValues(group);

    AssembleFieldOp assembled = AssembleFieldOp::create(
        builder, entry.getLoc(), resultType, values, group.name, *width,
        buildSliceArray(builder, group));

    packetValues.push_back(assembled.getResult());
  }

  return success();
}

auto expandEntry(OpBuilder &builder, Value instWord, DecodeEntryOp entry,
                 llvm::SmallVectorImpl<PacketCandidate> &candidates)
    -> LogicalResult {
  MatchOp match =
      MatchOp::create(builder, entry.getLoc(), instWord, entry.getWidth(),
                      entry.getMask(), entry.getValue());

  llvm::SmallVector<FieldGroup, 8> groups;

  for (Attribute attr : entry.getFields()) {
    auto dict = dyn_cast<DictionaryAttr>(attr);
    if (!dict) {
      entry.emitError() << "expected field metadata to be a dictionary attr";
      return failure();
    }

    std::optional<FieldExtractInfo> field = parseField(dict);
    if (!field.has_value()) {
      entry.emitError() << "malformed field metadata";
      return failure();
    }

    Type resultType = IntegerType::get(builder.getContext(),
                                       static_cast<unsigned>(field->width()));

    ExtractFieldOp extract = ExtractFieldOp::create(
        builder, entry.getLoc(), resultType, instWord, field->name,
        field->srcMsb, field->srcLsb, field->instMsb, field->instLsb);

    FieldGroup &group = getOrCreateGroup(groups, field->name);
    group.parts.push_back(
        ExtractedFieldPart{.info = *field, .value = extract.getResult()});
  }

  llvm::SmallVector<Value, 8> packetValues;
  llvm::SmallVector<std::string, 8> packetFieldNames;

  if (failed(finalizeFieldGroups(builder, groups, entry, packetValues,
                                 packetFieldNames))) {
    return failure();
  }

  DecodePacketOp::create(
      builder, entry.getLoc(), entry.getInst(), match.getMatched(),
      packetValues, buildStringArray(builder, packetFieldNames),
      entry.getReads(), entry.getWrites(), entry.getImmediates(),
      entry.getOps(), entry.getInstClass(), entry.getMemory(),
      entry.getControlFlow());

  candidates.push_back(PacketCandidate{.inst = entry.getInst().str(),
                                       .matched = match.getMatched()});

  return success();
}

auto emitSelectPacket(OpBuilder &builder, Location loc,
                      const llvm::SmallVectorImpl<PacketCandidate> &candidates)
    -> void {
  if (candidates.empty()) {
    return;
  }

  llvm::SmallVector<Value, 8> matches = buildMatchValues(candidates);

  SelectPacketOp::create(builder, loc, matches,
                         buildInstArray(builder, candidates), "priority");
}

struct ExpandProcDecoderPass final
    : public impl::ExpandProcDecoderBase<ExpandProcDecoderPass> {
  using Base::Base;

  auto runOnOperation() -> void final {
    ModuleOp module = getOperation();

    llvm::SmallVector<DecoderOp, 4> decoders;
    module.walk(
        [&](DecoderOp decoder) -> void { decoders.push_back(decoder); });

    for (DecoderOp decoder : decoders) {
      eraseExistingExpandedOps(decoder);

      OpBuilder builder(decoder.getContext());
      FailureOr<Value> instWord = getOrCreateInstArgument(builder, decoder);
      if (failed(instWord)) {
        signalPassFailure();
        return;
      }

      llvm::SmallVector<DecodeEntryOp, 32> entries =
          collectDecodeEntries(decoder);

      Block &block = decoder.getBody().front();
      builder.setInsertionPointToEnd(&block);

      llvm::SmallVector<PacketCandidate, 8> candidates;

      for (DecodeEntryOp entry : entries) {
        if (failed(expandEntry(builder, *instWord, entry, candidates))) {
          signalPassFailure();
          return;
        }
      }

      emitSelectPacket(builder, decoder.getLoc(), candidates);
    }
  }
};

} // namespace

auto createExpandProcDecoderPass() -> std::unique_ptr<Pass> {
  return std::make_unique<ExpandProcDecoderPass>();
}

} // namespace adl::proc
