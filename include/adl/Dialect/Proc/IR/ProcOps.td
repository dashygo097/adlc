#ifndef ADL_DIALECT_PROC_IR_PROCOPS_TD
#define ADL_DIALECT_PROC_IR_PROCOPS_TD

include "mlir/IR/OpBase.td"
include "mlir/Interfaces/SideEffectInterfaces.td"

def ProcDialect : Dialect {
  let name = "proc";
  let cppNamespace = "::adl::proc";
  let summary = "Processor architecture dialect for ADL";
  let description = [{
    The proc dialect describes processor-level structures derived from ISA
    metadata and architecture descriptions. Unlike the ISA dialect, proc is
    allowed to describe pipeline-visible structures such as decoders, dispatch
    packets, execution resources, and later microarchitectural topology.
  }];
}

class Proc_Op<string mnemonic, list<Trait> traits = []>
    : Op<ProcDialect, mnemonic, traits>;

def Proc_DecoderOp : Proc_Op<"decoder", [NoTerminator]> {
  let summary = "Processor instruction decoder metadata";

  let description = [{
    proc.decoder groups processor-level decode entries. It is derived from
    isa.decoder_table and isa.decoder_entry operations and is the handoff point
    to later hardware decoder generation.
  }];

  let arguments = (ins I64Attr:$width);
  let regions = (region AnyRegion:$body);

  let assemblyFormat = "attr-dict-with-keyword $body";
}

def Proc_DecodeEntryOp : Proc_Op<"decode_entry"> {
  let summary = "Processor decode entry";

  let description = [{
    proc.decode_entry references an ISA instruction and records the metadata
    needed by a processor decoder: match mask, match value, and field
    extraction descriptors.
  }];

  let arguments = (ins
    FlatSymbolRefAttr:$inst,
    I64Attr:$width,
    StrAttr:$mask,
    StrAttr:$value,
    ArrayAttr:$fields
  );

  let assemblyFormat = "$inst attr-dict";
}

#endif
