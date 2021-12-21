
#include "evaluate.hpp"
#include "board.hpp"
#include "move.hpp"
#include "perf_counter.hpp"
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
int evaluate_move(Board &board, const move_t move) {
  perf_counter.increment("evaluate_move");
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

  // Checks get +10000
  board.make_move(move);
  const bool is_check = board.king_in_check();
  board.unmake_move();
  if (is_check)
    value += 10000;
  return value;
}

void order_moves(const Board &board, std::vector<move_t> &moves) {
  perf_counter.increment("order_moves");
  std::vector<std::pair<int, move_t>> eval_legal_moves;
  eval_legal_moves.reserve(moves.size());
  Board tmp(board);
  for (const move_t move : moves) {
    const int value = evaluate_move(tmp, move);
    eval_legal_moves.emplace_back(value, move);
  }
  std::sort(eval_legal_moves.begin(), eval_legal_moves.end(),
            std::greater<std::pair<int, move_t>>());
  for (size_t idx = 0; idx < moves.size(); ++idx) {
    moves[idx] = eval_legal_moves[idx].second;
  }
}

std::vector<move_t> get_sorted_legal_moves(const Board &board,
                                           const int spec = MOVEGEN_ALL) {
  perf_counter.increment("get_sorted_legal_moves");
  auto legal_moves = board.legal_moves(spec);
  order_moves(board, legal_moves);
  return legal_moves;
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
  perf_counter.increment("SE");

  // Evaluate everything from white's perspective and take into account side
  // at the end
  int white_eval = 0;
  if (board.is_drawn()) {
    white_eval = 0;
  } else if (!board.has_legal_moves()) {
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
  const int final_result = (side == WHITE) ? white_eval : -white_eval;
  return final_result;
}

int quiescence_search(Board &board, const int ply, int alpha = -SCORE_INFINITY,
                      const int beta = SCORE_INFINITY) {
  perf_counter.increment("QS");

  // std::cout << "QS " << std::setw(16) << std::setfill('0') << std::hex
  //           << board.hash() << std::dec << "\t\t";
  // std::cout << std::setw(3) << std::setfill(' ') << ply << " | ";
  // std::cout << std::setw(10) << std::setfill(' ') << alpha << ", "
  //           << std::setw(10) << std::setfill(' ') << beta << std::endl;

  if (!board.has_legal_moves()) {
    return board.king_in_check() ? -MATE + ply * MATE_OFFSET : 0;
  } else if (board.is_drawn()) {
    return 0;
  }

  const int stand_pat_eval = static_evaluate_board(board, board.m_side_to_move);
  auto legal_captures =
      board.legal_moves(MOVEGEN_CAPTURES | MOVEGEN_PROMOTIONS);
  if (legal_captures.empty()) {
    return stand_pat_eval;
  }

  if (stand_pat_eval >= beta) {
    perf_counter.increment("QS_cut_beta_stand_pat");
    return stand_pat_eval;
  }
  alpha = std::max(alpha, stand_pat_eval);

  perf_counter.increment("QS_");

  const TableEntry entry = transposition_table.query(board.hash());
  const move_t killer_move = entry.best_move;
  if (killer_move != 0 && move_captured(killer_move)) {
    board.make_move(killer_move);
    const int value = -quiescence_search(board, ply + 1, -beta, -alpha);
    board.unmake_move();
    if (value >= beta) {
      perf_counter.increment("QS_cut_beta_killer");
      return value;
    }
    alpha = std::max(value, alpha);
  }

  order_moves(board, legal_captures);
  for (const move_t next_move : legal_captures) {
    board.make_move(next_move);
    const int value = -quiescence_search(board, ply + 1, -beta, -alpha);
    board.unmake_move();
    if (value >= beta) {
      perf_counter.increment("QS_cut_beta_non_killer");
      return value;
    }
    alpha = std::max(value, alpha);
  }
  perf_counter.increment("QS_cut_none");
  return alpha;
}

int negamax(Board &board, const int ply, const int depth) {
  if (!board.has_legal_moves()) {
    return board.king_in_check() ? -mate_in(ply) : 0;
  } else if (board.is_drawn()) {
    return 0;
  } else if (ply >= depth) {
    return quiescence_search(board, ply);
  }

  const auto legal_moves = get_sorted_legal_moves(board);
  int best_score = -SCORE_INFINITY;
  for (const move_t move : legal_moves) {
    board.make_move(move);
    const int value = -negamax(board, ply + 1, depth);
    board.unmake_move();
    best_score = std::max(best_score, value);
  }
  return best_score;
}

inline std::string to_string(const int num, const int num_digits) {
  std::stringstream ss;
  ss << std::setw(num_digits) << std::setfill('0') << num;
  return ss.str();
}

int alpha_beta(Board &board, const int ply, const int max_depth, int alpha,
               const int beta) {
  perf_counter.increment("AB");
  ASSERT_MSG(alpha <= beta, "alpha_beta range (%d - %d) is empty", alpha, beta);

  // if (ply > 1) {
  //   std::cout << "AB " << std::setw(16) << std::setfill('0') << std::hex
  //             << board.hash() << std::dec << "\t\t";
  //   std::cout << std::setw(3) << std::setfill(' ') << ply << " | ";
  //   std::cout << std::setw(10) << std::setfill(' ') << alpha << ", "
  //             << std::setw(10) << std::setfill(' ') << beta << std::endl;
  // }

  // Get the move-ordered legal moves and check for game termination cases
  if (!board.has_legal_moves()) {
    return board.king_in_check() ? -mate_in(ply) : 0;
  } else if (board.is_drawn()) {
    return 0;
  } else if (ply >= max_depth) {
    // If we've reached the search depth, perform quiescence_search instead
    return quiescence_search(board, ply, alpha, beta);
  }

  perf_counter.increment("AB_");

  // Consult the transposition table: grab a cached evaluation and the best move
  const TableEntry entry = transposition_table.query(board.hash());
  ASSERT_MSG(
      entry.type == NodeType::None || entry.hash == board.hash(),
      "Queried table entry's hash (%llu) did not match board hash (%llu)",
      entry.hash, board.hash());
  const move_t killer_move = entry.best_move;
  // Switch on entry.type to get better bounds on alpha and beta
  if (entry.type != None && entry.depth >= max_depth) {
    if (entry.type == NodeType::Exact) {
      perf_counter.increment("AB_lookup_exact");
      return entry.value;
    } else if (entry.type == NodeType::Upper && entry.value <= alpha) {
      perf_counter.increment("AB_lookup_upper");
      return alpha;
    } else if (entry.type == NodeType::Lower && entry.value >= beta) {
      perf_counter.increment("AB_lookup_lower");
      return entry.value;
    }
  }

  const int start_alpha = alpha;
  move_t best_move = 0;

  if (killer_move != 0) {
    board.make_move(killer_move);
    const int value = -alpha_beta(board, ply + 1, max_depth, -beta, -alpha);
    board.unmake_move();

    if (value >= beta) {
      perf_counter.increment("AB_cut_beta_killer");
      transposition_table.insert(board, killer_move, max_depth, value, Lower);
      return value;
    }
    if (value > alpha) {
      alpha = value;
      best_move = killer_move;
    }
  }

  const auto legal_moves = get_sorted_legal_moves(board);
  best_move = best_move ? best_move : legal_moves[0];

  int move_num = 0;
  for (const move_t next_move : legal_moves) {
    board.make_move(next_move);
    const int value = -alpha_beta(board, ply + 1, max_depth, -beta, -alpha);
    board.unmake_move();

    if (value >= beta) {
      // perf_counter.increment("AB_cut_beta_move_" + to_string(move_num, 3));
      perf_counter.increment("AB_cut_beta_non_killer");
      transposition_table.insert(board, next_move, max_depth, value, Lower);
      return value;
    }
    if (value > alpha) {
      alpha = value;
      best_move = next_move;
    }
    move_num++;
  }

  const NodeType type = (alpha == start_alpha) ? Upper : Exact;
  transposition_table.insert(board, best_move, max_depth, alpha, type);

  perf_counter.increment("AB_cut_none");
  return alpha;
}

inline float seconds_since(
    const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
  const auto finish = std::chrono::high_resolution_clock::now();
  const auto ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start)
          .count();
  return ns / 1e9;
}

int iterative_deepening(Board &board, const int max_depth,
                        const float seconds_to_search) {

  const auto start = std::chrono::high_resolution_clock::now();

  int depth = 1;
  while (depth <= max_depth && seconds_since(start) < seconds_to_search / 2.) {
    std::cout << "Searching to depth " << std::setw(2) << depth << "..."
              << std::flush;
    alpha_beta(board, 0, depth);

    const TableEntry entry = transposition_table.query(board.hash());
    const float seconds_elapsed = seconds_since(start);
    std::cout << "  done! (" << std::setw(7) << transposition_table.size()
              << " entries, eval = " << eval_to_string(entry.value)
              << ", PV = " << get_pv_string(board) << ")" << std::endl;
    std::cout << seconds_elapsed << "s elapsed" << std::endl;
    depth++;
  }

  return depth;
}

int evaluate_board(const Board &board, const int depth) {
  Board tmp = board;
  const int iterative_depth = iterative_deepening(tmp, depth, 5);
  return alpha_beta(tmp, 0, iterative_depth);
}

move_t get_best_move(const Board &board, const int depth, const float seconds) {
  perf_counter.clear();
  Board tmp(board);
  iterative_deepening(tmp, depth, seconds);

  const TableEntry entry = transposition_table.query(board.hash());
  // std::cout << "The overall best move was: "
  //           << board.algebraic_notation(entry.best_move) << " with score "
  //           << eval_to_string(entry.value) << std::endl;
  std::cout << "EV: " << eval_to_string(entry.value) << std::endl;
  std::cout << "PV: " << get_pv_string(board) << std::endl;

  perf_counter.dump();
  return entry.best_move;
}
