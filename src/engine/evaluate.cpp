
#include "evaluate.hpp"
#include "board.hpp"
#include "move.hpp"
#include "perf_counter.hpp"
#include "piece_values.hpp"
#include "square.hpp"
#include "transposition_table.hpp"
#include "uci_protocol.hpp"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>

static bool should_stop(const SearchInfo &info) {
  if (info.is_stopped || info.has_quit)
    return true;
  if (info.time_set && seconds_since(info.start_time) > info.seconds_to_search)
    return true;
  return false;
}

// Returns the evaluation from the perspective of white. Thus white pieces
// will generally have positive evaluations and black pieces will generally have
// negative evaluations
int evaluate_piece(const Board &board, piece_t piece, const square_t square) {
  if (board.is_endgame()) {
    if (piece == WHITE_KING) {
      piece = WHITE_ENDGAME_KING;
    } else if (piece == BLACK_KING) {
      piece = BLACK_ENDGAME_KING;
    }
  }
  return piece_values[piece][square];
}

// Given a board and a (not necessarily legal) move, returns a heuristic
// evaluation of the move from the perspective of the side to move.
//
// Captured and promoted pieces significantly boost this score.
int evaluate_move(Board &board, const TableEntry entry, const move_t move) {
  perf_counter.increment("evaluate_move");

  int value = 0;
  const piece_t moved_piece = ::moved_piece(move);
  if (move_captured(move)) {
    value += 10 * base_piece_values[to_white(captured_piece(move))] +
             (1000 - base_piece_values[to_white(moved_piece)]);
  }
  if (move_promoted(move)) {
    value += 3 * base_piece_values[to_white(promoted_piece(move))];
  }

  // Checks get +10000
  board.make_move(move);
  const bool is_check = board.king_in_check();
  board.unmake_move();
  if (is_check)
    value += 10000;

  // PV move gets +20000
  if (move == entry.best_move)
    value += 20000;

  return value;
}

void order_moves(const Board &board, std::vector<move_t> &moves) {
  perf_counter.increment("order_moves");
  std::vector<std::pair<int, move_t>> eval_legal_moves;
  eval_legal_moves.reserve(moves.size());
  const TableEntry entry = transposition_table.query(board.hash());
  Board tmp(board);
  for (const move_t move : moves) {
    const int value = evaluate_move(tmp, entry, move);
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

  // print_piece_evaluations(board);

  // Evaluate everything from white's perspective and take into account side
  // at the end
  int white_eval = 0;
  if (board.is_repeated() || board.is_drawn()) {
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

inline std::string to_string(const int num, const int num_digits) {
  std::stringstream ss;
  ss << std::setw(num_digits) << std::setfill('0') << num;
  return ss.str();
}

int quiescence_search(SearchInfo &info, Board &board, const int ply,
                      int alpha = -SCORE_INFINITY,
                      const int beta = SCORE_INFINITY) {

  // std::cout << "QS " << std::setw(16) << std::setfill('0') << std::hex
  //           << board.hash() << std::dec << "\t\t";
  // std::cout << std::setw(3) << std::setfill(' ') << ply << " | ";
  // std::cout << std::setw(10) << std::setfill(' ') << alpha << ", "
  //           << std::setw(10) << std::setfill(' ') << beta << std::endl;

  if (info.nodes % SearchInfo::refresh_frequency == 0 && should_stop(info)) {
    info.is_stopped = true;
    return 0;
  }

  info.nodes++;

  if (!board.has_legal_moves()) {
    return board.king_in_check() ? -MATE + ply * MATE_OFFSET : 0;
  } else if (board.is_drawn() || board.is_repeated()) {
    return 0;
  }

  const int stand_pat_eval = static_evaluate_board(board, board.m_side_to_move);
  if (stand_pat_eval >= beta)
    return stand_pat_eval;

  auto legal_captures =
      board.legal_moves(MOVEGEN_CAPTURES | MOVEGEN_PROMOTIONS);
  if (legal_captures.empty())
    return stand_pat_eval;

  alpha = std::max(alpha, stand_pat_eval);

  order_moves(board, legal_captures);
  for (const move_t next_move : legal_captures) {
    board.make_move(next_move);
    const int value = -quiescence_search(info, board, ply + 1, -beta, -alpha);
    board.unmake_move();

    if (info.is_stopped || info.has_quit)
      return 0;
    if (value >= beta)
      return value;
    alpha = std::max(alpha, value);
  }
  return alpha;
}

int negamax(SearchInfo &info, Board &board, const int ply, const int depth) {
  if (!board.has_legal_moves()) {
    return board.king_in_check() ? -mate_in(ply) : 0;
  } else if (board.is_drawn() || board.is_repeated()) {
    return 0;
  } else if (depth <= 0) {
    return quiescence_search(info, board, ply);
  }

  const auto legal_moves = get_sorted_legal_moves(board);
  int best_score = -SCORE_INFINITY;
  for (const move_t move : legal_moves) {
    board.make_move(move);
    const int value = -negamax(info, board, ply + 1, depth - 1);
    board.unmake_move();
    best_score = std::max(best_score, value);
  }
  return best_score;
}

int alpha_beta(SearchInfo &info, Board &board, const int ply, const int depth,
               int alpha, int beta) {
  perf_counter.increment("AB");
  ASSERT_MSG(alpha <= beta, "alpha_beta range (%d - %d) is empty", alpha, beta);

  if (info.nodes % SearchInfo::refresh_frequency == 0 && should_stop(info)) {
    info.is_stopped = true;
    return 0;
  }
  info.nodes++;

  // Mate distance pruning
  const int mating_value = mate_in(ply);
  if (mating_value < beta) {
    beta = mating_value;
    if (alpha >= mating_value) {
      perf_counter.increment("AB_mate_pruned");
      return mating_value;
    }
  }

  if (-mating_value > alpha) {
    alpha = -mating_value;
    if (beta <= -mating_value) {
      perf_counter.increment("AB_mate_pruned");
      return -mating_value;
    }
  }

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
  } else if (board.is_drawn() || board.is_repeated()) {
    return 0;
  } else if (depth <= 0) {
    // If we've reached the search depth, perform quiescence_search instead
    return quiescence_search(info, board, ply, alpha, beta);
  }

  // Consult the transposition table: grab a cached evaluation and the best move
  const TableEntry entry = transposition_table.query(board.hash());
  // Switch on entry.type to get better bounds on alpha and beta
  if (entry.type != None && entry.depth >= depth) {
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

  perf_counter.increment("AB_");

  const int start_alpha = alpha;
  move_t best_move = 0;
  const auto legal_moves = get_sorted_legal_moves(board);

  int move_num = 0;
  for (const move_t next_move : legal_moves) {
    board.make_move(next_move);
    const int value =
        -alpha_beta(info, board, ply + 1, depth - 1, -beta, -alpha);
    board.unmake_move();

    if (info.is_stopped || info.has_quit)
      return 0;

    if (value >= beta) {
      perf_counter.increment("AB_cut_beta_move_" + to_string(move_num, 3));
      perf_counter.increment("AB_cut_beta");
      transposition_table.insert(board, next_move, depth, value, Lower);
      return value;
    }
    if (value > alpha) {
      alpha = value;
      best_move = next_move;
    }
    move_num++;
  }

  if (alpha > start_alpha) {
    ASSERT(best_move != 0);
    perf_counter.increment("AB_cut_none_improved");
    transposition_table.insert(board, best_move, depth, alpha, Exact);
  }
  return alpha;
}

void iterative_deepening(SearchInfo &info, Board &board) {
  std::stringstream info_ss;

  info_ss << "Searching to depth " << info.depth << " for "
          << info.seconds_to_search << " seconds";
  UCIProtocol::send_info(info_ss.str());
  info_ss.str(std::string());

  for (int depth = 1; depth <= info.depth; ++depth) {
    info_ss << "Searching to depth " << std::setw(2) << depth << "...";
    UCIProtocol::send_info(info_ss.str());
    info_ss.str(std::string());

    alpha_beta(info, board, 0, depth, -SCORE_INFINITY, SCORE_INFINITY);

    if (info.is_stopped || info.has_quit)
      break;

    const TableEntry entry = transposition_table.query(board.hash());
    std::cout << "info ";
    std::cout << "score " << eval_to_uci_string(entry.value) << " ";
    std::cout << "depth " << entry.depth << " ";
    std::cout << "nodes " << info.nodes << " ";
    std::cout << "nps "
              << static_cast<int>(info.nodes / seconds_since(info.start_time))
              << " ";
    std::cout << "time "
              << static_cast<int>(1000 * seconds_since(info.start_time)) << " ";
    std::cout << "pv ";
    const std::vector<move_t> pv_moves = get_pv(board);
    for (const move_t move : pv_moves) {
      std::cout << simple_string_from_move(move) << " ";
    }
    std::cout << std::endl;

    // std::cout << "Done! (" << std::setw(7) << transposition_table.size()
    //           << " entries, eval = " << eval_to_string(entry.value)
    //           << ", PV = " << get_pv_string(board) << ")" << std::endl;
    // const float seconds_elapsed = seconds_since(info.start_time);
    // std::cout << seconds_elapsed << "s elapsed" << std::endl;
  }
  //
  // std::cout << "Search stopped" << std::endl;
  // const float seconds_elapsed = seconds_since(info.start_time);
  // std::cout << seconds_elapsed << "s elapsed" << std::endl;
}

move_t get_best_move(SearchInfo &info, const Board &board) {
  perf_counter.clear();
  Board tmp(board);
  // std::cout << "Starting search to depth " << info.depth << " with "
  //           << info.seconds_to_search << " seconds..." << std::endl;
  iterative_deepening(info, tmp);

  const TableEntry entry = transposition_table.query(board.hash());
  // std::cout << "The overall best move was: "
  //           << board.algebraic_notation(entry.best_move) << " with score "
  //           << eval_to_string(entry.value) << std::endl;
  // perf_counter.dump();
  // const float fail_high_rate =
  //     static_cast<float>(perf_counter.get_value("AB_cut_beta_move_000")) /
  //     static_cast<float>(perf_counter.get_value("AB_cut_beta"));
  // std::cout << "fail_high_rate: " << fail_high_rate << std::endl;
  // std::cout << "EV: " << eval_to_string(entry.value) << std::endl;
  // std::cout << "PV: " << get_pv_string(board) << std::endl;

  std::cout << "bestmove " << simple_string_from_move(entry.best_move)
            << std::endl;

  return entry.best_move;
}
