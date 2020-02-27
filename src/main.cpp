
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "../tests/runtests.hpp"

#include "assert.hpp"
#include "board.hpp"
#include "hash.hpp"
#include "move.hpp"

int main() {
  init_hash();

  std::stringstream test_output;
  ASSERT_MSG(!run_tests(test_output), "Tests did not complete successfully");
  INFO("Test output: %s", test_output.str().c_str());
  printf("Done testing!\n"
  "========================================================================\n");

  Board starting_board;

  for (int num_unmakes = 0; num_unmakes < 10; ++num_unmakes) {
    const auto start = std::chrono::high_resolution_clock::now();

    int num_moves = 0, checkmates = 0, stalemates = 0;
    for (size_t num_runs = 0; num_runs < 1000; ++num_runs) {
      Board board = starting_board;
      for (size_t num = 0; num < 200; ++num) {
        const auto &legal_moves = board.legal_moves();
        const size_t num_legal_moves = legal_moves.size();
        if (num_legal_moves == 0) {
          const bool current_side = board.m_next_move_colour, other_side = !current_side;
          const piece_t king_piece = (current_side == WHITE) ? WHITE_KING : BLACK_KING;
          if (board.square_attacked(board.m_positions[king_piece][0], other_side)) {
            INFO("%s wins by checkmate", (current_side == WHITE) ? "White" : "Black");
            checkmates++;
          } else {
            INFO("Draw by stalemate");
            stalemates++;
          }
          break;
        }
        while (true) {
          const auto &next_idx = random_hash() % num_legal_moves;
          const auto &next_move = legal_moves[next_idx];
          INFO("Making move %s", string_from_move(next_move).c_str());
          if (!board.make_move(next_move)) {
            board.unmake_move();
          } else {
            for (int i = 0; i < num_unmakes; ++i) {
              board.unmake_move();
              board.make_move(next_move);
            }
            num_moves++;
            break;
          }
        }
      }
    }
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
    std::cout << "Made " << num_moves << " moves in " << diff << "ns\n";
    std::cout << "  with " << num_unmakes << " unmakes\n";
    std::cout << "  for an average of " << (diff / num_moves) << "ns per move\n";
    std::cout << "Also achieved " << checkmates << " checkmates and " << stalemates << " stalemates" << "\n";
  }
}
