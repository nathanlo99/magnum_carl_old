
#pragma once

#include "board.hpp"

const static int WHITE_MATE = 90000;
const static int BLACK_MATE = -90000;
const static int SCORE_INFINITY = 1000000;

int static_evaluate_board(const Board &board, const int side);
int evaluate_board(const Board &board, move_t &best_move);
move_t get_best_move(const Board &board);
