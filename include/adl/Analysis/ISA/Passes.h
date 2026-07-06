#pragma once

#ifndef ADL_ANALYSIS_ISA_PASSES_H
#define ADL_ANALYSIS_ISA_PASSES_H

#include <memory>

namespace mlir {
class Pass;
} // namespace mlir

namespace adl::isa {

std::unique_ptr<mlir::Pass> createDumpInstInfoPass();

void registerISAAnalysisPasses();

} // namespace adl::isa

#endif
