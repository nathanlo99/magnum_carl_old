
#pragma once

#include "board.hpp"

enum {
  MATE = 90000,
  SCORE_INFINITY = 1000000,
};

int static_evaluate_board(const Board &board, const int side);
int evaluate_board(const Board &board, const int depth);
move_t get_best_move(const Board &board, const int depth);

// For testing only

int negamax(Board &board, const int depth);
int alpha_beta(Board &board, const int depth, int alpha = -SCORE_INFINITY,
               const int beta = SCORE_INFINITY);
