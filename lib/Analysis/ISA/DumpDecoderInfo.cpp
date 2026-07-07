#include "adl/Analysis/ISA/Passes.h"

#include "adl/Analysis/ISA/DecoderInfo.h"
#include "adl/Analysis/ISA/InstructionInfo.h"

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

#include "llvm/Support/raw_ostream.h"

#include <memory>

using namespace mlir;

namespace adl::isa {

#define GEN_PASS_DEF_DUMPDECODERINFO
#include "adl/Analysis/ISA/Passes.h.inc"

namespace {

auto printDecoderInfo(const DecoderInfo &decoder) -> void {
  if (!decoder.valid) {
    llvm::outs() << "  decoder: invalid\n";
    llvm::outs() << "  decoder-error: " << decoder.error << "\n";
    return;
  }

  llvm::outs() << "  decoder-width: " << decoder.width << "\n";
  llvm::outs() << "  decoder-mask: " << decoder.maskHex() << "\n";
  llvm::outs() << "  decoder-value: " << decoder.valueHex() << "\n";

  if (decoder.fields.empty()) {
    llvm::outs() << "  fields: none\n";
    return;
  }

  llvm::outs() << "  fields:\n";
  for (const DecoderField &field : decoder.fields) {
    llvm::outs() << "    " << field.sourceName() << " -> "
                 << field.instructionRangeName() << "\n";
  }
}

struct DumpDecoderInfoPass final
    : public impl::DumpDecoderInfoBase<DumpDecoderInfoPass> {
  using Base::Base;

  auto runOnOperation() -> void final {
    ModuleOp module = getOperation();

    module.walk([&](InstOp inst) -> void {
      InstructionInfo instInfo = analyzeInstruction(inst);
      DecoderInfo decoderInfo = buildDecoderInfo(instInfo);

      llvm::outs() << instInfo.name << ":\n";
      printDecoderInfo(decoderInfo);
    });
  }
};

} // namespace

auto createDumpDecoderInfoPass() -> std::unique_ptr<Pass> {
  return std::make_unique<DumpDecoderInfoPass>();
}

} // namespace adl::isa
