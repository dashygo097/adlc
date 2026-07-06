#include "adl/Analysis/ISA/Passes.h"

#include "adl/Analysis/ISA/InstructionInfo.h"

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

#include "llvm/Support/raw_ostream.h"

#include <memory>
#include <string>

using namespace mlir;

namespace adl::isa {
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
    : public PassWrapper<DumpInstInfoPass, OperationPass<ModuleOp>> {
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(DumpInstInfoPass)

  [[nodiscard]] auto getArgument() const -> StringRef final {
    return "adl-dump-inst-info";
  }

  [[nodiscard]] auto getDescription() const -> StringRef final {
    return "Dump inferred instruction information from the ADL ISA dialect";
  }

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

void registerISAAnalysisPasses() { PassRegistration<DumpInstInfoPass>(); }

} // namespace adl::isa
