
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "../tests/runtests.hpp"

#include "assert.hpp"
#include "board.hpp"
#include "hash.hpp"
#include "move.hpp"
#include "simulate.hpp"

int main() {
  init_hash();

  const int test_error = run_tests("tests/fast_perft.txt", 1000);
  ASSERT_MSG(!test_error, "Tests did not complete successfully");
  printf("Done testing!\n"
  "========================================================================\n");
  return 0;

  // const game_record result = manual_play();
  // std::cout << "Result: " << result.result << std::endl;

  const size_t num_games = 1000000;
  size_t num_moves = 0;
  std::array<size_t, 3> results = {0};
  const auto diff = timeit([&]{
    for (size_t i = 0; i < num_games; ++i) {
      if (i % 2000 == 0) {
        std::cout << i << std::endl;
        std::cout << results[0] << ", " << results[1] << ", " << results[2] << std::endl;
      }
      const auto& result = simulate_random();
      num_moves += result.moves.size();
      results[result.result + 1]++;
    }
  });
  std::cout << "Took " << diff << " ns " << "(" << diff / num_moves << " ns / move" << "), " << "(" << 1e6 * num_moves / diff << "KNps" << ")" << "\n";
  std::cout << results[0] << ", " << results[1] << ", " << results[2] << std::endl;
}
