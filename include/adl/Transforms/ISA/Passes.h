#pragma once

#ifndef ADL_TRANSFORMS_ISA_PASSES_H
#define ADL_TRANSFORMS_ISA_PASSES_H

#include "mlir/Pass/Pass.h"

#include <memory>

namespace adl::isa {

#define GEN_PASS_DECL
#include "adl/Transforms/ISA/Passes.h.inc"

auto createMaterializeDecoderTablePass() -> std::unique_ptr<mlir::Pass>;

#define GEN_PASS_REGISTRATION
#include "adl/Transforms/ISA/Passes.h.inc"

} // namespace adl::isa

#endif
