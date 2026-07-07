#include "adl/Analysis/ISA/Passes.h"

#include "adl/Analysis/ISA/DecoderTable.h"

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

#include "llvm/Support/raw_ostream.h"

#include <memory>

using namespace mlir;

namespace adl::isa {

#define GEN_PASS_DEF_DUMPDECODERTABLE
#include "adl/Analysis/ISA/Passes.h.inc"

namespace {

auto printDecoderEntry(const DecoderTableEntry &entry) -> void {
  const DecoderInfo &decoder = entry.decoder;

  llvm::outs() << "  " << entry.name << ":\n";

  if (!decoder.valid) {
    llvm::outs() << "    decoder: invalid\n";
    llvm::outs() << "    decoder-error: " << decoder.error << "\n";
    return;
  }

  llvm::outs() << "    width: " << decoder.width << "\n";
  llvm::outs() << "    mask: " << decoder.maskHex() << "\n";
  llvm::outs() << "    value: " << decoder.valueHex() << "\n";
}

auto printDecoderConflict(const DecoderConflict &conflict) -> void {
  llvm::outs() << "  " << conflict.lhsName << " <-> " << conflict.rhsName
               << ":\n";
  llvm::outs() << "    common-mask: " << conflict.commonMaskHex() << "\n";
  llvm::outs() << "    lhs-value: " << conflict.lhsValueHex() << "\n";
  llvm::outs() << "    rhs-value: " << conflict.rhsValueHex() << "\n";
}

struct DumpDecoderTablePass final
    : public impl::DumpDecoderTableBase<DumpDecoderTablePass> {
  using Base::Base;

  auto runOnOperation() -> void final {
    DecoderTable table = buildDecoderTable(getOperation());

    llvm::outs() << "decoder-table:\n";
    llvm::outs() << "  entries: " << table.entries.size() << "\n";

    if (table.errors.empty()) {
      llvm::outs() << "  errors: none\n";
    } else {
      llvm::outs() << "  errors:\n";
      for (const std::string &error : table.errors) {
        llvm::outs() << "    " << error << "\n";
      }
    }

    if (table.conflicts.empty()) {
      llvm::outs() << "  conflicts: none\n";
    } else {
      llvm::outs() << "  conflicts:\n";
      for (const DecoderConflict &conflict : table.conflicts) {
        printDecoderConflict(conflict);
      }
    }

    llvm::outs() << "  table:\n";
    for (const DecoderTableEntry &entry : table.entries) {
      printDecoderEntry(entry);
    }
  }
};

} // namespace

auto createDumpDecoderTablePass() -> std::unique_ptr<Pass> {
  return std::make_unique<DumpDecoderTablePass>();
}

} // namespace adl::isa
