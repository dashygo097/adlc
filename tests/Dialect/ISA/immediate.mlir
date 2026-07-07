// RUN: adl-opt %s | FileCheck %s

// CHECK-LABEL: isa.inst @ADDI
// CHECK: isa.imm "imm"
// CHECK-SAME: is_signed = true
// CHECK-SAME: width = 12 : i64

module {
  isa.inst @ADDI attributes {encoding = "imm rs1 000 rd 0010011"} {
    %base = isa.read_reg "rs1" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %result = isa.add %base, %imm : i32, i32 -> i32
    isa.write_reg "rd", %result : i32
    isa.retire
  }
}
