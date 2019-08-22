
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "../tests/runtests.h"

#include "board.h"
#include "hash.h"

int main() {
  init_hash();

  ASSERT_MSG(!run_tests(), "Tests did not complete successfully");
  printf("Done testing!\n");

  Board starting_board;
  Board board1{"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"};
  Board board2{"rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"};
  Board board3{"rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"};

  std::cout << "sizeof(Board) = " << sizeof(Board) << std::endl;
  std::cout << starting_board << std::endl;
  std::cout << board1 << std::endl;
  std::cout << board2 << std::endl;
  std::cout << board3 << std::endl;

  std::string startFEN = starting_board.fen();
  Board copy{startFEN};
  std::string startFEN2 = copy.fen();
  ASSERT(startFEN == startFEN2);
  ASSERT(starting_board.hash() == copy.hash());
}
