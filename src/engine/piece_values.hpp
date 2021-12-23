
#pragma once

#include "piece.hpp"
#include "square.hpp"
#include "types.hpp"

#include <array>

// Source: https://www.chessprogramming.org/Simplified_Evaluation_Function
enum {
  PAWN_VALUE = 100,
  KNIGHT_VALUE = 320,
  BISHOP_VALUE = 330,
  ROOK_VALUE = 500,
  QUEEN_VALUE = 900,
  KING_VALUE = 0,
};

static const int base_piece_values[16] = {
    QUEEN_VALUE,   ROOK_VALUE,    PAWN_VALUE,  0, //
    BISHOP_VALUE,  KNIGHT_VALUE,  0,           0, // White pieces
    -QUEEN_VALUE,  -ROOK_VALUE,   -PAWN_VALUE, 0, //
    -BISHOP_VALUE, -KNIGHT_VALUE, 0,           0, // Black pieces
};

static const int pawn_square_table[64] = {
    800, 800, 800, 800, 800, 800, 800, 800, // Encourage promotion
    50,  50,  50,  50,  50,  50,  50,  50,  // 7
    10,  10,  20,  30,  30,  20,  10,  10,  // 6
    5,   5,   10,  25,  25,  10,  5,   5,   // 5
    0,   0,   0,   20,  20,  0,   0,   0,   // 4
    5,   -5,  -10, 0,   0,   -10, -5,  5,   // 3
    5,   10,  10,  -20, -20, 10,  10,  5,   // 2
    0,   0,   0,   0,   0,   0,   0,   0,   // 1
};

static const int knight_square_table[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, // 8
    -40, -20, 0,   0,   0,   0,   -20, -40, // 7
    -30, 0,   10,  15,  15,  10,  0,   -30, // 6
    -30, 5,   15,  20,  20,  15,  5,   -30, // 5
    -30, 0,   15,  20,  20,  15,  0,   -30, // 4
    -30, 5,   10,  15,  15,  10,  5,   -30, // 3
    -40, -20, 0,   5,   5,   0,   -20, -40, // 2
    -50, -40, -30, -30, -30, -30, -40, -50, // 1
};

static const int bishop_square_table[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, // 8
    -10, 0,   0,   0,   0,   0,   0,   -10, // 7
    -10, 0,   5,   10,  10,  5,   0,   -10, // 6
    -10, 5,   5,   10,  10,  5,   5,   -10, // 5
    -10, 0,   10,  10,  10,  10,  0,   -10, // 4
    -10, 10,  10,  10,  10,  10,  10,  -10, // 3
    -10, 5,   0,   0,   0,   0,   5,   -10, // 2
    -20, -10, -10, -10, -10, -10, -10, -20, // 1
};

static const int rook_square_table[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,  // 8
    5,  10, 10, 10, 10, 10, 10, 5,  // 7
    -5, 0,  0,  0,  0,  0,  0,  -5, // 6
    -5, 0,  0,  0,  0,  0,  0,  -5, // 5
    -5, 0,  0,  0,  0,  0,  0,  -5, // 4
    -5, 0,  0,  0,  0,  0,  0,  -5, // 3
    -5, 0,  0,  0,  0,  0,  0,  -5, // 2
    0,  0,  0,  5,  5,  0,  0,  0,  // 1
};

static const int queen_square_table[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20, // 8
    -10, 0,   0,   0,  0,  0,   0,   -10, // 7
    -10, 0,   5,   5,  5,  5,   0,   -10, // 6
    -5,  0,   5,   5,  5,  5,   0,   -5,  // 5
    0,   0,   5,   5,  5,  5,   0,   -5,  // 4
    -10, 5,   5,   5,  5,  5,   0,   -10, // 3
    -10, 0,   5,   0,  0,  0,   0,   -10, // 2
    -20, -10, -10, -5, -5, -10, -10, -20, // 1
};

static const int king_middlegame_square_table[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30, // 8
    -30, -40, -40, -50, -50, -40, -40, -30, // 7
    -30, -40, -40, -50, -50, -40, -40, -30, // 6
    -30, -40, -40, -50, -50, -40, -40, -30, // 5
    -20, -30, -30, -40, -40, -30, -30, -20, // 4
    -10, -20, -20, -20, -20, -20, -20, -10, // 3
    20,  20,  0,   0,   0,   0,   20,  20,  // 2
    20,  30,  10,  0,   0,   10,  30,  20,  // 1
};

static const int king_endgame_square_table[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50, // 8
    -30, -20, -10, 0,   0,   -10, -20, -30, // 7
    -30, -10, 20,  30,  30,  20,  -10, -30, // 6
    -30, -10, 30,  40,  40,  30,  -10, -30, // 5
    -30, -10, 30,  40,  40,  30,  -10, -30, // 4
    -30, -10, 20,  30,  30,  20,  -10, -30, // 3
    -30, -30, 0,   0,   0,   0,   -30, -30, // 2
    -50, -30, -30, -30, -30, -30, -30, -50, // 1
};

static const int empty_table[64] = {0};

static const int *square_table[16] = {
    queen_square_table,
    rook_square_table,
    pawn_square_table,
    king_endgame_square_table, //
    bishop_square_table,
    knight_square_table,
    king_middlegame_square_table,
    empty_table, // White pieces
    queen_square_table,
    rook_square_table,
    pawn_square_table,
    king_endgame_square_table, //
    bishop_square_table,
    knight_square_table,
    king_middlegame_square_table,
    empty_table, // Black pieces
};

constexpr inline square_t flip_square(const square_t sq) {
  return sq + 110 - 20 * (sq / 10);
}

extern int piece_values[16][120];

void init_piece_values();
