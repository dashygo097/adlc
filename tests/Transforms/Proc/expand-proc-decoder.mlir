// RUN: adl-opt %s --adl-materialize-decoder-table --adl-lower-isa-decoder-to-proc --adl-expand-proc-decoder | FileCheck %s

// CHECK: proc.decoder attributes {width = 32 : i64} {
// CHECK: ^bb0(%[[INST:.*]]: i32):

// CHECK: %[[LW_MATCH:.*]] = proc.match %[[INST]]
// CHECK-SAME: value = "0x00002003"
// CHECK-SAME: : i32 -> i1

// CHECK: %[[LW_IMM:.*]] = proc.extract_field %[[INST]] "imm"
// CHECK-SAME: : i32 -> i12
// CHECK: %[[LW_RS1:.*]] = proc.extract_field %[[INST]] "rs1"
// CHECK-SAME: : i32 -> i5
// CHECK: %[[LW_RD:.*]] = proc.extract_field %[[INST]] "rd"
// CHECK-SAME: : i32 -> i5

// CHECK: proc.decode_packet @LW %[[LW_MATCH]](%[[LW_IMM]], %[[LW_RS1]], %[[LW_RD]])
// CHECK-SAME: field_names = ["imm", "rs1", "rd"]
// CHECK-SAME: inst_class = "memory"
// CHECK-SAME: memory = "read"
// CHECK-SAME: reads = ["rs1"]
// CHECK-SAME: writes = ["rd"]
// CHECK-SAME: : i1, i12, i5, i5

// CHECK: %[[SW_MATCH:.*]] = proc.match %[[INST]]
// CHECK-SAME: value = "0x00002023"
// CHECK-SAME: : i32 -> i1

// CHECK: %[[SW_IMM_HI:.*]] = proc.extract_field %[[INST]] "imm"
// CHECK-SAME: : i32 -> i7
// CHECK: %[[SW_RS2:.*]] = proc.extract_field %[[INST]] "rs2"
// CHECK-SAME: : i32 -> i5
// CHECK: %[[SW_RS1:.*]] = proc.extract_field %[[INST]] "rs1"
// CHECK-SAME: : i32 -> i5
// CHECK: %[[SW_IMM_LO:.*]] = proc.extract_field %[[INST]] "imm"
// CHECK-SAME: : i32 -> i5

// CHECK: %[[SW_IMM:.*]] = proc.assemble_field "imm" %[[SW_IMM_HI]], %[[SW_IMM_LO]]
// CHECK-SAME: width = 12 : i64
// CHECK-SAME: : (i7, i5) -> i12

// CHECK: proc.decode_packet @SW %[[SW_MATCH]](%[[SW_IMM]], %[[SW_RS2]], %[[SW_RS1]])
// CHECK-SAME: field_names = ["imm", "rs2", "rs1"]
// CHECK-SAME: inst_class = "memory"
// CHECK-SAME: memory = "write"
// CHECK-SAME: reads = ["rs1", "rs2"]
// CHECK-SAME: writes = []
// CHECK-SAME: : i1, i12, i5, i5

// CHECK: %[[LEGAL:.*]], %[[ILLEGAL:.*]] = proc.select_packet %[[LW_MATCH]], %[[SW_MATCH]]
// CHECK-SAME: insts = ["LW", "SW"]
// CHECK-SAME: policy = "priority"
// CHECK-SAME: : i1, i1 -> i1, i1

// CHECK: }

module {
  isa.inst @LW attributes {encoding = "imm[11:0] rs1:5 010 rd:5 0000011"} {
    %base = isa.read_reg "rs1" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    %data = isa.load %addr {bytes = 4 : i64, is_signed = true} : i32 -> i32
    isa.write_reg "rd", %data : i32
    isa.retire
  }

  isa.inst @SW attributes {
    encoding = "imm[11:5] rs2:5 rs1:5 010 imm[4:0] 0100011"
  } {
    %base = isa.read_reg "rs1" : i32
    %data = isa.read_reg "rs2" : i32
    %imm = isa.imm "imm" {width = 12 : i64, is_signed = true} : i32
    %addr = isa.add %base, %imm : i32, i32 -> i32
    isa.store %addr, %data {bytes = 4 : i64} : i32, i32
    isa.retire
  }
}
