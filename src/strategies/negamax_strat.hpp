
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
    move_t best_move;
    const int value = evaluate_board(board, best_move);
    std::cout << "Negamax strategy evaluates this position at " << value
              << " in its favour" << std::endl;
    return best_move;
  }
};
