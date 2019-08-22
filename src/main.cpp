
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

  std::cout << starting_board << std::endl;
  std::cout << board1 << std::endl;
  std::cout << board2 << std::endl;
  std::cout << board3 << std::endl;

  std::cout << starting_board.fen() << std::endl;
  std::cout << board1.fen() << std::endl;
  std::cout << board2.fen() << std::endl;
  std::cout << board3.fen() << std::endl;

  std::cout << starting_board.hash() << std::endl;
  std::cout << board1.hash() << std::endl;
  std::cout << board2.hash() << std::endl;
  std::cout << board3.hash() << std::endl;
}
