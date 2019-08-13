
#include <cstdio>
#include <iostream>

#include "../tests/test_pieces.h"
#include "../tests/test_squares.h"

#include "board.h"

int main() {
  test_pieces();
  test_squares();
  printf("Done testing!\n");

  Board board;

  std::cout << board << std::endl;
}
