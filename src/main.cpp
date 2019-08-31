
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "../tests/runtests.h"

#include "assert.h"
#include "board.h"
#include "hash.h"
#include "move.h"

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

  std::cout << "Legal moves: [" << std::endl;
  for (const auto &move: starting_board.legal_moves()) {
    std::cout << "\tMove: " << move << std::endl;
  }
  std::cout << "]" << std::endl;

}
