// RUN: adl-opt %s --adl-dump-inst-info -o /dev/null | FileCheck %s

// CHECK: ADDI:
// CHECK: reads: rs1
// CHECK: writes: rd
// CHECK: immediates: imm
// CHECK: ops: imm, add
// CHECK: class: alu
// CHECK: memory: none
// CHECK: control-flow: none

module {
  isa.inst @ADDI attributes {encoding = "imm rs1 000 rd 0010011"} {
    %base = isa.read_reg "rs1" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %result = isa.add %base, %imm : i32, i32 -> i32
    isa.write_reg "rd", %result : i32
    isa.retire
  }
}
