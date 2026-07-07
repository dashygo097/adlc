// RUN: adl-opt %s --adl-dump-inst-info -o /dev/null | FileCheck %s

// CHECK: LW:
// CHECK: reads: rs1
// CHECK: writes: rd
// CHECK: immediates: imm
// CHECK: ops: imm, add, load
// CHECK: class: memory
// CHECK: memory: read
// CHECK: control-flow: none

// CHECK: SW:
// CHECK: reads: rs1, rs2
// CHECK: writes: none
// CHECK: immediates: imm
// CHECK: ops: imm, add, store
// CHECK: class: memory
// CHECK: memory: write
// CHECK: control-flow: none

module {
  isa.inst @LW attributes {encoding = "imm rs1 010 rd 0000011"} {
    %base = isa.read_reg "rs1" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    %data = isa.load %addr {bytes = 4 : i64, is_signed = true} : i32 -> i32
    isa.write_reg "rd", %data : i32
    isa.retire
  }

  isa.inst @SW attributes {encoding = "imm rs2 rs1 010 imm 0100011"} {
    %base = isa.read_reg "rs1" : i32
    %data = isa.read_reg "rs2" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    isa.store %addr, %data {bytes = 4 : i64} : i32, i32
    isa.retire
  }
}
