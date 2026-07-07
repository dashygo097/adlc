#include "adl/Transforms/Proc/Passes.h"

#include "adl/Analysis/ISA/InstructionInfo.h"
#include "adl/Dialect/ISA/IR/ISAOps.h"
#include "adl/Dialect/Proc/IR/ProcOps.h"

#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

#include <memory>
#include <string>

using namespace mlir;

namespace adl::proc {

#define GEN_PASS_DEF_LOWERISADECODERTOPROC
#include "adl/Transforms/Proc/Passes.h.inc"

namespace {

auto eraseExistingProcDecoders(ModuleOp module) -> void {
  llvm::SmallVector<Operation *, 4> toErase;

  module.walk([&](Operation *op) -> void {
    if (mlir::isa<DecoderOp>(op)) {
      toErase.push_back(op);
    }
  });

  for (Operation *op : toErase) {
    op->erase();
  }
}

auto collectISADecoderEntries(ModuleOp module)
    -> llvm::SmallVector<adl::isa::DecoderEntryOp, 32> {
  llvm::SmallVector<adl::isa::DecoderEntryOp, 32> entries;

  module.walk([&](adl::isa::DecoderEntryOp entry) -> void {
    entries.push_back(entry);
  });

  return entries;
}

auto findISADecoderTable(ModuleOp module) -> adl::isa::DecoderTableOp {
  adl::isa::DecoderTableOp table;

  module.walk([&](adl::isa::DecoderTableOp candidate) -> WalkResult {
    table = candidate;
    return WalkResult::interrupt();
  });

  return table;
}

auto findISAInstruction(ModuleOp module, llvm::StringRef name)
    -> adl::isa::InstOp {
  adl::isa::InstOp result;

  module.walk([&](adl::isa::InstOp inst) -> WalkResult {
    if (inst.getSymName() == name) {
      result = inst;
      return WalkResult::interrupt();
    }

    return WalkResult::advance();
  });

  return result;
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

auto emitMissingInstructionError(ModuleOp module,
                                 adl::isa::DecoderEntryOp entry)
    -> LogicalResult {
  module.emitError() << "isa.decoder_entry references missing instruction '"
                     << entry.getInst() << "'";
  return failure();
}

struct LowerISADecoderToProcPass final
    : public impl::LowerISADecoderToProcBase<LowerISADecoderToProcPass> {
  using Base::Base;

  auto runOnOperation() -> void final {
    ModuleOp module = getOperation();

    adl::isa::DecoderTableOp isaTable = findISADecoderTable(module);
    if (!isaTable) {
      module.emitError() << "expected isa.decoder_table; run "
                            "--adl-materialize-decoder-table first";
      signalPassFailure();
      return;
    }

    llvm::SmallVector<adl::isa::DecoderEntryOp, 32> isaEntries =
        collectISADecoderEntries(module);

    if (isaEntries.empty()) {
      module.emitError() << "expected at least one isa.decoder_entry";
      signalPassFailure();
      return;
    }

    eraseExistingProcDecoders(module);

    OpBuilder builder(module.getContext());
    builder.setInsertionPointToEnd(module.getBody());

    DecoderOp decoder =
        DecoderOp::create(builder, module.getLoc(), isaTable.getWidth());

    Region &body = decoder.getBody();
    Block *block = builder.createBlock(&body);
    builder.setInsertionPointToEnd(block);

    for (adl::isa::DecoderEntryOp isaEntry : isaEntries) {
      adl::isa::InstOp inst = findISAInstruction(module, isaEntry.getInst());
      if (!inst) {
        if (failed(emitMissingInstructionError(module, isaEntry))) {
          signalPassFailure();
          return;
        }
      }

      adl::isa::InstructionInfo info = adl::isa::analyzeInstruction(inst);

      DecodeEntryOp::create(
          builder, module.getLoc(), isaEntry.getInst(), isaEntry.getWidth(),
          isaEntry.getMask(), isaEntry.getValue(), isaEntry.getFields(),
          buildStringArray(builder, info.reads),
          buildStringArray(builder, info.writes),
          buildStringArray(builder, info.immediates),
          buildStringArray(builder, info.ops), info.getClassName(),
          info.getMemoryName(), info.hasControlFlow);
    }
  }
};

} // namespace

auto createLowerISADecoderToProcPass() -> std::unique_ptr<Pass> {
  return std::make_unique<LowerISADecoderToProcPass>();
}

} // namespace adl::proc
