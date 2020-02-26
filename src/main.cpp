
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "../tests/runtests.hpp"

#include "assert.hpp"
#include "board.hpp"
#include "hash.hpp"
#include "move.hpp"

int main() {
  init_hash();

  ASSERT_MSG(!run_tests(), "Tests did not complete successfully");
  printf("Done testing!\n"
  "========================================================================\n");

  Board starting_board;
  std::cout << starting_board << std::endl;

  const auto &legal_moves = starting_board.legal_moves();
  std::cout << "Legal moves (" << legal_moves.size() << "): [" << std::endl;
  for (const auto &move: legal_moves) {
    std::cout << "  " << string_from_move(move) << std::endl;
  }
  std::cout << "]" << std::endl;

  while (true) {
    Board board = starting_board;
    for (size_t num = 0; num < 200; ++num) {
      const auto &legal_moves = board.legal_moves();
      const size_t num_moves = legal_moves.size();
      if (num_moves == 0) break;
      const auto &next_idx = random_hash() % num_moves;
      const auto &next_move = legal_moves[next_idx];
      std::cout << "Making move " << string_from_move(next_move) << std::endl;
      board.make_move(next_move);
      // std::cout << "New board:" << std::endl;
      std::cout << board << std::endl;
    }
  }
}
