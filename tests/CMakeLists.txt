configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py
  MAIN_CONFIG
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py
)

set(ADL_TEST_DEPENDS
  adl-opt
  FileCheck
  count
  not
)

add_lit_testsuite(check-adl "Running ADL regression tests"
  ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS
  ${ADL_TEST_DEPENDS}
)

set_target_properties(check-adl PROPERTIES FOLDER "Tests")

add_lit_testsuites(ADL ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS
  ${ADL_TEST_DEPENDS}
)
