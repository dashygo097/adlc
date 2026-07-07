#ifndef ADL_TRANSFORMS_ISA_PASSES_TD
#define ADL_TRANSFORMS_ISA_PASSES_TD

include "mlir/Pass/PassBase.td"

def MaterializeDecoderTable
    : Pass<"adl-materialize-decoder-table", "mlir::ModuleOp"> {
  let summary = "Materialize ADL ISA decoder table metadata";

  let description = [{
    This pass builds a verified module-level decoder table from isa.inst
    encoding attributes and materializes it as isa.decoder_table and
    isa.decoder_entry operations.
  }];

  let constructor = "adl::isa::createMaterializeDecoderTablePass()";

  let dependentDialects = [
    "adl::isa::ISADialect"
  ];
}

#endif
