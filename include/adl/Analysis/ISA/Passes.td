#ifndef ADL_ANALYSIS_ISA_PASSES_TD
#define ADL_ANALYSIS_ISA_PASSES_TD

include "mlir/Pass/PassBase.td"

def DumpInstInfo : Pass<"adl-dump-inst-info", "mlir::ModuleOp"> {
  let summary = "Dump inferred ADL ISA instruction information";

  let description = [{
    This pass walks isa.inst operations and prints normalized instruction
    metadata inferred from the ADL ISA dialect.
  }];

  let constructor = "adl::isa::createDumpInstInfoPass()";

  let dependentDialects = [
    "adl::isa::ISADialect"
  ];
}

def DumpDecoderInfo : Pass<"adl-dump-decoder-info", "mlir::ModuleOp"> {
  let summary = "Dump inferred ADL ISA decoder information";

  let description = [{
    This pass walks isa.inst operations and prints decoder metadata derived
    from the instruction encoding attribute.
  }];

  let constructor = "adl::isa::createDumpDecoderInfoPass()";

  let dependentDialects = [
    "adl::isa::ISADialect"
  ];
}

def DumpDecoderTable : Pass<"adl-dump-decoder-table", "mlir::ModuleOp"> {
  let summary = "Dump ADL ISA decoder table information";

  let description = [{
    This pass builds a module-level decoder table from isa.inst operations,
    prints decoder entries, and reports overlapping instruction encodings.
  }];

  let constructor = "adl::isa::createDumpDecoderTablePass()";

  let dependentDialects = [
    "adl::isa::ISADialect"
  ];
}

def VerifyDecoderTable : Pass<"adl-verify-decoder-table", "mlir::ModuleOp"> {
  let summary = "Verify ADL ISA decoder table validity";

  let description = [{
    This pass builds a module-level decoder table from isa.inst operations and
    fails if any instruction encoding is invalid or if any two instruction
    encodings overlap.
  }];

  let constructor = "adl::isa::createVerifyDecoderTablePass()";

  let dependentDialects = [
    "adl::isa::ISADialect"
  ];
}

#endif
