#include "adl/Dialect/Proc/IR/ProcDialect.h"

#include "adl/Dialect/Proc/IR/ProcOps.h"

using namespace mlir;
using namespace adl::proc;

#include "adl/Dialect/Proc/IR/ProcDialect.cpp.inc"

auto ProcDialect::initialize() -> void {
  addOperations<
#define GET_OP_LIST
#include "adl/Dialect/Proc/IR/ProcOps.cpp.inc"
      >();
}
