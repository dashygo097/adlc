add_mlir_dialect_library(ADLISADialect
  ISADialect.cpp
  ISAOps.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/adl/Dialect/ISA/IR

  DEPENDS
  ADLISAIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRSupport
  MLIRSideEffectInterfaces
  MLIRBytecodeOpInterface
)
