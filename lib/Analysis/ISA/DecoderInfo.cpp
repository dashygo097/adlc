#include "adl/Analysis/ISA/DecoderInfo.h"

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

namespace adl::isa {
namespace {

auto makeInvalid(std::string error) -> DecoderInfo {
  DecoderInfo info;
  info.valid = false;
  info.error = std::move(error);
  return info;
}

auto bitMask(uint32_t bit) -> uint64_t { return uint64_t{1} << bit; }

auto toHex(uint64_t value, uint32_t width) -> std::string {
  uint32_t digits = (width + 3) / 4;

  std::ostringstream os;
  os << "0x" << std::uppercase << std::hex << std::setw(digits)
     << std::setfill('0') << value;
  return os.str();
}

} // namespace

auto DecoderField::width() const -> uint32_t { return instMsb - instLsb + 1; }

auto DecoderField::sourceName() const -> std::string {
  if (sourceSlice.has_value()) {
    return name + sourceSlice->str();
  }

  if (sourceWidth.has_value()) {
    return name + ":" + std::to_string(*sourceWidth);
  }

  return name + ":?";
}

auto DecoderField::instructionRangeName() const -> std::string {
  return "inst[" + std::to_string(instMsb) + ":" + std::to_string(instLsb) +
         "]";
}

auto DecoderInfo::maskHex() const -> std::string { return toHex(mask, width); }

auto DecoderInfo::valueHex() const -> std::string {
  return toHex(value, width);
}

auto buildDecoderInfo(const InstructionInfo &info) -> DecoderInfo {
  if (!info.encoding.has_value()) {
    return makeInvalid("instruction has no encoding attribute");
  }

  const InstructionEncoding &encoding = *info.encoding;

  if (!encoding.valid) {
    return makeInvalid(encoding.error);
  }

  if (encoding.hasUnknownWidth()) {
    return makeInvalid("encoding contains field with unknown width");
  }

  uint32_t width = encoding.knownWidth();
  if (width == 0) {
    return makeInvalid("encoding has zero width");
  }

  if (width > 64) {
    return makeInvalid(
        "decoder metadata currently supports encodings up to 64 bits");
  }

  DecoderInfo decoder;
  decoder.width = width;

  int64_t cursorMsb = static_cast<int64_t>(width) - 1;

  for (const EncodingSegment &segment : encoding.segments) {
    uint32_t segmentWidth = segment.width();

    if (segmentWidth == 0) {
      return makeInvalid("encoding segment has zero width");
    }

    int64_t segmentMsb = cursorMsb;
    int64_t segmentLsb = cursorMsb - static_cast<int64_t>(segmentWidth) + 1;

    if (segmentLsb < 0) {
      return makeInvalid("encoding segment exceeds instruction width");
    }

    if (segment.kind == EncodingSegmentKind::Literal) {
      for (uint32_t i = 0; i < segmentWidth; ++i) {
        uint32_t bit = static_cast<uint32_t>(segmentMsb) - i;
        decoder.mask |= bitMask(bit);

        if (segment.text[i] == '1') {
          decoder.value |= bitMask(bit);
        }
      }
    } else {
      DecoderField field;
      field.name = segment.fieldName;
      field.sourceSlice = segment.slice;
      field.sourceWidth = segment.explicitWidth;
      field.instMsb = static_cast<uint32_t>(segmentMsb);
      field.instLsb = static_cast<uint32_t>(segmentLsb);
      decoder.fields.push_back(field);
    }

    cursorMsb = segmentLsb - 1;
  }

  return decoder;
}

} // namespace adl::isa
