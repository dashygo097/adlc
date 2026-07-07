add_mlir_library(ADLISATransforms
  MaterializeDecoderTable.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/adl/Transforms/ISA

  DEPENDS
  ADLISATransformsPassIncGen

  LINK_LIBS PUBLIC
  ADLISAAnalysis
  ADLISADialect
  MLIRIR
  MLIRPass
  MLIRSupport
)
