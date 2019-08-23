
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "../tests/runtests.h"

#include "board.h"
#include "hash.h"

/*
TODOS:
  - Board tests
  - Move generation
  - Play move
  - Check
  - Undoing moves
  - ...
*/
int main() {
  init_hash();

  ASSERT_MSG(!run_tests(), "Tests did not complete successfully");
  printf("Done testing!\n");

  Board starting_board;
  Board board1{"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"};

  std::cout << "sizeof(Board) = " << sizeof(Board) << std::endl;
  std::cout << starting_board << std::endl;
  std::cout << board1 << std::endl;

  std::string startFEN = starting_board.fen();
  Board copy{startFEN};
  std::string startFEN2 = copy.fen();
  ASSERT(startFEN == startFEN2);
  ASSERT(starting_board.hash() == copy.hash());
}
