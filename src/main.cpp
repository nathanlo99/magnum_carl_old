
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

#ifdef DEBUG
uint64_t NUM_ASSERTS = 0;
#endif

int main() {
  init_hash();

  ASSERT_MSG(!run_tests(), "Tests did not complete successfully");
  printf("Done testing!\n");

}
