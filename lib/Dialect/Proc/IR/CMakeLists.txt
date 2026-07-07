add_mlir_dialect_library(ADLProcDialect
  ProcDialect.cpp
  ProcOps.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/adl/Dialect/Proc/IR

  DEPENDS
  ADLProcIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRSupport
  MLIRSideEffectInterfaces
)
