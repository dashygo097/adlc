// RUN: adl-opt %s --adl-materialize-decoder-table | FileCheck %s

// CHECK: isa.decoder_table {width = 32 : i64}

// CHECK: isa.decoder_entry @LW
// CHECK-SAME: fields = [
// CHECK-SAME: {inst_lsb = 20 : i64, inst_msb = 31 : i64, name = "imm", src_lsb = 0 : i64, src_msb = 11 : i64}
// CHECK-SAME: {inst_lsb = 15 : i64, inst_msb = 19 : i64, name = "rs1", src_width = 5 : i64}
// CHECK-SAME: {inst_lsb = 7 : i64, inst_msb = 11 : i64, name = "rd", src_width = 5 : i64}
// CHECK-SAME: mask = "0x0000707F"
// CHECK-SAME: value = "0x00002003"
// CHECK-SAME: width = 32 : i64

// CHECK: isa.decoder_entry @SW
// CHECK-SAME: fields = [
// CHECK-SAME: {inst_lsb = 25 : i64, inst_msb = 31 : i64, name = "imm", src_lsb = 5 : i64, src_msb = 11 : i64}
// CHECK-SAME: {inst_lsb = 20 : i64, inst_msb = 24 : i64, name = "rs2", src_width = 5 : i64}
// CHECK-SAME: {inst_lsb = 15 : i64, inst_msb = 19 : i64, name = "rs1", src_width = 5 : i64}
// CHECK-SAME: {inst_lsb = 7 : i64, inst_msb = 11 : i64, name = "imm", src_lsb = 0 : i64, src_msb = 4 : i64}
// CHECK-SAME: mask = "0x0000707F"
// CHECK-SAME: value = "0x00002023"
// CHECK-SAME: width = 32 : i64

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
