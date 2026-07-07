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

class ISA_Op<string mnemonic, list<Trait> traits = []> : Op<ISADialect, mnemonic, traits>;

def ISA_InstOp : ISA_Op<"inst", [NoTerminator]> {
  let summary = "Instruction semantic container";

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

def ISA_AddOp : ISA_Op<"add", [Pure]> {
  let summary = "ISA-level integer addition";

  let arguments = (ins AnyInteger:$lhs, AnyInteger:$rhs);
  let results = (outs AnyInteger:$result);

  let hasVerifier = 1;

  let assemblyFormat = "$lhs `,` $rhs attr-dict `:` type($lhs) `,` type($rhs) `->` type($result)";
}

def ISA_LoadOp : ISA_Op<"load"> {
  let summary = "ISA-level memory load effect";

  let arguments = (ins AnyInteger:$addr, I64Attr:$bytes, OptionalAttr<BoolAttr>:$is_signed);
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

#endif
