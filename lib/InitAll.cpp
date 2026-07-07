#include "adl/InitAll.h"

#include "adl/Analysis/ISA/Passes.h"
#include "adl/Dialect/ISA/IR/ISADialect.h"
#include "adl/Dialect/Proc/IR/ProcDialect.h"
#include "adl/Transforms/ISA/Passes.h"
#include "adl/Transforms/Proc/Passes.h"

#include "mlir/IR/DialectRegistry.h"

namespace adl {

auto registerAllDialects(mlir::DialectRegistry &registry) -> void {
  registry.insert<adl::isa::ISADialect, adl::proc::ProcDialect>();
}

auto registerAllPasses() -> void {
  adl::isa::registerISAAnalysisPasses();
  adl::isa::registerISATransformsPasses();
  adl::proc::registerProcTransformsPasses();
}

} // namespace adl
