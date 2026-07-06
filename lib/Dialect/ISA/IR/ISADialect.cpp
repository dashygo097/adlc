#include "adl/Dialect/ISA/IR/ISADialect.h"
#include "adl/Dialect/ISA/IR/ISAOps.h"

#include "mlir/IR/DialectImplementation.h"

using namespace mlir;
using namespace adl::isa;

#include "adl/Dialect/ISA/IR/ISADialect.cpp.inc"

void ISADialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "adl/Dialect/ISA/IR/ISAOps.cpp.inc"
      >();
}
