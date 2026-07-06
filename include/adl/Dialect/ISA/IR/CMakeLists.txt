set(LLVM_TARGET_DEFINITIONS ISAOps.td)

mlir_tablegen(ISAOps.h.inc -gen-op-decls)
mlir_tablegen(ISAOps.cpp.inc -gen-op-defs)
mlir_tablegen(ISADialect.h.inc -gen-dialect-decls -dialect=isa)
mlir_tablegen(ISADialect.cpp.inc -gen-dialect-defs -dialect=isa)

add_public_tablegen_target(ADLISAIncGen)
