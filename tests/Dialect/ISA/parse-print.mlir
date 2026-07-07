// RUN: adl-opt %s | FileCheck %s

// CHECK-LABEL: isa.inst @ADD
// CHECK-SAME: attributes {encoding = "0000000 rs2 rs1 000 rd 0110011"}
// CHECK: %[[A:.*]] = isa.read_reg "rs1" : i32
// CHECK: %[[B:.*]] = isa.read_reg "rs2" : i32
// CHECK: %[[R:.*]] = isa.add %[[A]], %[[B]] : i32, i32 -> i32
// CHECK: isa.write_reg "rd", %[[R]] : i32
// CHECK: isa.retire

module {
  isa.inst @ADD attributes {encoding = "0000000 rs2 rs1 000 rd 0110011"} {
    %a = isa.read_reg "rs1" : i32
    %b = isa.read_reg "rs2" : i32
    %r = isa.add %a, %b : i32, i32 -> i32
    isa.write_reg "rd", %r : i32
    isa.retire
  }
}
