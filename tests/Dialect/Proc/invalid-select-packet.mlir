// RUN: not adl-opt %s -o /dev/null 2>&1 | FileCheck %s

// CHECK: error: 'proc.extract_field' op requires result width to match source slice width

module {
  proc.decoder attributes {width = 32 : i64} {
  ^bb0(%inst: i32):
    %field = proc.extract_field %inst "rs1" {
      inst_lsb = 15 : i64,
      inst_msb = 19 : i64,
      src_lsb = 0 : i64,
      src_msb = 4 : i64
    } : i32 -> i4
  }
}
