
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "../tests/runtests.hpp"

#include "assert.hpp"
#include "board.hpp"
#include "hash.hpp"
#include "move.hpp"

void print_move_list(const std::vector<move_t> &move_list) {
  for (const move_t move : move_list) {
    std::cout << string_from_move(move) << std::endl;
  }
}

int manual_play(const std::string &fen = Board::startFEN) {
  std::string input;
  Board board(fen);
  while (true) {
    std::cout << board << std::endl;
    const auto &move_list = board.legal_moves();
    if (move_list.empty()) {
      if (board.is_drawn() || !board.king_in_check()) {
        return 0;
      } else {
        return (board.m_next_move_colour == WHITE) ? -1 : 1;
      }
    }

    bool move_made = false;
    while (!move_made) {
      std::getline(std::cin, input);
      for (const move_t move : move_list) {
        if (string_from_move(move) == input) {
          board.make_move(move);
          move_made = true;
          break;
        }
      }
      if (!move_made) {
        std::cout << "Invalid move; the legal moves are: " << std::endl;
        print_move_list(move_list);
      }
    }
  }
}

int simulate_random(const std::string &fen = Board::startFEN) {
  Board board(fen);
  // std::cout << board << std::endl;
  const auto &move_list = board.legal_moves();
  while (true) {
    const auto &move_list = board.legal_moves();
    if (move_list.empty()) {
      if (board.is_drawn() || !board.king_in_check()) {
        return 0;
      } else {
        if (board.m_next_move_colour == WHITE)
          return -1;
        else
          return 1;
      }
    }
    const size_t move_idx = random_hash() % move_list.size();
    board.make_move(move_list[move_idx]);
  }
}

int main() {
  init_hash();

  const int test_error = run_tests();
  ASSERT_MSG(!test_error, "Tests did not complete successfully");
  printf("Done testing!\n"
  "========================================================================\n");

  std::string input;
  std::array<int, 3> results = {0};
  for (int i = 0; i < 100000; ++i) {
    const int result = simulate_random();
    results[result + 1]++;
    std::cout << results[0] << ", " << results[1] << ", " << results[2] << std::endl;
  }
}
