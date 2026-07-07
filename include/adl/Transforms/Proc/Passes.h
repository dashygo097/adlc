#pragma once

#ifndef ADL_TRANSFORMS_PROC_PASSES_H
#define ADL_TRANSFORMS_PROC_PASSES_H

#include "mlir/Pass/Pass.h"

#include <memory>

namespace adl::proc {

#define GEN_PASS_DECL
#include "adl/Transforms/Proc/Passes.h.inc"

auto createLowerISADecoderToProcPass() -> std::unique_ptr<mlir::Pass>;

#define GEN_PASS_REGISTRATION
#include "adl/Transforms/Proc/Passes.h.inc"

} // namespace adl::proc

#endif
