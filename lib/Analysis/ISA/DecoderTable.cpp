#include "adl/Analysis/ISA/DecoderTable.h"

#include "adl/Analysis/ISA/InstructionInfo.h"

#include <iomanip>
#include <sstream>
#include <string>

using namespace mlir;

namespace adl::isa {
namespace {

auto toHex(uint64_t value, uint32_t width) -> std::string {
  uint32_t digits = (width + 3) / 4;

  std::ostringstream os;
  os << "0x" << std::uppercase << std::hex << std::setw(digits)
     << std::setfill('0') << value;
  return os.str();
}

auto decodersOverlap(const DecoderInfo &lhs, const DecoderInfo &rhs) -> bool {
  if (!lhs.valid || !rhs.valid) {
    return false;
  }

  if (lhs.width != rhs.width) {
    return false;
  }

  uint64_t commonMask = lhs.mask & rhs.mask;
  return ((lhs.value ^ rhs.value) & commonMask) == 0;
}

auto makeConflict(const DecoderTableEntry &lhs, const DecoderTableEntry &rhs)
    -> DecoderConflict {
  DecoderConflict conflict;
  conflict.lhsName = lhs.name;
  conflict.rhsName = rhs.name;
  conflict.width = lhs.decoder.width;
  conflict.commonMask = lhs.decoder.mask & rhs.decoder.mask;
  conflict.lhsValue = lhs.decoder.value;
  conflict.rhsValue = rhs.decoder.value;
  return conflict;
}

} // namespace

auto DecoderConflict::commonMaskHex() const -> std::string {
  return toHex(commonMask, width);
}

auto DecoderConflict::lhsValueHex() const -> std::string {
  return toHex(lhsValue, width);
}

auto DecoderConflict::rhsValueHex() const -> std::string {
  return toHex(rhsValue, width);
}

auto DecoderTable::valid() const -> bool {
  return errors.empty() && conflicts.empty();
}

auto DecoderTable::hasConflicts() const -> bool { return !conflicts.empty(); }

auto buildDecoderTable(ModuleOp module) -> DecoderTable {
  DecoderTable table;

  module.walk([&](InstOp inst) -> void {
    InstructionInfo instInfo = analyzeInstruction(inst);
    DecoderInfo decoderInfo = buildDecoderInfo(instInfo);

    DecoderTableEntry entry;
    entry.name = instInfo.name;
    entry.decoder = decoderInfo;
    table.entries.push_back(entry);

    if (!decoderInfo.valid) {
      table.errors.push_back(instInfo.name + ": " + decoderInfo.error);
    }
  });

  for (size_t i = 0; i < table.entries.size(); ++i) {
    const DecoderTableEntry &lhs = table.entries[i];

    if (!lhs.decoder.valid) {
      continue;
    }

    for (size_t j = i + 1; j < table.entries.size(); ++j) {
      const DecoderTableEntry &rhs = table.entries[j];

      if (!rhs.decoder.valid) {
        continue;
      }

      if (decodersOverlap(lhs.decoder, rhs.decoder)) {
        table.conflicts.push_back(makeConflict(lhs, rhs));
      }
    }
  }

  return table;
}

} // namespace adl::isa
