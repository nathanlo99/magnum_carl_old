
#pragma once

#include "../board.hpp"
#include "../evaluate.hpp"
#include "../move.hpp"
#include "strategy.hpp"

#include <vector>

class SearchStrategy : Strategy {
  const int m_depth;
  const float m_seconds;

public:
  SearchStrategy(const int depth, const float max_seconds)
      : m_depth(depth), m_seconds(max_seconds) {}
  void init(const Board &board) override {}
  move_t make_move(const Board &board,
                   const std::vector<move_t> &move_list) override {
    return get_best_move(board, m_depth, m_seconds);
  }
};
