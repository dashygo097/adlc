// RUN: not adl-opt %s --adl-verify-decoder-table -o /dev/null 2>&1 | FileCheck %s

// CHECK: error: decoder conflict between 'LW' and 'LW_ALIAS'
// CHECK-SAME: common-mask = 0x0000707F
// CHECK-SAME: lhs-value = 0x00002003
// CHECK-SAME: rhs-value = 0x00002003

module {
  isa.inst @LW attributes {encoding = "imm[11:0] rs1:5 010 rd:5 0000011"} {
    %base = isa.read_reg "rs1" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    %data = isa.load %addr {bytes = 4 : i64, is_signed = true} : i32 -> i32
    isa.write_reg "rd", %data : i32
    isa.retire
  }

  isa.inst @LW_ALIAS attributes {
    encoding = "imm[11:0] rs1:5 010 rd:5 0000011"
  } {
    %base = isa.read_reg "rs1" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    %data = isa.load %addr {bytes = 4 : i64, is_signed = true} : i32 -> i32
    isa.write_reg "rd", %data : i32
    isa.retire
  }
}
