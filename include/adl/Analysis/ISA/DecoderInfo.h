#pragma once

#ifndef ADL_ANALYSIS_ISA_DECODERINFO_H
#define ADL_ANALYSIS_ISA_DECODERINFO_H

#include "adl/Analysis/ISA/InstructionInfo.h"

#include "llvm/ADT/SmallVector.h"

#include <cstdint>
#include <optional>
#include <string>

namespace adl::isa {

struct DecoderField {
  std::string name;
  std::optional<EncodingFieldSlice> sourceSlice;
  std::optional<uint32_t> sourceWidth;
  uint32_t instMsb = 0;
  uint32_t instLsb = 0;

  [[nodiscard]] auto width() const -> uint32_t;
  [[nodiscard]] auto sourceName() const -> std::string;
  [[nodiscard]] auto instructionRangeName() const -> std::string;
};

struct DecoderInfo {
  bool valid = true;
  std::string error;
  uint32_t width = 0;
  uint64_t mask = 0;
  uint64_t value = 0;
  llvm::SmallVector<DecoderField, 8> fields;

  [[nodiscard]] auto maskHex() const -> std::string;
  [[nodiscard]] auto valueHex() const -> std::string;
};

[[nodiscard]] auto buildDecoderInfo(const InstructionInfo &info) -> DecoderInfo;

} // namespace adl::isa

#endif
