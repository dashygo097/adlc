// RUN: not adl-opt %s --adl-verify-decoder-table -o /dev/null 2>&1 | FileCheck %s

// CHECK: error: invalid decoder table entry: BAD: encoding contains field with unknown width

module {
  isa.inst @BAD attributes {encoding = "imm rs1:5 010 rd:5 0000011"} {
    %base = isa.read_reg "rs1" : i32
    isa.write_reg "rd", %base : i32
    isa.retire
  }
}
