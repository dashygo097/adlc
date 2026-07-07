#pragma once

#ifndef ADL_ANALYSIS_ISA_DECODERTABLE_H
#define ADL_ANALYSIS_ISA_DECODERTABLE_H

#include "adl/Analysis/ISA/DecoderInfo.h"

#include "mlir/IR/BuiltinOps.h"

#include "llvm/ADT/SmallVector.h"

#include <cstdint>
#include <string>

namespace adl::isa {

struct DecoderTableEntry {
  std::string name;
  DecoderInfo decoder;
};

struct DecoderConflict {
  std::string lhsName;
  std::string rhsName;
  uint32_t width = 0;
  uint64_t commonMask = 0;
  uint64_t lhsValue = 0;
  uint64_t rhsValue = 0;

  [[nodiscard]] auto commonMaskHex() const -> std::string;
  [[nodiscard]] auto lhsValueHex() const -> std::string;
  [[nodiscard]] auto rhsValueHex() const -> std::string;
};

struct DecoderTable {
  llvm::SmallVector<DecoderTableEntry, 32> entries;
  llvm::SmallVector<DecoderConflict, 8> conflicts;
  llvm::SmallVector<std::string, 8> errors;

  [[nodiscard]] auto valid() const -> bool;
  [[nodiscard]] auto hasConflicts() const -> bool;
};

[[nodiscard]] auto buildDecoderTable(mlir::ModuleOp module) -> DecoderTable;

} // namespace adl::isa

#endif
