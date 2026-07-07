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

  let arguments = (ins I64Attr:$width);
  let regions = (region AnyRegion:$body);

  let assemblyFormat = "attr-dict-with-keyword $body";
}

def Proc_DecodeEntryOp : Proc_Op<"decode_entry"> {
  let summary = "Processor decode entry";

  let arguments = (ins
    FlatSymbolRefAttr:$inst,
    I64Attr:$width,
    StrAttr:$mask,
    StrAttr:$value,
    ArrayAttr:$fields,
    ArrayAttr:$reads,
    ArrayAttr:$writes,
    ArrayAttr:$immediates,
    ArrayAttr:$ops,
    StrAttr:$inst_class,
    StrAttr:$memory,
    BoolAttr:$control_flow
  );

  let assemblyFormat = "$inst attr-dict";
}

def Proc_MatchOp : Proc_Op<"match", [Pure]> {
  let summary = "Match an instruction word against a decoder entry";

  let arguments = (ins
    AnyInteger:$inst,
    I64Attr:$width,
    StrAttr:$mask,
    StrAttr:$value
  );

  let results = (outs I1:$matched);

  let builders = [
    OpBuilder<(ins
      "mlir::Value":$inst,
      "uint64_t":$width,
      "llvm::StringRef":$mask,
      "llvm::StringRef":$value
    ), [{
      build($_builder, $_state, $_builder.getI1Type(), inst,
            $_builder.getI64IntegerAttr(width), $_builder.getStringAttr(mask),
            $_builder.getStringAttr(value));
    }]>
  ];

  let hasVerifier = 1;

  let assemblyFormat =
      "$inst attr-dict `:` type($inst) `->` type($matched)";
}

def Proc_ExtractFieldOp : Proc_Op<"extract_field", [Pure]> {
  let summary = "Extract a decoded field from an instruction word";

  let arguments = (ins
    AnyInteger:$inst,
    StrAttr:$name,
    I64Attr:$src_msb,
    I64Attr:$src_lsb,
    I64Attr:$inst_msb,
    I64Attr:$inst_lsb
  );

  let results = (outs AnyInteger:$result);

  let hasVerifier = 1;

  let assemblyFormat =
      "$inst $name attr-dict `:` type($inst) `->` type($result)";
}

def Proc_AssembleFieldOp : Proc_Op<"assemble_field", [Pure]> {
  let summary = "Assemble split decoded field fragments";

  let arguments = (ins
    Variadic<AnyInteger>:$parts,
    StrAttr:$name,
    I64Attr:$width,
    ArrayAttr:$slices
  );

  let results = (outs AnyInteger:$result);

  let hasVerifier = 1;

  let assemblyFormat =
      "$name $parts attr-dict `:` functional-type($parts, $result)";
}

def Proc_DecodePacketOp : Proc_Op<"decode_packet"> {
  let summary = "Decoded instruction packet candidate";

  let arguments = (ins
    FlatSymbolRefAttr:$inst,
    I1:$matched,
    Variadic<AnyInteger>:$field_values,
    ArrayAttr:$field_names,
    ArrayAttr:$reads,
    ArrayAttr:$writes,
    ArrayAttr:$immediates,
    ArrayAttr:$ops,
    StrAttr:$inst_class,
    StrAttr:$memory,
    BoolAttr:$control_flow
  );

  let hasVerifier = 1;

  let assemblyFormat =
      "$inst $matched `(` $field_values `)` attr-dict `:` type($matched) `,` type($field_values)";
}

def Proc_SelectPacketOp : Proc_Op<"select_packet", [Pure]> {
  let summary = "Select one decoded packet candidate";

  let arguments = (ins
    Variadic<I1>:$matches,
    ArrayAttr:$insts,
    StrAttr:$policy
  );

  let results = (outs
    I1:$legal,
    I1:$illegal
  );

  let builders = [
    OpBuilder<(ins
      "mlir::ValueRange":$matches,
      "mlir::ArrayAttr":$insts,
      "llvm::StringRef":$policy
    ), [{
      build($_builder, $_state, $_builder.getI1Type(), $_builder.getI1Type(),
            matches, insts, $_builder.getStringAttr(policy));
    }]>
  ];

  let hasVerifier = 1;

  let assemblyFormat =
      "$matches attr-dict `:` type($matches) `->` type($legal) `,` type($illegal)";
}

#endif
