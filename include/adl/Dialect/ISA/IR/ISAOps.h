#pragma once

#ifndef ADL_DIALECT_ISA_IR_ISAOPS_H
#define ADL_DIALECT_ISA_IR_ISAOPS_H

#include "adl/Dialect/ISA/IR/ISADialect.h"

#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"

#define GET_OP_CLASSES
#include "adl/Dialect/ISA/IR/ISAOps.h.inc"

#endif
