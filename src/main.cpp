
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

  std::string perft_file =
      (argc > 1) ? argv[1] : "tests/perft_files/skip.perft";
  const int test_error = run_tests(perft_file, 6);
  ASSERT_MSG(!test_error, "Tests did not complete successfully");
  printf("Done testing!\n"
         "====================================================================="
         "===\n");

  const game_record result = manual_play_white();
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

  const size_t num_games = 10;
  size_t num_moves = 0;
  std::map<size_t, int> results;
  const auto diff = timeit([&] {
    for (size_t i = 0; i < num_games; ++i) {
      const auto &result = simulate_negamax();
      num_moves += result.moves.size();
      results[result.result]++;
      if (i % 1 == 0) {
        std::cout << "After " << i << " games, white won: " << results[1]
                  << ", black won: " << results[-1] << " and there were "
                  << results[0] << " draws" << std::endl;
        const std::string last_result = (result.result == 0)
                                            ? "The game was drawn"
                                        : (result.result == 1) ? "White won"
                                                               : "Black won";
        std::cout << last_result << " in " << result.moves.size() << " moves"
                  << std::endl;
      }
    }
  });
  std::cout << "Took " << (diff / 1e9) << " s "
            << "(" << (diff / num_moves) / 1e6 << " ms / move), "
            << "(" << 1e9 * num_moves / diff << "Nps)" << std::endl;
}
