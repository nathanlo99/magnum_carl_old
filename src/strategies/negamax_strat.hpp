
#pragma once

#include "../board.hpp"
#include "../evaluate.hpp"
#include "../move.hpp"
#include "strategy.hpp"

#include <vector>

class NegamaxStrategy : Strategy {
public:
  void init(const Board &board) override {}
  move_t make_move(const Board &board,
                   const std::vector<move_t> &move_list) override {
    const move_t best_move = get_best_move(board);
    return best_move;
  }
};
