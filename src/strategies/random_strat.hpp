
#pragma once

#include "strategies/strategy.hpp"
#include "hash.hpp"
#include "board.hpp"
#include "move.hpp"
#include <vector>

class RandomStrategy : Strategy {
public:
  void init(Board board) override {}
  size_t choose(Board board, const std::vector<move_t> &move_list) override {
    return random_hash() % move_list.size();
  }
};
