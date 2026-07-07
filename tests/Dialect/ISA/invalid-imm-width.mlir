// RUN: not adl-opt %s -o /dev/null 2>&1 | FileCheck %s

// CHECK: error: 'isa.imm' op requires immediate width to fit in result type

module {
  isa.inst @BAD_IMM attributes {encoding = "bad"} {
    %imm = isa.imm "imm" {width = 64 : i64, is_signed = true} : i32
    isa.write_reg "rd", %imm : i32
    isa.retire
  }
}
