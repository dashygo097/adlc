add_mlir_library(ADLProcTransforms
  LowerISADecoderToProc.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/adl/Transforms/Proc

  DEPENDS
  ADLProcTransformsPassIncGen

  LINK_LIBS PUBLIC
  ADLISAAnalysis
  ADLISADialect
  ADLProcDialect
  MLIRIR
  MLIRPass
  MLIRSupport
)
