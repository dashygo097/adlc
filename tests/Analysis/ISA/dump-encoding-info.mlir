// RUN: adl-opt %s --adl-dump-inst-info -o /dev/null | FileCheck %s

// CHECK: LW:
// CHECK: encoding: imm[11:0] rs1:5 010 rd:5 0000011
// CHECK: encoding-width: 32
// CHECK: reads: rs1
// CHECK: writes: rd
// CHECK: immediates: imm
// CHECK: class: memory

// CHECK: SW:
// CHECK: encoding: imm[11:5] rs2:5 rs1:5 010 imm[4:0] 0100011
// CHECK: encoding-width: 32
// CHECK: reads: rs1, rs2
// CHECK: writes: none
// CHECK: immediates: imm
// CHECK: class: memory

module {
  isa.inst @LW attributes {encoding = "imm[11:0] rs1:5 010 rd:5 0000011"} {
    %base = isa.read_reg "rs1" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    %data = isa.load %addr {bytes = 4 : i64, is_signed = true} : i32 -> i32
    isa.write_reg "rd", %data : i32
    isa.retire
  }

  isa.inst @SW attributes {
    encoding = "imm[11:5] rs2:5 rs1:5 010 imm[4:0] 0100011"
  } {
    %base = isa.read_reg "rs1" : i32
    %data = isa.read_reg "rs2" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    isa.store %addr, %data {bytes = 4 : i64} : i32, i32
    isa.retire
  }
}
