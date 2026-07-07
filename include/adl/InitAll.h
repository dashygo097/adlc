#pragma once

#ifndef ADL_INITALL_H
#define ADL_INITALL_H

namespace mlir {
class DialectRegistry;
}

namespace adl {

auto registerAllDialects(mlir::DialectRegistry &registry) -> void;
auto registerAllPasses() -> void;
auto registerAllPipelines() -> void;

} // namespace adl

#endif
