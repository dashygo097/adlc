include_guard(GLOBAL)

function(adl_collect_mlir_opt_libs out_var)
  get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
  get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)

  set(${out_var}
    ${dialect_libs}
    ${conversion_libs}
    MLIROptLib
    PARENT_SCOPE
  )
endfunction()

function(add_adl_tool tool_name)
  cmake_parse_arguments(
    ADL_TOOL
    ""
    ""
    "SOURCES;LINK_LIBS"
    ${ARGN}
  )

  if(ADL_TOOL_UNPARSED_ARGUMENTS)
    list(APPEND ADL_TOOL_SOURCES ${ADL_TOOL_UNPARSED_ARGUMENTS})
  endif()

  if(NOT ADL_TOOL_SOURCES)
    message(FATAL_ERROR "add_adl_tool(${tool_name}) requires SOURCES or positional source files")
  endif()

  add_llvm_executable(${tool_name}
    ${ADL_TOOL_SOURCES}
  )

  llvm_update_compile_flags(${tool_name})

  if(ADL_TOOL_LINK_LIBS)
    target_link_libraries(${tool_name}
      PRIVATE
      ${ADL_TOOL_LINK_LIBS}
    )
  endif()

  set_target_properties(${tool_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  )

  if(COMMAND mlir_check_all_link_libraries)
    mlir_check_all_link_libraries(${tool_name})
  endif()
endfunction()

function(add_adl_opt_tool tool_name)
  cmake_parse_arguments(
    ADL_OPT_TOOL
    ""
    ""
    "SOURCES;LINK_LIBS"
    ${ARGN}
  )

  if(ADL_OPT_TOOL_UNPARSED_ARGUMENTS)
    list(APPEND ADL_OPT_TOOL_SOURCES ${ADL_OPT_TOOL_UNPARSED_ARGUMENTS})
  endif()

  adl_collect_mlir_opt_libs(mlir_opt_libs)

  add_adl_tool(${tool_name}
    SOURCES
    ${ADL_OPT_TOOL_SOURCES}

    LINK_LIBS
    ${mlir_opt_libs}
    ADLISADialect
    ${ADL_OPT_TOOL_LINK_LIBS}
  )
endfunction()
