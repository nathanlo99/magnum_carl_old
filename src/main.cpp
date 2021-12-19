
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>

#include "../tests/runtests.hpp"

#include "assert.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "hash.hpp"
#include "move.hpp"
#include "opening_book.hpp"
#include "simulate.hpp"

void test_position(std::string fen) {
  Board test_board(fen);
  const move_t best_move = get_best_move(test_board, 16, 150.0);
  std::cout << test_board << std::endl;
  std::cout << "Best move is " << test_board.algebraic_notation(best_move)
            << std::endl;
}

int main(int argc, char *argv[]) {
  init_hash();
  // opening_book.read("references/book/processed_games.txt", 12);

  std::string perft_file =
      (argc > 1) ? argv[1] : "tests/perft_files/skip.perft";
  const int test_error = run_tests(perft_file, 6);
  ASSERT_MSG(!test_error, "Tests did not complete successfully");
  printf("Done testing!\n"
         "====================================================================="
         "===\n");

  test_position("6k1/3b3r/1p1p4/p1n2p2/1PPNpP1q/P3Q1p1/1R1RB1P1/5K2 b - - 0 1");

  const game_record result = manual_play_white(100, 5.0);
  switch (result.result) {
  case -1:
    std::cout << "Black wins!" << std::endl;
    break;
  case 0:
    std::cout << "Tie!" << std::endl;
    break;
  case 1:
    std::cout << "White wins!" << std::endl;
    break;
  default:
    break;
  }

  for (int i = 0; i < 10; ++i) {
    simulate_search(100, 1.0);
  }
}
