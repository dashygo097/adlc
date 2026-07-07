set(LLVM_TARGET_DEFINITIONS ProcOps.td)

mlir_tablegen(ProcOps.h.inc -gen-op-decls)
mlir_tablegen(ProcOps.cpp.inc -gen-op-defs)
mlir_tablegen(ProcDialect.h.inc -gen-dialect-decls -dialect=proc)
mlir_tablegen(ProcDialect.cpp.inc -gen-dialect-defs -dialect=proc)

add_public_tablegen_target(ADLProcIncGen)
