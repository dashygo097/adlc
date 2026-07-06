#include "adl/InitAll.h"

#include "adl/Analysis/ISA/Passes.h"
#include "adl/Dialect/ISA/IR/ISADialect.h"

#include "mlir/IR/DialectRegistry.h"

namespace adl {

void registerAllDialects(mlir::DialectRegistry &registry) {
  registry.insert<adl::isa::ISADialect>();
}

void registerAllPasses() { adl::isa::registerISAAnalysisPasses(); }

} // namespace adl
