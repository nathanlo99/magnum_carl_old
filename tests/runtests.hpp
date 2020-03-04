
#include "test_pieces.hpp"
#include "test_squares.hpp"
#include "test_board.hpp"
#include "test_perft.hpp"

int run_tests() {
  int fail_flag = 0;
  fail_flag |= test_pieces();
  fail_flag |= test_squares();
  fail_flag |= test_board();
  fail_flag |= test_perft("tests/start_perft.txt");
  return fail_flag;
}
