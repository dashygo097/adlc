// RUN: not adl-opt %s -o /dev/null 2>&1 | FileCheck %s

// CHECK: error: 'proc.assemble_field' op requires assembled slices not to overlap

module {
  proc.decoder attributes {width = 32 : i64} {
  ^bb0(%inst: i32):
    %hi = proc.extract_field %inst "imm" {
      inst_lsb = 25 : i64,
      inst_msb = 31 : i64,
      src_lsb = 5 : i64,
      src_msb = 11 : i64
    } : i32 -> i7

    %lo = proc.extract_field %inst "imm" {
      inst_lsb = 7 : i64,
      inst_msb = 11 : i64,
      src_lsb = 4 : i64,
      src_msb = 8 : i64
    } : i32 -> i5

    %imm = proc.assemble_field "imm" %hi, %lo {
      slices = [
        {inst_lsb = 25 : i64, inst_msb = 31 : i64, src_lsb = 5 : i64, src_msb = 11 : i64},
        {inst_lsb = 7 : i64, inst_msb = 11 : i64, src_lsb = 4 : i64, src_msb = 8 : i64}
      ],
      width = 12 : i64
    } : (i7, i5) -> i12
  }
}
