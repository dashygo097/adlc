// RUN: adl-opt %s --adl-materialize-decoder-table --adl-lower-isa-decoder-to-proc | FileCheck %s

// CHECK: proc.decoder attributes {width = 32 : i64} {

// CHECK: proc.decode_entry @LW
// CHECK-SAME: control_flow = false
// CHECK-SAME: immediates = ["imm"]
// CHECK-SAME: inst_class = "memory"
// CHECK-SAME: mask = "0x0000707F"
// CHECK-SAME: memory = "read"
// CHECK-SAME: ops = ["imm", "add", "load"]
// CHECK-SAME: reads = ["rs1"]
// CHECK-SAME: value = "0x00002003"
// CHECK-SAME: width = 32 : i64
// CHECK-SAME: writes = ["rd"]

// CHECK: proc.decode_entry @SW
// CHECK-SAME: control_flow = false
// CHECK-SAME: immediates = ["imm"]
// CHECK-SAME: inst_class = "memory"
// CHECK-SAME: mask = "0x0000707F"
// CHECK-SAME: memory = "write"
// CHECK-SAME: ops = ["imm", "add", "store"]
// CHECK-SAME: reads = ["rs1", "rs2"]
// CHECK-SAME: value = "0x00002023"
// CHECK-SAME: width = 32 : i64
// CHECK-SAME: writes = []

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
