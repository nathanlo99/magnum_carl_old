
#pragma once

#include "../board.hpp"
#include "../evaluate.hpp"
#include "../move.hpp"
#include "strategy.hpp"

#include <vector>

class SearchStrategy : Strategy {
public:
  void init(const Board &board) override {}
  move_t make_move(const Board &board,
                   const std::vector<move_t> &move_list) override {
    move_t best_move;
    evaluate_board(board, best_move);
    // std::cout << "Search strategy evaluates this position at " << value
    //           << " in its favour" << std::endl;
    return best_move;
  }
};
