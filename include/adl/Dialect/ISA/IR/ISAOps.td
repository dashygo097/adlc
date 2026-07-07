#ifndef ADL_DIALECT_ISA_IR_ISAOPS_TD
#define ADL_DIALECT_ISA_IR_ISAOPS_TD

include "mlir/IR/OpBase.td"
include "mlir/Interfaces/SideEffectInterfaces.td"

def ISADialect : Dialect {
  let name = "isa";
  let cppNamespace = "::adl::isa";
  let summary = "Instruction-set semantic dialect for ADL";
  let description = [{
    The ISA dialect describes normalized instruction semantics extracted from
    ISA definitions such as Sail. It is intentionally independent from any
    microarchitecture topology such as ROB, rename, issue queue, or LSQ.
  }];
}

class ISA_Op<string mnemonic, list<Trait> traits = []>
    : Op<ISADialect, mnemonic, traits>;

def ISA_InstOp : ISA_Op<"inst", [NoTerminator]> {
  let summary = "Instruction semantic container";
  let description = [{
    isa.inst represents one architectural instruction. The operation owns a
    body region containing normalized ISA effects such as register reads,
    register writes, arithmetic, memory accesses, branches, traps, and retire.
  }];

  let arguments = (ins SymbolNameAttr:$sym_name);
  let regions = (region AnyRegion:$body);

  let hasVerifier = 1;

  let assemblyFormat = "$sym_name attr-dict-with-keyword $body";
}

def ISA_ReadRegOp : ISA_Op<"read_reg", [Pure]> {
  let summary = "Read an architectural register operand";

  let arguments = (ins StrAttr:$name);
  let results = (outs AnyInteger:$value);

  let assemblyFormat = "$name attr-dict `:` type($value)";
}

def ISA_WriteRegOp : ISA_Op<"write_reg"> {
  let summary = "Write an architectural register operand";

  let arguments = (ins StrAttr:$name, AnyType:$value);

  let assemblyFormat = "$name `,` $value attr-dict `:` type($value)";
}

def ISA_ImmOp : ISA_Op<"imm", [Pure]> {
  let summary = "Read a decoded immediate field";

  let arguments = (ins
    StrAttr:$name,
    I64Attr:$width,
    OptionalAttr<BoolAttr>:$is_signed
  );

  let results = (outs AnyInteger:$value);

  let hasVerifier = 1;

  let assemblyFormat = "$name attr-dict `:` type($value)";
}

def ISA_AddOp : ISA_Op<"add", [Pure]> {
  let summary = "ISA-level integer addition";

  let arguments = (ins AnyInteger:$lhs, AnyInteger:$rhs);
  let results = (outs AnyInteger:$result);

  let hasVerifier = 1;

  let assemblyFormat =
      "$lhs `,` $rhs attr-dict `:` type($lhs) `,` type($rhs) `->` type($result)";
}

def ISA_LoadOp : ISA_Op<"load"> {
  let summary = "ISA-level memory load effect";

  let arguments = (ins
    AnyInteger:$addr,
    I64Attr:$bytes,
    OptionalAttr<BoolAttr>:$is_signed
  );

  let results = (outs AnyInteger:$result);

  let hasVerifier = 1;

  let assemblyFormat = "$addr attr-dict `:` type($addr) `->` type($result)";
}

def ISA_StoreOp : ISA_Op<"store"> {
  let summary = "ISA-level memory store effect";

  let arguments = (ins AnyInteger:$addr, AnyInteger:$value, I64Attr:$bytes);

  let hasVerifier = 1;

  let assemblyFormat = "$addr `,` $value attr-dict `:` type($addr) `,` type($value)";
}

def ISA_RetireOp : ISA_Op<"retire"> {
  let summary = "Mark normal architectural retirement";

  let assemblyFormat = "attr-dict";
}

def ISA_DecoderTableOp : ISA_Op<"decoder_table"> {
  let summary = "Materialized ISA decoder table metadata";

  let description = [{
    isa.decoder_table is derived metadata generated from isa.inst encoding
    attributes. It is intended to be consumed by later decoder-generation
    passes, not written by hand.
  }];

  let arguments = (ins I64Attr:$width);

  let assemblyFormat = "attr-dict";
}

def ISA_DecoderEntryOp : ISA_Op<"decoder_entry"> {
  let summary = "Materialized decoder entry for one instruction";

  let description = [{
    isa.decoder_entry records the decoder match metadata for one instruction.
    It references the instruction symbol instead of defining a new symbol.
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
