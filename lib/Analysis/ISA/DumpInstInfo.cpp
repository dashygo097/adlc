#include "adl/Analysis/ISA/Passes.h"

#include "adl/Analysis/ISA/InstructionInfo.h"

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

#include "llvm/Support/raw_ostream.h"

#include <memory>
#include <string>

using namespace mlir;

namespace adl::isa {

#define GEN_PASS_DEF_DUMPINSTINFO
#include "adl/Analysis/ISA/Passes.h.inc"

namespace {

static void printList(llvm::StringRef label,
                      const llvm::SmallVectorImpl<std::string> &values) {
  llvm::outs() << "  " << label << ": ";
  if (values.empty()) {
    llvm::outs() << "none\n";
    return;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    if (i != 0) {
      llvm::outs() << ", ";
    }
    llvm::outs() << values[i];
  }

  llvm::outs() << "\n";
}

struct DumpInstInfoPass final
    : public impl::DumpInstInfoBase<DumpInstInfoPass> {
  using Base::Base;

  void runOnOperation() final {
    ModuleOp module = getOperation();

    module.walk([&](InstOp inst) -> void {
      InstructionInfo info = analyzeInstruction(inst);

      llvm::outs() << info.name << ":\n";
      printList("reads", info.reads);
      printList("writes", info.writes);
      printList("ops", info.ops);
      llvm::outs() << "  class: " << info.getClassName() << "\n";
      llvm::outs() << "  memory: " << (info.hasMemory ? "yes" : "none") << "\n";
      llvm::outs() << "  control-flow: "
                   << (info.hasControlFlow ? "yes" : "none") << "\n";
    });
  }
};

} // namespace

auto createDumpInstInfoPass() -> std::unique_ptr<Pass> {
  return std::make_unique<DumpInstInfoPass>();
}

} // namespace adl::isa
