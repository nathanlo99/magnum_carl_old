
#pragma once

#include "../board.hpp"
#include "../move.hpp"
#include <vector>

class Strategy {
public:
  virtual void init(Board board) = 0;
  virtual size_t choose(Board board, const std::vector<move_t> &move_list) = 0;
};
