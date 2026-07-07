#pragma once

#ifndef ADL_ANALYSIS_ISA_PASSES_H
#define ADL_ANALYSIS_ISA_PASSES_H

#include "mlir/Pass/Pass.h"

#include <memory>

namespace adl::isa {

#define GEN_PASS_DECL
#include "adl/Analysis/ISA/Passes.h.inc"

auto createDumpInstInfoPass() -> std::unique_ptr<mlir::Pass>;
auto createDumpDecoderInfoPass() -> std::unique_ptr<mlir::Pass>;
auto createDumpDecoderTablePass() -> std::unique_ptr<mlir::Pass>;
auto createVerifyDecoderTablePass() -> std::unique_ptr<mlir::Pass>;

#define GEN_PASS_REGISTRATION
#include "adl/Analysis/ISA/Passes.h.inc"

} // namespace adl::isa

#endif
