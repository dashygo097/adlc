// RUN: adl-opt %s --adl-build-proc-decoder | FileCheck %s

// CHECK: isa.decoder_table {width = 32 : i64}
// CHECK: isa.decoder_entry @LW
// CHECK: isa.decoder_entry @SW

// CHECK: proc.decoder attributes {width = 32 : i64} {

// CHECK: proc.decode_entry @LW
// CHECK-SAME: inst_class = "memory"
// CHECK-SAME: memory = "read"
// CHECK-SAME: ops = ["imm", "add", "load"]
// CHECK-SAME: reads = ["rs1"]
// CHECK-SAME: writes = ["rd"]

// CHECK: proc.decode_entry @SW
// CHECK-SAME: inst_class = "memory"
// CHECK-SAME: memory = "write"
// CHECK-SAME: ops = ["imm", "add", "store"]
// CHECK-SAME: reads = ["rs1", "rs2"]
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
