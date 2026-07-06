#include "adl/Analysis/ISA/Passes.h"

#include "adl/Dialect/ISA/IR/ISAOps.h"

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>
#include <string>

using namespace mlir;

namespace adl::isa {
namespace {

struct InstInfo {
  llvm::SmallVector<std::string, 4> reads;
  llvm::SmallVector<std::string, 4> writes;
  llvm::SmallVector<std::string, 4> ops;
  bool hasMemory = false;
  bool hasControlFlow = false;
};

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

static auto analyzeInst(InstOp inst) -> InstInfo {
  InstInfo info;

  inst->walk([&](Operation *op) -> void {
    if (op == inst.getOperation()) {
      return;
    }

    if (auto read = dyn_cast<ReadRegOp>(op)) {
      info.reads.push_back(read.getName().str());
      return;
    }

    if (auto write = dyn_cast<WriteRegOp>(op)) {
      info.writes.push_back(write.getName().str());
      return;
    }

    if (mlir::isa<AddOp>(op)) {
      info.ops.push_back("add");
      return;
    }
  });

  return info;
}

static auto inferInstClass(const InstInfo &info) -> llvm::StringRef {
  if (info.hasMemory) {
    return "memory";
  }

  if (info.hasControlFlow) {
    return "control";
  }

  for (const auto &op : info.ops) {
    if (op == "add") {
      return "alu";
    }
  }

  return "unknown";
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
      InstInfo info = analyzeInst(inst);

      llvm::outs() << inst.getSymName() << ":\n";
      printList("reads", info.reads);
      printList("writes", info.writes);
      printList("ops", info.ops);
      llvm::outs() << "  class: " << inferInstClass(info) << "\n";
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
