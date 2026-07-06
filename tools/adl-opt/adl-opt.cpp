#include "adl/InitAll.h"

#include "mlir/IR/DialectRegistry.h"
#include "mlir/Support/LogicalResult.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"

auto main(int argc, char **argv) -> int {
  mlir::DialectRegistry registry;
  adl::registerAllDialects(registry);
  adl::registerAllPasses();

  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "ADL optimizer driver\n", registry));
}
