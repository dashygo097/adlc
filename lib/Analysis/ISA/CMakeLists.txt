add_mlir_library(ADLISAAnalysis
  DumpInstInfo.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/adl/Analysis/ISA

  LINK_LIBS PUBLIC
  ADLISADialect
  MLIRIR
  MLIRPass
  MLIRSupport
)
