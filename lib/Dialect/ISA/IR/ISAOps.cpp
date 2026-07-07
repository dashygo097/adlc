#include "adl/Dialect/ISA/IR/ISAOps.h"

#include "mlir/IR/Builders.h"
#include "mlir/IR/Diagnostics.h"

#include <iterator>

using namespace mlir;
using namespace adl::isa;

#define GET_OP_CLASSES
#include "adl/Dialect/ISA/IR/ISAOps.cpp.inc"

auto AddOp::verify() -> LogicalResult {
  if (getLhs().getType() != getRhs().getType()) {
    return emitOpError("requires lhs and rhs to have the same type");
  }

  if (getResult().getType() != getLhs().getType()) {
    return emitOpError(
        "requires result type to matcLogicalResulth operand types");
  }

  return success();
}

auto LoadOp::verify() -> LogicalResult {
  if (getBytes() == 0) {
    return emitOpError("requires non-zero byte width");
  }

  return success();
}

auto StoreOp::verify() -> LogicalResult {
  if (getBytes() == 0) {
    return emitOpError("requires non-zero byte width");
  }

  return success();
}

auto InstOp::verify() -> LogicalResult {
  Region &body = getBody();

  if (body.empty()) {
    return emitOpError("requires a non-empty body");
  }

  if (std::next(body.begin()) != body.end()) {
    return emitOpError("requires exactly one body block");
  }

  bool hasRetire = false;

  for (Operation &op : body.front().getOperations()) {
    if (mlir::isa<RetireOp>(op)) {
      hasRetire = true;
      break;
    }
  }

  if (!hasRetire) {
    return emitOpError("requires an isa.retire operation in its body");
  }

  return success();
}
