
#include "test_pieces.h"
#include "test_squares.h"

int run_tests() {
  int fail_flag = 0;
  fail_flag |= test_pieces();
  fail_flag |= test_squares();
  return fail_flag;
}
