#include "adl/Dialect/ISA/IR/ISAOps.h"

#include "mlir/IR/Builders.h"

using namespace mlir;
using namespace adl::isa;

#define GET_OP_CLASSES
#include "adl/Dialect/ISA/IR/ISAOps.cpp.inc"
