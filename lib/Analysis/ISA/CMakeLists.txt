add_mlir_library(ADLISAAnalysis
  InstructionInfo.cpp
  DumpInstInfo.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/adl/Analysis/ISA

  DEPENDS
  ADLISAAnalysisPassIncGen

  LINK_LIBS PUBLIC
  ADLISADialect
  MLIRIR
  MLIRPass
  MLIRSupport
)
