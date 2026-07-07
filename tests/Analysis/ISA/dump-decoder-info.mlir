// RUN: adl-opt %s --adl-dump-decoder-info -o /dev/null | FileCheck %s

// CHECK: LW:
// CHECK: decoder-width: 32
// CHECK: decoder-mask: 0x0000707F
// CHECK: decoder-value: 0x00002003
// CHECK: fields:
// CHECK: imm[11:0] -> inst[31:20]
// CHECK: rs1:5 -> inst[19:15]
// CHECK: rd:5 -> inst[11:7]

// CHECK: SW:
// CHECK: decoder-width: 32
// CHECK: decoder-mask: 0x0000707F
// CHECK: decoder-value: 0x00002023
// CHECK: fields:
// CHECK: imm[11:5] -> inst[31:25]
// CHECK: rs2:5 -> inst[24:20]
// CHECK: rs1:5 -> inst[19:15]
// CHECK: imm[4:0] -> inst[11:7]

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
