
#pragma once

#include "board.hpp"

#include <iomanip>
#include <sstream>
#include <string>

enum {
  MATE = 1000000,
  MATE_OFFSET = 50,
  MATE_THRESHOLD = MATE - 100 * MATE_OFFSET,
  SCORE_INFINITY = MATE,
};

constexpr inline int mate_in(const int num_moves) {
  return MATE - num_moves * MATE_OFFSET;
}

inline std::string eval_to_string(const int value) {
  std::stringstream result;
  if (value >= MATE_THRESHOLD) {
    result << "#" << (MATE - value) / MATE_OFFSET;
  } else if (value <= -MATE_THRESHOLD) {
    result << "#-" << (MATE + value) / MATE_OFFSET;
  } else {
    result << std::fixed << std::setprecision(2) << (value / 100.0);
  }
  return result.str();
}

int static_evaluate_board(const Board &board, const int side);
int evaluate_board(const Board &board, const int depth);
move_t get_best_move(const Board &board, const int depth, const float seconds);

// For testing only
int alpha_beta(Board &board, const int depth, int alpha = -SCORE_INFINITY,
               const int beta = SCORE_INFINITY);