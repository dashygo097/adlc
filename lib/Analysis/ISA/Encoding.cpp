#include "adl/Analysis/ISA/Encoding.h"

#include <cctype>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>

namespace adl::isa {
namespace {

auto isBitLiteral(llvm::StringRef token) -> bool {
  if (token.empty()) {
    return false;
  }

  for (char c : token) {
    if (c != '0' && c != '1') {
      return false;
    }
  }

  return true;
}

auto isIdentifierStart(char c) -> bool {
  return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

auto isIdentifierChar(char c) -> bool {
  return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

auto parseUInt(llvm::StringRef text, uint32_t &value) -> bool {
  uint64_t parsed = 0;

  if (text.getAsInteger(10, parsed)) {
    return false;
  }

  if (parsed > std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  value = static_cast<uint32_t>(parsed);
  return true;
}

auto makeInvalid(llvm::StringRef raw, std::string error)
    -> InstructionEncoding {
  InstructionEncoding encoding;
  encoding.raw = raw.str();
  encoding.valid = false;
  encoding.error = std::move(error);
  return encoding;
}

auto parseFieldToken(llvm::StringRef token, EncodingSegment &segment) -> bool {
  if (token.empty() || !isIdentifierStart(token.front())) {
    return false;
  }

  size_t pos = 1;
  while (pos < token.size() && isIdentifierChar(token[pos])) {
    ++pos;
  }

  segment.kind = EncodingSegmentKind::Field;
  segment.fieldName = token.take_front(pos).str();
  segment.text = token.str();

  if (pos == token.size()) {
    return true;
  }

  if (token[pos] == ':') {
    uint32_t width = 0;
    llvm::StringRef widthText = token.drop_front(pos + 1);

    if (!parseUInt(widthText, width) || width == 0) {
      return false;
    }

    segment.explicitWidth = width;
    return true;
  }

  if (token[pos] == '[') {
    if (token.empty() || token.back() != ']') {
      return false;
    }

    llvm::StringRef body = token.slice(pos + 1, token.size() - 1);

    llvm::SmallVector<llvm::StringRef, 2> parts;
    body.split(parts, ":", -1, false);

    if (parts.size() != 2 || parts[0].empty() || parts[1].empty()) {
      return false;
    }

    uint32_t msb = 0;
    uint32_t lsb = 0;

    if (!parseUInt(parts[0], msb) || !parseUInt(parts[1], lsb)) {
      return false;
    }

    if (msb < lsb) {
      return false;
    }

    segment.slice = EncodingFieldSlice{.msb = msb, .lsb = lsb};
    return true;
  }

  return false;
}

} // namespace

auto EncodingFieldSlice::width() const -> uint32_t { return msb - lsb + 1; }

auto EncodingFieldSlice::str() const -> std::string {
  return "[" + std::to_string(msb) + ":" + std::to_string(lsb) + "]";
}

auto EncodingSegment::widthKnown() const -> bool {
  if (kind == EncodingSegmentKind::Literal) {
    return true;
  }

  return slice.has_value() || explicitWidth.has_value();
}

auto EncodingSegment::width() const -> uint32_t {
  if (kind == EncodingSegmentKind::Literal) {
    return literalWidth;
  }

  if (slice.has_value()) {
    return slice->width();
  }

  if (explicitWidth.has_value()) {
    return *explicitWidth;
  }

  return 0;
}

auto EncodingSegment::str() const -> std::string {
  if (kind == EncodingSegmentKind::Literal) {
    return text;
  }

  if (slice.has_value()) {
    return fieldName + slice->str();
  }

  if (explicitWidth.has_value()) {
    return fieldName + ":" + std::to_string(*explicitWidth);
  }

  return fieldName + ":?";
}

auto InstructionEncoding::hasUnknownWidth() const -> bool {
  for (const auto &segment : segments) {
    if (!segment.widthKnown()) {
      return true;
    }
  }

  return false;
}

auto InstructionEncoding::knownWidth() const -> uint32_t {
  uint32_t total = 0;

  for (const auto &segment : segments) {
    total += segment.width();
  }

  return total;
}

auto InstructionEncoding::str() const -> std::string {
  std::string result;

  for (size_t i = 0; i < segments.size(); ++i) {
    if (i != 0) {
      result += " ";
    }

    result += segments[i].str();
  }

  return result;
}

auto parseEncoding(llvm::StringRef raw) -> InstructionEncoding {
  InstructionEncoding encoding;
  encoding.raw = raw.str();

  llvm::SmallVector<llvm::StringRef, 16> tokens;
  raw.split(tokens, " ", -1, false);

  if (tokens.empty()) {
    return makeInvalid(raw, "encoding string is empty");
  }

  for (llvm::StringRef token : tokens) {
    EncodingSegment segment;

    if (isBitLiteral(token)) {
      segment.kind = EncodingSegmentKind::Literal;
      segment.text = token.str();
      segment.literalWidth = static_cast<uint32_t>(token.size());
      encoding.segments.push_back(segment);
      continue;
    }

    if (parseFieldToken(token, segment)) {
      encoding.segments.push_back(segment);
      continue;
    }

    return makeInvalid(raw, "invalid encoding token: " + token.str());
  }

  return encoding;
}

} // namespace adl::isa
