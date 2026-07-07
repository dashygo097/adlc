#include "adl/Dialect/Proc/IR/ProcOps.h"

#include "mlir/IR/Builders.h"
#include "mlir/IR/Diagnostics.h"
#include "mlir/IR/OpDefinition.h"

using namespace mlir;
using namespace adl::proc;

#define GET_OP_CLASSES
#include "adl/Dialect/Proc/IR/ProcOps.cpp.inc"
