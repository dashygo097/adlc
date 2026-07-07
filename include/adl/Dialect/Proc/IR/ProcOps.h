#pragma once

#ifndef ADL_DIALECT_PROC_IR_PROCOPS_H
#define ADL_DIALECT_PROC_IR_PROCOPS_H

#include "adl/Dialect/Proc/IR/ProcDialect.h"

#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"

#define GET_OP_CLASSES
#include "adl/Dialect/Proc/IR/ProcOps.h.inc"

#endif
