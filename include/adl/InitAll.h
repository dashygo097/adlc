#pragma once

#ifndef ADL_INITALL_H
#define ADL_INITALL_H

namespace mlir {
class DialectRegistry;
} // namespace mlir

namespace adl {

void registerAllDialects(mlir::DialectRegistry &registry);
void registerAllPasses();

} // namespace adl

#endif
