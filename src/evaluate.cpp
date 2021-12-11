
#include "evaluate.hpp"
#include "board.hpp"
#include "move.hpp"
#include "square.hpp"
#include "transposition_table.hpp"

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

int evaluate_piece(const Board &board, const piece_t piece, const square_t sq) {
  const square_t idx64 = get_square_64(sq);
  const square_t normalized_square =
      get_side(piece) == WHITE ? idx64 : flip_square(idx64);
  const int raw_value = piece_square_tables[piece][normalized_square];
  return get_side(piece) == WHITE ? raw_value : -raw_value;
}

// Evaluating a move

int evaluate_move(const Board &board, const move_t move) {
  const piece_t captured_piece =
      move_captured(move) ? ::captured_piece(move) : INVALID_PIECE;
  const piece_t promoted_piece =
      move_promoted(move) ? ::promoted_piece(move) : INVALID_PIECE;
  const piece_t moved_piece = ::moved_piece(move);
  const square_t from_square = move_from(move);
  const square_t to_square = move_to(move);
  const int start_value = evaluate_piece(board, moved_piece, from_square);
  const int end_value = evaluate_piece(board, moved_piece, to_square);
  const int value = piece_values[captured_piece] +
                    piece_values[promoted_piece] + end_value - start_value;
  return value;
}

std::vector<move_t> get_sorted_legal_moves(const Board &board, const int spec) {
  std::vector<move_t> legal_moves = board.legal_moves(spec);
  std::vector<std::pair<move_t, int>> eval_legal_moves;
  std::vector<move_t> result;

  eval_legal_moves.reserve(legal_moves.size());
  result.reserve(legal_moves.size());

  for (const move_t move : legal_moves) {
    eval_legal_moves.emplace_back(move, evaluate_move(board, move));
  }

  std::sort(eval_legal_moves.begin(), eval_legal_moves.end());

  for (const auto &[move, eval] : eval_legal_moves) {
    result.push_back(move);
  }
  return result;
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
      result += evaluate_piece(board, piece, piece_location);
    }
  }
  return (side == WHITE) ? result : -result;
}

int quiescence_search(Board &board, int alpha = -SCORE_INFINITY,
                      int beta = SCORE_INFINITY, int depth = 0) {
  const auto legal_captures =
      get_sorted_legal_moves(board, MOVEGEN_CAPTURES | MOVEGEN_PROMOTIONS);
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

  const hash_t hash = board.hash();
  const auto it = transposition_table.find(hash);
  move_t killer_move = 0;
  if (it != transposition_table.end()) {
    const TableEntry &cached_entry = it->second;
    if (cached_entry.depth >= depth) {
      best_move = cached_entry.best_move;
      return cached_entry.value;
    }
    killer_move = cached_entry.best_move;
  }

  const auto legal_moves = get_sorted_legal_moves(board, MOVEGEN_ALL);
  if (legal_moves.empty()) {
    return board.king_in_check() ? -90000 : 0;
  } else if (board.is_drawn()) {
    return 0;
  } else if (depth == 0) {
    return quiescence_search(board, alpha, beta);
  }

  best_move = 0;
  move_t tmp_move;

  // Try the killer move first
  if (killer_move != 0) {
    board.make_move(killer_move);
    const int value = -alpha_beta(board, tmp_move, -beta, -alpha, depth - 1);
    if (value >= beta) {
      board.unmake_move();
      best_move = killer_move;
      transposition_table[hash] = TableEntry(best_move, value, depth);
      return value;
    }
    if (value > alpha) {
      alpha = value;
      best_move = killer_move;
    }
    board.unmake_move();
  }

  for (const move_t next_move : legal_moves) {
    if (next_move == killer_move)
      continue;
    board.make_move(next_move);
    const int value = -alpha_beta(board, tmp_move, -beta, -alpha, depth - 1);
    if (value >= beta) {
      best_move = next_move;
      board.unmake_move();
      transposition_table[hash] = TableEntry(best_move, value, depth);
      return value;
    }
    if (value > alpha) {
      alpha = value;
      best_move = next_move;
    }
    board.unmake_move();
  }
  transposition_table[hash] = TableEntry(best_move, alpha, depth);

  // Transposition table debug logging
  static int iteration_count = 0;
  iteration_count++;
  if (iteration_count % 1000 == 0) {
    std::cout << "After " << iteration_count
              << " iterations, the transposition table has size "
              << transposition_table.size() << std::endl;
  }
  return alpha;
}

int iterative_deepening(Board &board, move_t &best_move) {
  move_t tmp_move;
  for (int i = 1; i < SEARCH_DEPTH; ++i) {
    alpha_beta(board, tmp_move, -SCORE_INFINITY, SCORE_INFINITY, i);
  }
  return alpha_beta(board, best_move);
}

int evaluate_board(const Board &board, move_t &best_move) {
  Board tmp(board);
  return iterative_deepening(tmp, best_move);
}

move_t get_best_move(const Board &board) {
  Board tmp(board);
  move_t best_move = 0;
  iterative_deepening(tmp, best_move);
  return best_move;
}
