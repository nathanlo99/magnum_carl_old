
#include "evaluate.hpp"
#include "board.hpp"
#include "move.hpp"
#include "square.hpp"
#include "transposition_table.hpp"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>

// Source: https://www.chessprogramming.org/Simplified_Evaluation_Function
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

static const int pawn_square_table[120] = {
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 800, 800, 800, 800, 800, 800, 800, 800, 0, // Encourage promotion
    0, 50,  50,  50,  50,  50,  50,  50,  50,  0, // 7
    0, 10,  10,  20,  30,  30,  20,  10,  10,  0, // 6
    0, 5,   5,   10,  25,  25,  10,  5,   5,   0, // 5
    0, 0,   0,   0,   20,  20,  0,   0,   0,   0, // 4
    0, 5,   -5,  -10, 0,   0,   -10, -5,  5,   0, // 3
    0, 5,   10,  10,  -20, -20, 10,  10,  5,   0, // 2
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // 1
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
};

static const int knight_square_table[120] = {
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, -50, -40, -30, -30, -30, -30, -40, -50, 0, // 8
    0, -40, -20, 0,   0,   0,   0,   -20, -40, 0, // 7
    0, -30, 0,   10,  15,  15,  10,  0,   -30, 0, // 6
    0, -30, 5,   15,  20,  20,  15,  5,   -30, 0, // 5
    0, -30, 0,   15,  20,  20,  15,  0,   -30, 0, // 4
    0, -30, 5,   10,  15,  15,  10,  5,   -30, 0, // 3
    0, -40, -20, 0,   5,   5,   0,   -20, -40, 0, // 2
    0, -50, -40, -30, -30, -30, -30, -40, -50, 0, // 1
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
};

static const int bishop_square_table[120] = {
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, -20, -10, -10, -10, -10, -10, -10, -20, 0, // 8
    0, -10, 0,   0,   0,   0,   0,   0,   -10, 0, // 7
    0, -10, 0,   5,   10,  10,  5,   0,   -10, 0, // 6
    0, -10, 5,   5,   10,  10,  5,   5,   -10, 0, // 5
    0, -10, 0,   10,  10,  10,  10,  0,   -10, 0, // 4
    0, -10, 10,  10,  10,  10,  10,  10,  -10, 0, // 3
    0, -10, 5,   0,   0,   0,   0,   5,   -10, 0, // 2
    0, -20, -10, -10, -10, -10, -10, -10, -20, 0, // 1
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
};

static const int rook_square_table[120] = {
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, // Invalid row
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, // Invalid row
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, // 8
    0, 5,  10, 10, 10, 10, 10, 10, 5,  0, // 7
    0, -5, 0,  0,  0,  0,  0,  0,  -5, 0, // 6
    0, -5, 0,  0,  0,  0,  0,  0,  -5, 0, // 5
    0, -5, 0,  0,  0,  0,  0,  0,  -5, 0, // 4
    0, -5, 0,  0,  0,  0,  0,  0,  -5, 0, // 3
    0, -5, 0,  0,  0,  0,  0,  0,  -5, 0, // 2
    0, 0,  0,  0,  5,  5,  0,  0,  0,  0, // 1
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, // Invalid row
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, // Invalid row
};

static const int queen_square_table[120] = {
    0, 0,   0,   0,   0,  0,  0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,  0,  0,   0,   0,   0, // Invalid row
    0, -20, -10, -10, -5, -5, -10, -10, -20, 0, // 8
    0, -10, 0,   0,   0,  0,  0,   0,   -10, 0, // 7
    0, -10, 0,   5,   5,  5,  5,   0,   -10, 0, // 6
    0, -5,  0,   5,   5,  5,  5,   0,   -5,  0, // 5
    0, 0,   0,   5,   5,  5,  5,   0,   -5,  0, // 4
    0, -10, 5,   5,   5,  5,  5,   0,   -10, 0, // 3
    0, -10, 0,   5,   0,  0,  0,   0,   -10, 0, // 2
    0, -20, -10, -10, -5, -5, -10, -10, -20, 0, // 1
    0, 0,   0,   0,   0,  0,  0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,  0,  0,   0,   0,   0, // Invalid row
};

static const int king_middlegame_square_table[120] = {
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, -30, -40, -40, -50, -50, -40, -40, -30, 0, // 8
    0, -30, -40, -40, -50, -50, -40, -40, -30, 0, // 7
    0, -30, -40, -40, -50, -50, -40, -40, -30, 0, // 6
    0, -30, -40, -40, -50, -50, -40, -40, -30, 0, // 5
    0, -20, -30, -30, -40, -40, -30, -30, -20, 0, // 4
    0, -10, -20, -20, -20, -20, -20, -20, -10, 0, // 3
    0, 20,  20,  0,   0,   0,   0,   20,  20,  0, // 2
    0, 20,  30,  10,  0,   0,   10,  30,  20,  0, // 1
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
};

static const int king_endgame_square_table[120] = {
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, -50, -40, -30, -20, -20, -30, -40, -50, 0, // 8
    0, -30, -20, -10, 0,   0,   -10, -20, -30, 0, // 7
    0, -30, -10, 20,  30,  30,  20,  -10, -30, 0, // 6
    0, -30, -10, 30,  40,  40,  30,  -10, -30, 0, // 5
    0, -30, -10, 30,  40,  40,  30,  -10, -30, 0, // 4
    0, -30, -10, 20,  30,  30,  20,  -10, -30, 0, // 3
    0, -30, -30, 0,   0,   0,   0,   -30, -30, 0, // 2
    0, -50, -30, -30, -30, -30, -30, -30, -50, 0, // 1
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
    0, 0,   0,   0,   0,   0,   0,   0,   0,   0, // Invalid row
};

constexpr inline square_t flip_square(const square_t sq) {
  return sq + 110 - 20 * (sq / 10);
}

// Returns the evaluation from the perspective of white. Thus white pieces
// will generally have positive evaluations and black pieces will generally have
// negative evaluations
int evaluate_piece(const Board &board, piece_t piece, square_t square) {
  if (!valid_piece(piece))
    return 0;

  int result = 0, side_multiplier = 1;
  if (get_side(piece) == WHITE) {
    square = flip_square(square);
  } else {
    piece ^= 8;
    side_multiplier = -1;
  }
  switch (piece) {
  case WHITE_PAWN: {
    result = PAWN_VALUE + pawn_square_table[square];
  } break;
  case WHITE_KNIGHT: {
    result = KNIGHT_VALUE + knight_square_table[square];
  } break;
  case WHITE_BISHOP: {
    result = BISHOP_VALUE + bishop_square_table[square];
  } break;
  case WHITE_ROOK: {
    result = ROOK_VALUE + rook_square_table[square];
  } break;
  case WHITE_QUEEN: {
    result = QUEEN_VALUE + queen_square_table[square];
  } break;
  case WHITE_KING: {
    if (board.is_endgame()) {
      result = king_endgame_square_table[square];
    } else {
      result = king_middlegame_square_table[square];
    }
  } break;
  default:
    return 0;
  }
  return result * side_multiplier;
}

// Given a board and a (not necessarily legal) move, returns a heuristic
// evaluation of the move from the perspective of the side to move.
//
// Captured and promoted pieces significantly boost this score.
int evaluate_move(const Board &board, const move_t move) {
  int value = 0;
  const piece_t moved_piece = ::moved_piece(move);
  if (move_captured(move)) {
    value += 2 * piece_values[to_white(captured_piece(move))] -
             piece_values[to_white(moved_piece)];
  }
  if (move_promoted(move)) {
    value += 3 * piece_values[to_white(promoted_piece(move))];
  }
  const square_t from_square = move_from(move);
  const square_t to_square = move_to(move);
  const int side_multiplier = board.m_side_to_move == WHITE ? 1 : -1;
  const int from_eval =
      side_multiplier * evaluate_piece(board, moved_piece, from_square);
  const int to_eval =
      side_multiplier * evaluate_piece(board, moved_piece, to_square);
  value += to_eval - from_eval;
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

void print_piece_evaluations(const Board &board) {
  std::cout << "=============================================\n";
  std::cout << board << std::endl;
  std::stringstream result;
  result << "\n";
  result << "   +-----+-----+-----+-----+-----+-----+-----+-----+\n";
  for (int row = 7; row >= 0; --row) {
    const char row_name = '1' + row;
    result << " " << row_name << " |";
    for (int col = 0; col < 8; ++col) {
      const square_t square = get_square_120_rc(row, col);
      const piece_t piece_idx = board.m_pieces[square];
      result << std::setw(5) << evaluate_piece(board, piece_idx, square) << "|";
    }
    result << "\n";
    result << "   +-----+-----+-----+-----+-----+-----+-----+-----+\n";
  }
  result << "     a     b     c     d     e     f     g     h\n\n";
  std::cout << result.str() << std::endl;
}

int static_evaluate_board(const Board &board, const int side) {
  // Evaluate everything from white's perspective and take into account side
  // at the end

  // print_piece_evaluations(board);
  int white_eval = 0;
  if (board.is_drawn()) {
    white_eval = 0;
  } else if (board.legal_moves().empty()) {
    white_eval = board.king_in_check() ? -MATE : 0;
  } else {
    // Loop straight through the invalid pieces to avoid branching: there
    // should be 0 of them and their piece value is 0.
    for (piece_t piece = 0; piece < 16; ++piece) {
      const size_t num_pieces = board.m_num_pieces[piece];
      for (size_t piece_idx = 0; piece_idx < num_pieces; ++piece_idx) {
        const square_t piece_square = board.m_positions[piece][piece_idx];
        white_eval += evaluate_piece(board, piece, piece_square);
      }
    }
  }

  // std::cout << "White result was: " << white_eval << std::endl;
  return (side == WHITE) ? white_eval : -white_eval;
}

int quiescence_search(Board &board, int depth = 0, int alpha = -SCORE_INFINITY,
                      const int beta = SCORE_INFINITY) {
  /*
  std::cout << "QS " << std::setw(16) << std::setfill('0') << std::hex
            << board.hash() << std::dec << "\t\t";
  std::cout << std::setw(3) << std::setfill(' ') << depth << " | ";
  std::cout << std::setw(10) << std::setfill(' ') << alpha << ", "
            << std::setw(10) << std::setfill(' ') << beta << std::endl;
  */
  const int stand_pat_eval = static_evaluate_board(board, board.m_side_to_move);
  const auto legal_captures =
      get_sorted_legal_moves(board, MOVEGEN_CAPTURES | MOVEGEN_PROMOTIONS);
  if (board.legal_moves().empty()) {
    return board.king_in_check() ? -MATE : 0;
  } else if (legal_captures.empty()) {
    return stand_pat_eval;
  }

  if (stand_pat_eval >= beta)
    return beta;
  alpha = std::max(alpha, stand_pat_eval);

  for (const move_t next_move : legal_captures) {
    board.make_move(next_move);
    const int value = -quiescence_search(board, depth - 1, -beta, -alpha);
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
    return quiescence_search(board, 0);
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
  ASSERT_MSG(alpha <= beta, "alpha_beta range (%d - %d) is empty", alpha, beta);
  /*
  if (depth > 1) {
    std::cout << "AB " << std::setw(16) << std::setfill('0') << std::hex
              << board.hash() << std::dec << "\t\t";
    std::cout << std::setw(2) << std::setfill(' ') << depth << " | ";
    std::cout << std::setw(10) << std::setfill(' ') << alpha << ", "
              << std::setw(10) << std::setfill(' ') << beta << std::endl;
  }
  */

  const auto legal_moves = get_sorted_legal_moves(board);
  if (legal_moves.empty()) {
    return board.king_in_check() ? -MATE : 0;
  } else if (board.is_drawn()) {
    return 0;
  } else if (depth <= 0) {
    return quiescence_search(board, 0, alpha, beta);
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
    const int this_value = -alpha_beta(tmp, depth - 1, -SCORE_INFINITY, -value);
    tmp.unmake_move();

    printf(
        "Evaluated the move %s (%s) with a searched score of %d and a static "
        "score of %d\n",
        string_from_move(move).c_str(),
        algebraic_notation(legal_moves, move).c_str(), this_value,
        evaluate_move(tmp, move));
    std::cout << std::flush;

    if (this_value > value) {
      std::cout << "New best move!" << std::endl;
      best_move = move;
      value = this_value;
    }
  }
  std::cout << "The overall best move was: "
            << algebraic_notation(legal_moves, best_move) << " with score "
            << value << std::endl;
  return best_move;
}
