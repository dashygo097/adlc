#ifndef ADL_TRANSFORMS_PROC_PASSES_TD
#define ADL_TRANSFORMS_PROC_PASSES_TD

include "mlir/Pass/PassBase.td"

def LowerISADecoderToProc
    : Pass<"adl-lower-isa-decoder-to-proc", "mlir::ModuleOp"> {
  let summary = "Lower materialized ISA decoder metadata to proc dialect";

  let description = [{
    This pass consumes isa.decoder_table and isa.decoder_entry operations and
    emits a processor-level proc.decoder containing proc.decode_entry
    operations.
  }];

  let constructor = "adl::proc::createLowerISADecoderToProcPass()";

  let dependentDialects = [
    "adl::isa::ISADialect",
    "adl::proc::ProcDialect"
  ];
}

def ExpandProcDecoder
    : Pass<"adl-expand-proc-decoder", "mlir::ModuleOp"> {
  let summary = "Expand proc decoder metadata into executable decode IR";

  let description = [{
    This pass consumes proc.decode_entry metadata and emits proc.match and
    proc.extract_field operations inside proc.decoder.
  }];

  let constructor = "adl::proc::createExpandProcDecoderPass()";

  let dependentDialects = [
    "adl::proc::ProcDialect"
  ];
}

#endif
