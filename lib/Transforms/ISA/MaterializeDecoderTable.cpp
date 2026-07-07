#include "adl/Transforms/ISA/Passes.h"

#include "adl/Analysis/ISA/DecoderTable.h"
#include "adl/Dialect/ISA/IR/ISAOps.h"

#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

#include <memory>
#include <string>

using namespace mlir;

namespace adl::isa {

#define GEN_PASS_DEF_MATERIALIZEDECODERTABLE
#include "adl/Transforms/ISA/Passes.h.inc"

namespace {

auto eraseExistingDecoderMetadata(ModuleOp module) -> void {
  llvm::SmallVector<Operation *, 8> toErase;

  module.walk([&](Operation *op) -> void {
    if (mlir::isa<DecoderTableOp, DecoderEntryOp>(op)) {
      toErase.push_back(op);
    }
  });

  for (Operation *op : toErase) {
    op->erase();
  }
}

auto buildFieldDictionary(OpBuilder &builder, const DecoderField &field)
    -> DictionaryAttr {
  llvm::SmallVector<NamedAttribute, 8> attrs;

  attrs.push_back(
      builder.getNamedAttr("name", builder.getStringAttr(field.name)));
  attrs.push_back(builder.getNamedAttr(
      "inst_msb", builder.getI64IntegerAttr(field.instMsb)));
  attrs.push_back(builder.getNamedAttr(
      "inst_lsb", builder.getI64IntegerAttr(field.instLsb)));

  if (field.sourceSlice.has_value()) {
    attrs.push_back(builder.getNamedAttr(
        "src_msb", builder.getI64IntegerAttr(field.sourceSlice->msb)));
    attrs.push_back(builder.getNamedAttr(
        "src_lsb", builder.getI64IntegerAttr(field.sourceSlice->lsb)));
  }

  if (field.sourceWidth.has_value()) {
    attrs.push_back(builder.getNamedAttr(
        "src_width", builder.getI64IntegerAttr(*field.sourceWidth)));
  }

  return builder.getDictionaryAttr(attrs);
}

auto buildFieldArray(OpBuilder &builder, const DecoderInfo &decoder)
    -> ArrayAttr {
  llvm::SmallVector<Attribute, 8> fields;

  for (const DecoderField &field : decoder.fields) {
    fields.push_back(buildFieldDictionary(builder, field));
  }

  return builder.getArrayAttr(fields);
}

auto emitDecoderTableErrors(ModuleOp module, const DecoderTable &table)
    -> bool {
  bool failed = false;

  for (const std::string &error : table.errors) {
    module.emitError() << "invalid decoder table entry: " << error;
    failed = true;
  }

  for (const DecoderConflict &conflict : table.conflicts) {
    module.emitError() << "decoder conflict between '" << conflict.lhsName
                       << "' and '" << conflict.rhsName << "'"
                       << ", common-mask = " << conflict.commonMaskHex()
                       << ", lhs-value = " << conflict.lhsValueHex()
                       << ", rhs-value = " << conflict.rhsValueHex();
    failed = true;
  }

  return failed;
}

struct MaterializeDecoderTablePass final
    : public impl::MaterializeDecoderTableBase<MaterializeDecoderTablePass> {
  using Base::Base;

  auto runOnOperation() -> void final {
    ModuleOp module = getOperation();
    DecoderTable table = buildDecoderTable(module);

    if (emitDecoderTableErrors(module, table)) {
      signalPassFailure();
      return;
    }

    eraseExistingDecoderMetadata(module);

    OpBuilder builder(module.getContext());
    builder.setInsertionPointToEnd(module.getBody());

    uint32_t tableWidth = 0;
    if (!table.entries.empty()) {
      tableWidth = table.entries.front().decoder.width;
    }

    DecoderTableOp::create(builder, module.getLoc(),
                           builder.getI64IntegerAttr(tableWidth));

    for (const DecoderTableEntry &entry : table.entries) {
      const DecoderInfo &decoder = entry.decoder;

      DecoderEntryOp::create(
          builder, module.getLoc(),
          FlatSymbolRefAttr::get(builder.getContext(), entry.name),
          builder.getI64IntegerAttr(decoder.width),
          builder.getStringAttr(decoder.maskHex()),
          builder.getStringAttr(decoder.valueHex()),
          buildFieldArray(builder, decoder));
    }
  }
};

} // namespace

auto createMaterializeDecoderTablePass() -> std::unique_ptr<Pass> {
  return std::make_unique<MaterializeDecoderTablePass>();
}

} // namespace adl::isa
