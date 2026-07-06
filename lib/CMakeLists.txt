add_subdirectory(Dialect)
add_subdirectory(Analysis)

add_mlir_library(ADLInitAll
  InitAll.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/adl

  LINK_LIBS PUBLIC
  ADLISADialect
  ADLISAAnalysis
  MLIRIR
  MLIRPass
  MLIRSupport
)
