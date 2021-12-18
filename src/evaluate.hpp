
#pragma once

#include "board.hpp"

enum {
  MATE = 90000,
  SCORE_INFINITY = MATE + 1,
};

int static_evaluate_board(const Board &board, const int side);
int evaluate_board(const Board &board, const int depth);
move_t get_best_move(const Board &board, const int depth);

// For testing only
int alpha_beta(Board &board, const int depth, int alpha = -SCORE_INFINITY,
               const int beta = SCORE_INFINITY);
