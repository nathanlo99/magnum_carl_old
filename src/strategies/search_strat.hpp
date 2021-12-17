
#pragma once

#include "../board.hpp"
#include "../evaluate.hpp"
#include "../move.hpp"
#include "strategy.hpp"

#include <vector>

class SearchStrategy : Strategy {
  const int m_depth;

public:
  SearchStrategy(const int depth) : m_depth(depth) {}
  void init(const Board &board) override {}
  move_t make_move(const Board &board,
                   const std::vector<move_t> &move_list) override {
    return get_best_move(board, m_depth);
  }
};
