
#include "evaluate.hpp"
#include "board.hpp"
#include "move.hpp"
#include "square.hpp"

#include <iostream>

static const int piece_values[16] = {
    900,  500,  100,  0, 350,  300,  1000,  0, // White pieces
    -900, -500, -100, 0, -350, -300, -1000, 0, // Black pieces
};

// Initial piece-square tables taken from
// https://adamberent.com/2019/03/02/piece-square-table/
static const int empty_table[64] = {0};

static const int pawn_table[64]{
    0,  0,  0,   0,   0,   0,   0,  0,  //
    50, 50, 50,  50,  50,  50,  50, 50, //
    10, 10, 20,  30,  30,  20,  10, 10, //
    5,  5,  10,  27,  27,  10,  5,  5,  //
    0,  0,  0,   25,  25,  0,   0,  0,  //
    5,  -5, -10, 0,   0,   -10, -5, 5,  //
    5,  10, 10,  -25, -25, 10,  10, 5,  //
    0,  0,  0,   0,   0,   0,   0,  0,  //
};

static const int knight_table[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, //
    -40, -20, 0,   0,   0,   0,   -20, -40, //
    -30, 0,   10,  15,  15,  10,  0,   -30, //
    -30, 5,   15,  20,  20,  15,  5,   -30, //
    -30, 0,   15,  20,  20,  15,  0,   -30, //
    -30, 5,   10,  15,  15,  10,  5,   -30, //
    -40, -20, 0,   5,   5,   0,   -20, -40, //
    -50, -40, -20, -30, -30, -20, -40, -50, //
};

static const int bishop_table[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, //
    -10, 0,   0,   0,   0,   0,   0,   -10, //
    -10, 0,   5,   10,  10,  5,   0,   -10, //
    -10, 5,   5,   10,  10,  5,   5,   -10, //
    -10, 0,   10,  10,  10,  10,  0,   -10, //
    -10, 10,  10,  10,  10,  10,  10,  -10, //
    -10, 5,   0,   0,   0,   0,   5,   -10, //
    -20, -10, -40, -10, -10, -40, -10, -20, //
};

static const int king_opening_table[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30, //
    -30, -40, -40, -50, -50, -40, -40, -30, //
    -30, -40, -40, -50, -50, -40, -40, -30, //
    -30, -40, -40, -50, -50, -40, -40, -30, //
    -20, -30, -30, -40, -40, -30, -30, -20, //
    -10, -20, -20, -20, -20, -20, -20, -10, //
    20,  20,  0,   0,   0,   0,   20,  20,  //
    20,  30,  10,  0,   0,   10,  30,  20,  //
};

static const int king_endgame_table[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50, //
    -30, -20, -10, 0,   0,   -10, -20, -30, //
    -30, -10, 20,  30,  30,  20,  -10, -30, //
    -30, -10, 30,  40,  40,  30,  -10, -30, //
    -30, -10, 30,  40,  40,  30,  -10, -30, //
    -30, -10, 20,  30,  30,  20,  -10, -30, //
    -30, -30, 0,   0,   0,   0,   -30, -30, //
    -50, -30, -30, -30, -30, -30, -30, -50, //
};

constexpr const int *piece_square_tables[16] = {
    empty_table,  empty_table,  pawn_table,         empty_table, //
    bishop_table, knight_table, king_opening_table, empty_table, //
    empty_table,  empty_table,  pawn_table,         empty_table, //
    bishop_table, knight_table, king_opening_table, empty_table, //
};

constexpr square_t flip_square(square_t sq) { return sq + 56 - (sq / 8) * 16; }

int static_evaluate_board(const Board &board) {
  if (board.is_drawn())
    return 0;

  int result = 0;
  for (piece_t piece = 0; piece < 16; ++piece) {
    if (!valid_piece(piece))
      continue;
    const size_t num_pieces = board.m_num_pieces[piece];
    result += num_pieces * piece_values[piece];
    for (size_t piece_idx = 0; piece_idx < num_pieces; ++piece_idx) {
      const square_t piece_location = board.m_positions[piece][piece_idx];
      if (get_side(piece) == WHITE) {
        const int value =
            piece_square_tables[piece]
                               [flip_square(get_square_64(piece_location))];
        INFO("White piece %c on square %s contributed %d\n",
             char_from_piece(piece), string_from_square(piece_location).c_str(),
             value);
        result += value;
      } else {
        const int value =
            -piece_square_tables[piece][get_square_64(piece_location)];
        INFO("Black piece %c on square %s contributed %d\n",
             char_from_piece(piece), string_from_square(piece_location).c_str(),
             value);
        result += value;
      }
    }
  }
  return result;
}

const int SEARCH_DEPTH = 4;

int negamax(Board &board, move_t &best_move, int depth) {
  if (depth == 0) {
    const int static_eval = static_evaluate_board(board);
    return board.m_next_move_colour == WHITE ? static_eval : -static_eval;
  }

  move_t tmp_move;

  int result = -1000000;
  int legal_moves = 0;
  for (const move_t next_move : board.pseudo_moves()) {
    if (board.make_move(next_move)) {
      legal_moves++;

      const int child_value = -negamax(board, tmp_move, depth - 1);
      if (child_value > result) {
        result = child_value;
        best_move = next_move;
      }
    }
    board.unmake_move();
  }

  if (legal_moves == 0) {
    best_move = 0;
    return board.king_in_check() ? -90000 : 0;
  }
  return result;
}

int evaluate_board(const Board &board) {
  Board tmp(board);
  move_t best_move;
  return negamax(tmp, best_move, SEARCH_DEPTH);
}

move_t get_best_move(const Board &board) {
  Board tmp(board);
  move_t best_move;
  negamax(tmp, best_move, SEARCH_DEPTH);
  return best_move;
}
