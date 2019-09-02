
#include "test_pieces.h"
#include "test_squares.h"
#include "test_board.h"
#include "test_perft.h"

int run_tests() {
  int fail_flag = 0;
  fail_flag |= test_pieces();
  fail_flag |= test_squares();
  fail_flag |= test_board();
  fail_flag |= test_perft();
  return fail_flag;
}
