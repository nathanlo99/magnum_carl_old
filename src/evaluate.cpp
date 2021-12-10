
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

constexpr inline square_t flip_square(const square_t sq) {
  return sq + 56 - (sq / 8) * 16;
}

int static_evaluate_board(const Board &board, const int side) {
  if (board.legal_moves().empty()) {
    return board.king_in_check() ? -90000 : 0;
  } else if (board.is_drawn()) {
    return 0;
  }

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
  return (side == WHITE) ? result : -result;
}

int quiescence_search(Board &board, int alpha = -SCORE_INFINITY,
                      int beta = SCORE_INFINITY, int depth = 0) {
  const auto legal_captures =
      board.legal_moves(MOVEGEN_CAPTURES | MOVEGEN_PROMOTIONS);
  if (legal_captures.empty()) {
    if (board.legal_moves().empty()) {
      return board.king_in_check() ? -90000 : 0;
    }
    return static_evaluate_board(board, board.m_next_move_colour);
  }

  for (const move_t next_move : legal_captures) {
    board.make_move(next_move);
    const int value = -quiescence_search(board, -beta, -alpha, depth + 1);
    if (value >= beta) {
      board.unmake_move();
      return value;
    }
    if (value > alpha)
      alpha = value;
    board.unmake_move();
  }
  return alpha;
}

const int SEARCH_DEPTH = 4;

int alpha_beta(Board &board, move_t &best_move, int alpha = -SCORE_INFINITY,
               int beta = SCORE_INFINITY, int depth = SEARCH_DEPTH) {

  const auto legal_moves = board.legal_moves();
  if (legal_moves.empty()) {
    return board.king_in_check() ? -90000 : 0;
  } else if (board.is_drawn()) {
    return 0;
  } else if (depth == 0) {
    return quiescence_search(board, alpha, beta);
  }

  best_move = 0;
  move_t tmp_move;
  for (const move_t next_move : board.legal_moves()) {
    board.make_move(next_move);
    const int value = -alpha_beta(board, tmp_move, -beta, -alpha, depth - 1);
    if (value >= beta) {
      best_move = next_move;
      board.unmake_move();
      return value;
    }
    if (value > alpha) {
      alpha = value;
      best_move = next_move;
    }
    board.unmake_move();
  }
  return alpha;
}

int evaluate_board(const Board &board, move_t &best_move) {
  Board tmp(board);
  return alpha_beta(tmp, best_move);
}

move_t get_best_move(const Board &board) {
  Board tmp(board);
  move_t best_move = 0;
  alpha_beta(tmp, best_move);
  return best_move;
}
