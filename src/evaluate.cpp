
#include "evaluate.hpp"
#include "board.hpp"
#include "move.hpp"
#include "square.hpp"
#include "transposition_table.hpp"

#include <algorithm>
#include <functional>
#include <iostream>

enum {
  PAWN_VALUE = 100,
  KNIGHT_VALUE = 320,
  BISHOP_VALUE = 330,
  ROOK_VALUE = 500,
  QUEEN_VALUE = 900,
  KING_VALUE = 20000,
};

static const int piece_values[16] = {
    QUEEN_VALUE,   ROOK_VALUE,    PAWN_VALUE,  0,
    BISHOP_VALUE,  KNIGHT_VALUE,  KING_VALUE,  0, // White pieces
    -QUEEN_VALUE,  -ROOK_VALUE,   -PAWN_VALUE, 0,
    -BISHOP_VALUE, -KNIGHT_VALUE, -KING_VALUE, 0, // Black pieces
};

// Given a board and a (not necessarily legal) move, returns a heuristic
// evaluation of the move from the perspective of the side to move.
//
// Captured and promoted pieces significantly boost this score.
int evaluate_move(const Board &board, const move_t move) {
  const piece_t captured_piece =
      move_captured(move) ? ::captured_piece(move) : INVALID_PIECE;
  const piece_t promoted_piece =
      move_promoted(move) ? ::promoted_piece(move) : INVALID_PIECE;
  const int value =
      -2 * piece_values[captured_piece] + 3 * piece_values[promoted_piece];
  return value;
}

std::vector<move_t> get_sorted_legal_moves(const Board &board,
                                           const int spec = MOVEGEN_ALL) {
  std::vector<move_t> legal_moves = board.legal_moves(spec);
  std::vector<std::pair<int, move_t>> eval_legal_moves;
  std::vector<move_t> result;

  eval_legal_moves.reserve(legal_moves.size());
  result.reserve(legal_moves.size());

  for (const move_t move : legal_moves) {
    const int value = evaluate_move(board, move);
    eval_legal_moves.emplace_back(value, move);
  }

  std::sort(eval_legal_moves.begin(), eval_legal_moves.end(),
            std::greater<std::pair<int, move_t>>());

  for (const auto &[eval, move] : eval_legal_moves) {
    result.push_back(move);
  }
  return result;
}

int static_evaluate_board(const Board &board, const int side) {
  // Evaluate everything from white's perspective and take into account side at
  // the end
  int white_result = 0;
  if (board.is_drawn()) {
    white_result = 0;
  } else if (board.legal_moves().empty()) {
    white_result = board.king_in_check() ? -MATE : 0;
  } else {
    // Loop straight through the invalid pieces to avoid branching: there should
    // be 0 of them and their piece value is 0.
    for (piece_t piece = 0; piece < 16; ++piece) {
      const size_t num_pieces = board.m_num_pieces[piece];
      const int total_score = num_pieces * piece_values[piece];
      white_result += total_score;
    }
  }
  return (side == WHITE) ? white_result : -white_result;
}

int quiescence_search(Board &board, int alpha = -SCORE_INFINITY,
                      const int beta = SCORE_INFINITY) {
  const auto legal_captures =
      get_sorted_legal_moves(board, MOVEGEN_CAPTURES | MOVEGEN_PROMOTIONS);
  if (board.legal_moves().empty()) {
    return board.king_in_check() ? -MATE : 0;
  } else if (legal_captures.empty()) {
    return static_evaluate_board(board, board.m_side_to_move);
  }

  for (const move_t next_move : legal_captures) {
    board.make_move(next_move);
    const int value = -quiescence_search(board, -beta, -alpha);
    board.unmake_move();
    if (value >= beta)
      return value;
    alpha = std::max(value, alpha);
  }
  return alpha;
}

int negamax(Board &board, const int depth) {
  const auto legal_moves = get_sorted_legal_moves(board);
  if (legal_moves.empty()) {
    return board.king_in_check() ? -MATE : 0;
  } else if (board.is_drawn()) {
    return 0;
  } else if (depth <= 0) {
    return quiescence_search(board);
  }

  int best_score = -SCORE_INFINITY;
  for (const move_t move : legal_moves) {
    board.make_move(move);
    const int this_score = -negamax(board, depth - 1);
    board.unmake_move();
    best_score = std::max(best_score, this_score);
  }
  return best_score;
}

int alpha_beta(Board &board, const int depth, int alpha, const int beta) {
  const auto legal_moves = get_sorted_legal_moves(board);
  if (legal_moves.empty()) {
    return board.king_in_check() ? -MATE : 0;
  } else if (board.is_drawn()) {
    return 0;
  } else if (depth <= 0) {
    return quiescence_search(board, alpha, beta);
  }

  for (const move_t next_move : legal_moves) {
    board.make_move(next_move);
    const int value = -alpha_beta(board, depth - 1, -beta, -alpha);
    board.unmake_move();

    if (value >= beta)
      return value;
    alpha = std::max(alpha, value);
  }

  return alpha;
}

int evaluate_board(const Board &board, const int depth) {
  Board tmp = board;
  return alpha_beta(tmp, depth);
}

move_t get_best_move(const Board &board, const int depth) {
  Board tmp = board;
  move_t best_move = 0;
  int value = -SCORE_INFINITY;

  std::cout << "------------------------------------------------" << std::endl;
  std::cout << board << std::endl;

  const auto legal_moves = get_sorted_legal_moves(board);
  for (const move_t move : legal_moves) {
    tmp.make_move(move);
    const int this_value = -alpha_beta(tmp, depth - 1, value, SCORE_INFINITY);
    tmp.unmake_move();
    if (this_value > value) {
      INFO("Found a new best move: %s with a searched score of %d and a static "
           "score of %d",
           string_from_move(move).c_str(), this_value,
           evaluate_move(tmp, move));
      best_move = move;
      value = this_value;
    }
  }
  std::cout << "The overall best move was: " << string_from_move(best_move)
            << " with score " << value << std::endl;
  return best_move;
}
