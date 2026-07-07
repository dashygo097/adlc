#pragma once

#ifndef ADL_ANALYSIS_ISA_ENCODING_H
#define ADL_ANALYSIS_ISA_ENCODING_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <cstdint>
#include <optional>
#include <string>

namespace adl::isa {

enum class EncodingSegmentKind {
  Literal,
  Field,
};

struct EncodingFieldSlice {
  uint32_t msb = 0;
  uint32_t lsb = 0;

  [[nodiscard]] auto width() const -> uint32_t;
  [[nodiscard]] auto str() const -> std::string;
};

struct EncodingSegment {
  EncodingSegmentKind kind = EncodingSegmentKind::Literal;
  std::string text;
  std::string fieldName;
  std::optional<EncodingFieldSlice> slice;
  std::optional<uint32_t> explicitWidth;
  uint32_t literalWidth = 0;

  [[nodiscard]] auto widthKnown() const -> bool;
  [[nodiscard]] auto width() const -> uint32_t;
  [[nodiscard]] auto str() const -> std::string;
};

struct InstructionEncoding {
  std::string raw;
  llvm::SmallVector<EncodingSegment, 8> segments;
  bool valid = true;
  std::string error;

  [[nodiscard]] auto hasUnknownWidth() const -> bool;
  [[nodiscard]] auto knownWidth() const -> uint32_t;
  [[nodiscard]] auto str() const -> std::string;
};

[[nodiscard]] auto parseEncoding(llvm::StringRef raw) -> InstructionEncoding;

} // namespace adl::isa

#endif
