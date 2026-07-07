add_mlir_library(ADLISAAnalysis
  Encoding.cpp
  DecoderInfo.cpp
  InstructionInfo.cpp
  DumpInstInfo.cpp
  DumpDecoderInfo.cpp

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
