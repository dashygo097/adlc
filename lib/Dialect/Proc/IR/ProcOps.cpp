#include "adl/Dialect/Proc/IR/ProcOps.h"

#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/Diagnostics.h"

#include <cstdint>
#include <limits>
#include <vector>

using namespace mlir;
using namespace adl::proc;

#define GET_OP_CLASSES
#include "adl/Dialect/Proc/IR/ProcOps.cpp.inc"

namespace {

auto getIntegerWidth(Type type) -> std::optional<uint64_t> {
  auto intType = dyn_cast<IntegerType>(type);
  if (!intType) {
    return std::nullopt;
  }

  return intType.getWidth();
}

auto getAttrU64(IntegerAttr attr) -> std::optional<uint64_t> {
  if (!attr) {
    return std::nullopt;
  }

  if (attr.getValue().getBitWidth() > 64) {
    return std::nullopt;
  }

  return attr.getValue().getZExtValue();
}

auto getDictU64(DictionaryAttr dict, llvm::StringRef name)
    -> std::optional<uint64_t> {
  return getAttrU64(dict.getAs<IntegerAttr>(name));
}

auto verifyStringArray(Operation *op, ArrayAttr array, llvm::StringRef name)
    -> LogicalResult {
  for (Attribute attr : array) {
    if (!isa<StringAttr>(attr)) {
      return op->emitOpError()
             << "requires `" << name << "` to contain only string attributes";
    }
  }

  return success();
}

auto verifyHexString(Operation *op, StringAttr attr, llvm::StringRef name)
    -> LogicalResult {
  uint64_t value = 0;

  if (attr.getValue().getAsInteger(0, value)) {
    return op->emitOpError()
           << "requires `" << name << "` to be an integer string";
  }

  return success();
}

auto verifySliceDictionary(Operation *op, Attribute attr,
                           uint64_t expectedWidth) -> LogicalResult {
  auto dict = dyn_cast<DictionaryAttr>(attr);
  if (!dict) {
    return op->emitOpError() << "requires `slices` entries to be dictionaries";
  }

  std::optional<uint64_t> srcMsb = getDictU64(dict, "src_msb");
  std::optional<uint64_t> srcLsb = getDictU64(dict, "src_lsb");
  std::optional<uint64_t> instMsb = getDictU64(dict, "inst_msb");
  std::optional<uint64_t> instLsb = getDictU64(dict, "inst_lsb");

  if (!srcMsb.has_value() || !srcLsb.has_value() || !instMsb.has_value() ||
      !instLsb.has_value()) {
    return op->emitOpError()
           << "requires each slice to contain src_msb, src_lsb, inst_msb, "
              "and inst_lsb";
  }

  if (*srcMsb < *srcLsb) {
    return op->emitOpError() << "requires slice src_msb >= src_lsb";
  }

  if (*instMsb < *instLsb) {
    return op->emitOpError() << "requires slice inst_msb >= inst_lsb";
  }

  uint64_t srcWidth = *srcMsb - *srcLsb + 1;
  uint64_t instWidth = *instMsb - *instLsb + 1;

  if (srcWidth != expectedWidth) {
    return op->emitOpError()
           << "requires slice source width to match corresponding operand "
              "width";
  }

  if (instWidth != expectedWidth) {
    return op->emitOpError()
           << "requires slice instruction width to match corresponding operand "
              "width";
  }

  return success();
}

} // namespace

auto MatchOp::verify() -> LogicalResult {
  std::optional<uint64_t> instWidth = getIntegerWidth(getInst().getType());
  std::optional<uint64_t> width = getAttrU64(getWidthAttr());

  if (!instWidth.has_value() || !width.has_value()) {
    return emitOpError("requires integer instruction type and width attribute");
  }

  if (*instWidth != *width) {
    return emitOpError("requires width attribute to match instruction width");
  }

  if (failed(verifyHexString(getOperation(), getMaskAttr(), "mask"))) {
    return failure();
  }

  if (failed(verifyHexString(getOperation(), getValueAttr(), "value"))) {
    return failure();
  }

  return success();
}

auto ExtractFieldOp::verify() -> LogicalResult {
  std::optional<uint64_t> instWidth = getIntegerWidth(getInst().getType());
  std::optional<uint64_t> resultWidth = getIntegerWidth(getResult().getType());

  std::optional<uint64_t> srcMsb = getAttrU64(getSrcMsbAttr());
  std::optional<uint64_t> srcLsb = getAttrU64(getSrcLsbAttr());
  std::optional<uint64_t> instMsb = getAttrU64(getInstMsbAttr());
  std::optional<uint64_t> instLsb = getAttrU64(getInstLsbAttr());

  if (!instWidth.has_value() || !resultWidth.has_value() ||
      !srcMsb.has_value() || !srcLsb.has_value() || !instMsb.has_value() ||
      !instLsb.has_value()) {
    return emitOpError("requires valid integer types and bit range attributes");
  }

  if (*srcMsb < *srcLsb) {
    return emitOpError("requires src_msb >= src_lsb");
  }

  if (*instMsb < *instLsb) {
    return emitOpError("requires inst_msb >= inst_lsb");
  }

  if (*instMsb >= *instWidth) {
    return emitOpError("requires instruction slice to fit instruction width");
  }

  uint64_t sourceWidth = *srcMsb - *srcLsb + 1;
  uint64_t instructionWidth = *instMsb - *instLsb + 1;

  if (sourceWidth != instructionWidth) {
    return emitOpError("requires source and instruction slice widths to match");
  }

  if (*resultWidth != sourceWidth) {
    return emitOpError("requires result width to match source slice width");
  }

  return success();
}

auto AssembleFieldOp::verify() -> LogicalResult {
  std::optional<uint64_t> resultWidth = getIntegerWidth(getResult().getType());
  std::optional<uint64_t> width = getAttrU64(getWidthAttr());

  if (!resultWidth.has_value() || !width.has_value()) {
    return emitOpError(
        "requires valid integer result type and width attribute");
  }

  if (*resultWidth != *width) {
    return emitOpError("requires width attribute to match result width");
  }

  if (getParts().size() != getSlices().size()) {
    return emitOpError("requires number of parts to match number of slices");
  }

  if (*width > std::numeric_limits<size_t>::max()) {
    return emitOpError("width is too large");
  }

  std::vector<bool> covered(static_cast<size_t>(*width), false);

  for (auto [part, slice] : llvm::zip(getParts(), getSlices())) {
    std::optional<uint64_t> partWidth = getIntegerWidth(part.getType());
    if (!partWidth.has_value()) {
      return emitOpError("requires all parts to have integer type");
    }

    if (failed(verifySliceDictionary(getOperation(), slice, *partWidth))) {
      return failure();
    }

    auto dict = cast<DictionaryAttr>(slice);
    uint64_t srcMsb = *getDictU64(dict, "src_msb");
    uint64_t srcLsb = *getDictU64(dict, "src_lsb");

    if (srcMsb >= *width) {
      return emitOpError("requires slice source range to fit assembled width");
    }

    for (uint64_t bit = srcLsb; bit <= srcMsb; ++bit) {
      if (covered[static_cast<size_t>(bit)]) {
        return emitOpError("requires assembled slices not to overlap");
      }

      covered[static_cast<size_t>(bit)] = true;
    }
  }

  for (bool bitCovered : covered) {
    if (!bitCovered) {
      return emitOpError("requires assembled slices to fully cover result");
    }
  }

  return success();
}

auto DecodePacketOp::verify() -> LogicalResult {
  if (getFieldValues().size() != getFieldNames().size()) {
    return emitOpError()
           << "requires number of field values to match field_names";
  }

  if (failed(
          verifyStringArray(getOperation(), getFieldNames(), "field_names"))) {
    return failure();
  }

  if (failed(verifyStringArray(getOperation(), getReads(), "reads"))) {
    return failure();
  }

  if (failed(verifyStringArray(getOperation(), getWrites(), "writes"))) {
    return failure();
  }

  if (failed(
          verifyStringArray(getOperation(), getImmediates(), "immediates"))) {
    return failure();
  }

  if (failed(verifyStringArray(getOperation(), getOps(), "ops"))) {
    return failure();
  }

  return success();
}

auto SelectPacketOp::verify() -> LogicalResult {
  if (getMatches().size() != getInsts().size()) {
    return emitOpError() << "requires number of matches to match insts";
  }

  if (getPolicy() != "priority" && getPolicy() != "onehot") {
    return emitOpError() << "requires policy to be `priority` or `onehot`";
  }

  if (failed(verifyStringArray(getOperation(), getInsts(), "insts"))) {
    return failure();
  }

  return success();
}
