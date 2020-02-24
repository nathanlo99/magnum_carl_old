
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "../tests/runtests.hpp"

#include "assert.hpp"
#include "board.hpp"
#include "hash.hpp"
#include "move.hpp"

/*
TODOS:
  - Move generation
  - Play move
  - Check
  - Undoing moves
  - ...
*/

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

}
