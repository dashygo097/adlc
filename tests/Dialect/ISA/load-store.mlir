// RUN: adl-opt %s | FileCheck %s

// CHECK-LABEL: isa.inst @LW
// CHECK-SAME: attributes {encoding = "imm rs1 010 rd 0000011"}
// CHECK: %[[BASE:.*]] = isa.read_reg "rs1" : i32
// CHECK: %[[IMM:.*]] = isa.imm "imm" {is_signed = true, width = 12 : i64} : i32
// CHECK: %[[ADDR:.*]] = isa.add %[[BASE]], %[[IMM]] : i32, i32 -> i32
// CHECK: %[[DATA:.*]] = isa.load %[[ADDR]] {bytes = 4 : i64, is_signed = true} : i32 -> i32
// CHECK: isa.write_reg "rd", %[[DATA]] : i32
// CHECK: isa.retire

// CHECK-LABEL: isa.inst @SW
// CHECK-SAME: attributes {encoding = "imm rs2 rs1 010 imm 0100011"}
// CHECK: %[[BASE:.*]] = isa.read_reg "rs1" : i32
// CHECK: %[[DATA:.*]] = isa.read_reg "rs2" : i32
// CHECK: %[[IMM:.*]] = isa.imm "imm" {is_signed = true, width = 12 : i64} : i32
// CHECK: %[[ADDR:.*]] = isa.add %[[BASE]], %[[IMM]] : i32, i32 -> i32
// CHECK: isa.store %[[ADDR]], %[[DATA]] {bytes = 4 : i64} : i32, i32
// CHECK: isa.retire

module {
  isa.inst @LW attributes {encoding = "imm rs1 010 rd 0000011"} {
    %base = isa.read_reg "rs1" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    %data = isa.load %addr {bytes = 4 : i64, is_signed = true} : i32 -> i32
    isa.write_reg "rd", %data : i32
    isa.retire
  }

  isa.inst @SW attributes {encoding = "imm rs2 rs1 010 imm 0100011"} {
    %base = isa.read_reg "rs1" : i32
    %data = isa.read_reg "rs2" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    isa.store %addr, %data {bytes = 4 : i64} : i32, i32
    isa.retire
  }
}
