#pragma once

#ifndef ADL_ANALYSIS_ISA_INSTRUCTIONINFO_H
#define ADL_ANALYSIS_ISA_INSTRUCTIONINFO_H

#include "adl/Dialect/ISA/IR/ISAOps.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <string>

namespace adl::isa {

enum class InstructionClass {
  Unknown,
  ALU,
  Memory,
  Control,
};

struct InstructionInfo {
  std::string name;
  llvm::SmallVector<std::string, 4> reads;
  llvm::SmallVector<std::string, 4> writes;
  llvm::SmallVector<std::string, 4> ops;
  bool hasMemoryRead = false;
  bool hasMemoryWrite = false;
  bool hasControlFlow = false;

  [[nodiscard]] auto hasMemory() const -> bool;
  [[nodiscard]] auto getMemoryName() const -> llvm::StringRef;
  [[nodiscard]] auto getClass() const -> InstructionClass;
  [[nodiscard]] auto getClassName() const -> llvm::StringRef;
};

[[nodiscard]] auto analyzeInstruction(InstOp inst) -> InstructionInfo;

} // namespace adl::isa

#endif
