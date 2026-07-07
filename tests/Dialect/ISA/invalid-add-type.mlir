// RUN: not adl-opt %s -o /dev/null 2>&1 | FileCheck %s

// CHECK: error: 'isa.add' op requires lhs and rhs to have the same type

module {
  isa.inst @BAD_ADD attributes {encoding = "bad"} {
    %a = isa.read_reg "rs1" : i32
    %b = isa.read_reg "rs2" : i64
    %r = isa.add %a, %b : i32, i64 -> i32
    isa.write_reg "rd", %r : i32
    isa.retire
  }
}
