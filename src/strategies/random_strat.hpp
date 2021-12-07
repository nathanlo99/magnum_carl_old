
#pragma once

#include "../board.hpp"
#include "../hash.hpp"
#include "../move.hpp"
#include "strategy.hpp"
#include <vector>

class RandomStrategy : Strategy {
public:
  void init(const Board &board) override {}
  move_t make_move(const Board &board,
                   const std::vector<move_t> &move_list) override {
    return move_list[random_hash() % move_list.size()];
  }
};
