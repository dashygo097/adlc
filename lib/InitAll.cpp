#include "adl/InitAll.h"

#include "adl/Analysis/ISA/Passes.h"
#include "adl/Dialect/ISA/IR/ISADialect.h"
#include "adl/Transforms/ISA/Passes.h"

#include "mlir/IR/DialectRegistry.h"

namespace adl {

auto registerAllDialects(mlir::DialectRegistry &registry) -> void {
  registry.insert<adl::isa::ISADialect>();
}

auto registerAllPasses() -> void {
  adl::isa::registerISAAnalysisPasses();
  adl::isa::registerISATransformsPasses();
}

} // namespace adl
