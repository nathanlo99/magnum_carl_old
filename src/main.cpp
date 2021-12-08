
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>

#include "../tests/runtests.hpp"

#include "assert.hpp"
#include "board.hpp"
#include "hash.hpp"
#include "move.hpp"
#include "simulate.hpp"

int main(int argc, char *argv[]) {
  init_hash();

  std::string perft_file = (argc > 1) ? argv[1] : "tests/perft_files/big.perft";
  const int test_error = run_tests(perft_file, 6);
  ASSERT_MSG(!test_error, "Tests did not complete successfully");
  printf("Done testing!\n"
         "====================================================================="
         "===\n");

  const game_record result = manual_play();
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

  const size_t num_games = 1000000;
  size_t num_moves = 0;
  std::map<size_t, int> results;
  const auto diff = timeit([&] {
    for (size_t i = 0; i < num_games; ++i) {
      if (i % 2000 == 0) {
        std::cout << i << std::endl;
        std::cout << results[-1] << ", " << results[0] << ", " << results[1]
                  << std::endl;
      }
      const auto &result = simulate_random();
      num_moves += result.moves.size();
      results[result.result]++;
    }
  });
  std::cout << "Took " << diff << " ns "
            << "(" << diff / num_moves << " ns / move), "
            << "(" << 1e6 * num_moves / diff << "KNps)" << std::endl;
  std::cout << results[0] << ", " << results[1] << ", " << results[2]
            << std::endl;
}
