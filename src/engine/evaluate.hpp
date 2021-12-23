
#pragma once

#include <iomanip>
#include <sstream>
#include <string>

#include "board.hpp"
#include "search_info.hpp"

enum {
  MATE = 90000,
  MATE_OFFSET = 50,
  SCORE_INFINITY = MATE,
  MAX_MATE_LENGTH = 100,
  MATE_THRESHOLD = MATE - MAX_MATE_LENGTH * MATE_OFFSET,
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
move_t get_best_move(SearchInfo &info, const Board &board);
