#include "adl/Analysis/ISA/InstructionInfo.h"

#include "mlir/IR/Operation.h"

using namespace mlir;

namespace adl::isa {

auto InstructionInfo::getClass() const -> InstructionClass {
  if (hasMemory) {
    return InstructionClass::Memory;
  }

  if (hasControlFlow) {
    return InstructionClass::Control;
  }

  for (const auto &op : ops) {
    if (op == "add") {
      return InstructionClass::ALU;
    }
  }

  return InstructionClass::Unknown;
}

auto InstructionInfo::getClassName() const -> llvm::StringRef {
  switch (getClass()) {
  case InstructionClass::Unknown:
    return "unknown";
  case InstructionClass::ALU:
    return "alu";
  case InstructionClass::Memory:
    return "memory";
  case InstructionClass::Control:
    return "control";
  }

  return "unknown";
}

auto analyzeInstruction(InstOp inst) -> InstructionInfo {
  InstructionInfo info;
  info.name = inst.getSymName().str();

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

} // namespace adl::isa
