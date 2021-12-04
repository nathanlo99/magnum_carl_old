
#include "test_board.hpp"
#include "test_perft.hpp"
#include "test_pieces.hpp"
#include "test_squares.hpp"

int run_tests(const std::string &perft_file, const int perft_depth) {
  int fail_flag = 0;
  fail_flag |= test_pieces();
  fail_flag |= test_squares();
  fail_flag |= test_board();
  fail_flag |= test_perft(perft_file, perft_depth);
  return fail_flag;
}
