#include "adl/Analysis/ISA/Passes.h"
#include "adl/Dialect/ISA/IR/ISADialect.h"

#include "mlir/IR/DialectRegistry.h"
#include "mlir/Support/LogicalResult.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"

int main(int argc, char **argv) {
  mlir::DialectRegistry registry;
  registry.insert<adl::isa::ISADialect>();

  adl::isa::registerISAAnalysisPasses();

  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "ADL optimizer driver\n", registry));
}
