
#pragma once

#include "../board.hpp"
#include "../move.hpp"
#include <vector>

class Strategy {
public:
  virtual void init(const Board &board) = 0;
  virtual move_t make_move(const Board &board,
                           const std::vector<move_t> &move_list) = 0;
};
