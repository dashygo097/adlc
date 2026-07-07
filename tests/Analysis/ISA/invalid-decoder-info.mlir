// RUN: adl-opt %s --adl-dump-decoder-info -o /dev/null | FileCheck %s

// CHECK: BAD:
// CHECK: decoder: invalid
// CHECK: decoder-error: encoding contains field with unknown width

module {
  isa.inst @BAD attributes {encoding = "imm rs1:5 010 rd:5 0000011"} {
    %base = isa.read_reg "rs1" : i32
    isa.write_reg "rd", %base : i32
    isa.retire
  }
}
