// RUN: adl-opt %s --adl-dump-inst-info -o /dev/null | FileCheck %s

// CHECK: ADD:
// CHECK: reads: rs1, rs2
// CHECK: writes: rd
// CHECK: ops: add
// CHECK: class: alu
// CHECK: memory: none
// CHECK: control-flow: none

module {
  isa.inst @ADD attributes {encoding = "0000000 rs2 rs1 000 rd 0110011"} {
    %a = isa.read_reg "rs1" : i32
    %b = isa.read_reg "rs2" : i32
    %r = isa.add %a, %b : i32, i32 -> i32
    isa.write_reg "rd", %r : i32
    isa.retire
  }
}
