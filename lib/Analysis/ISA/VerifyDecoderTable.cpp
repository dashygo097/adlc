#include "adl/Analysis/ISA/Passes.h"

#include "adl/Analysis/ISA/DecoderTable.h"

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

#include <memory>
#include <string>

using namespace mlir;

namespace adl::isa {

#define GEN_PASS_DEF_VERIFYDECODERTABLE
#include "adl/Analysis/ISA/Passes.h.inc"

namespace {

struct VerifyDecoderTablePass final
    : public impl::VerifyDecoderTableBase<VerifyDecoderTablePass> {
  using Base::Base;

  auto runOnOperation() -> void final {
    ModuleOp module = getOperation();
    DecoderTable table = buildDecoderTable(module);

    bool failed = false;

    for (const std::string &error : table.errors) {
      module.emitError() << "invalid decoder table entry: " << error;
      failed = true;
    }

    for (const DecoderConflict &conflict : table.conflicts) {
      module.emitError() << "decoder conflict between '" << conflict.lhsName
                         << "' and '" << conflict.rhsName << "'"
                         << ", common-mask = " << conflict.commonMaskHex()
                         << ", lhs-value = " << conflict.lhsValueHex()
                         << ", rhs-value = " << conflict.rhsValueHex();
      failed = true;
    }

    if (failed) {
      signalPassFailure();
    }
  }
};

} // namespace

auto createVerifyDecoderTablePass() -> std::unique_ptr<Pass> {
  return std::make_unique<VerifyDecoderTablePass>();
}

} // namespace adl::isa
