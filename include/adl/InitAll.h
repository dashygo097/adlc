#pragma once

#ifndef ADL_INITALL_H
#define ADL_INITALL_H

namespace mlir {
class DialectRegistry;
} // namespace mlir

namespace adl {

auto registerAllDialects(mlir::DialectRegistry &registry) -> void;
auto registerAllPasses() -> void;

} // namespace adl

#endif
