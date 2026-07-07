// RUN: not adl-opt %s -o /dev/null 2>&1 | FileCheck %s

// CHECK: error: 'isa.inst' op requires an isa.retire operation in its body

module {
  isa.inst @BAD attributes {encoding = "bad"} {
    %a = isa.read_reg "rs1" : i32
    isa.write_reg "rd", %a : i32
  }
}
