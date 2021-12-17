
#include "board.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "square.hpp"

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

std::vector<move_t> Board::pseudo_moves(const int spec) const noexcept {

  // In debug mode, check that invariants are maintained
  validate_board();

  std::vector<move_t> result;

  // Reserve some starting amount of space for moves, this is set to 64, roughly
  // twice the average branching factor
  result.reserve(START_POSITION_MOVES);

  const int side = m_side_to_move;
  const piece_t king_piece = (side == WHITE) ? WHITE_KING : BLACK_KING,
                queen_piece = (side == WHITE) ? WHITE_QUEEN : BLACK_QUEEN,
                rook_piece = (side == WHITE) ? WHITE_ROOK : BLACK_ROOK,
                bishop_piece = (side == WHITE) ? WHITE_BISHOP : BLACK_BISHOP,
                knight_piece = (side == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT,
                pawn_piece = (side == WHITE) ? WHITE_PAWN : BLACK_PAWN;

  const piece_t promote_pieces[4] = {
      static_cast<piece_t>(queen_piece ^ 8u),
      static_cast<piece_t>(rook_piece ^ 8u),
      static_cast<piece_t>(bishop_piece ^ 8u),
      static_cast<piece_t>(knight_piece ^ 8u),
  };

  // Queens
  INFO("Generating queen moves");
  const size_t num_queens = m_num_pieces[queen_piece];
  for (unsigned queen_idx = 0; queen_idx < num_queens; ++queen_idx) {
    const square_t start = m_positions[queen_piece][queen_idx];
    for (const int offset : {-11, -10, -9, -1, 1, 9, 10, 11}) {
      square_t cur_square = start + offset;
      while (valid_square(cur_square) &&
             m_pieces[cur_square] == INVALID_PIECE) {
        INFO("Quiet move from %s to %s", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        if (spec & MOVEGEN_NORMAL)
          result.push_back(quiet_move(start, cur_square, queen_piece));
        cur_square += offset;
      }
      if (valid_square(cur_square) &&
          opposite_colours(queen_piece, m_pieces[cur_square]) &&
          !is_king(m_pieces[cur_square])) {
        INFO("Capture move from %s to %s", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        if (spec & MOVEGEN_CAPTURES)
          result.push_back(capture_move(start, cur_square, queen_piece,
                                        m_pieces[cur_square]));
      }
    }
  }

  // Rooks
  INFO("Generating rook moves");
  const size_t num_rooks = m_num_pieces[rook_piece];
  for (unsigned rook_idx = 0; rook_idx < num_rooks; ++rook_idx) {
    const square_t start = m_positions[rook_piece][rook_idx];
    for (const int offset : {-10, -1, 1, 10}) {
      square_t cur_square = start + offset;
      while (valid_square(cur_square) &&
             m_pieces[cur_square] == INVALID_PIECE) {
        INFO("Quiet move from %s to %s", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        if (spec & MOVEGEN_NORMAL)
          result.push_back(quiet_move(start, cur_square, rook_piece));
        cur_square += offset;
      }
      if (valid_square(cur_square) &&
          opposite_colours(rook_piece, m_pieces[cur_square]) &&
          !is_king(m_pieces[cur_square])) {
        INFO("Capture move from %s to %s", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        if (spec & MOVEGEN_CAPTURES)
          result.push_back(capture_move(start, cur_square, rook_piece,
                                        m_pieces[cur_square]));
      }
    }
  }

  // Bishops
  INFO("Generating bishop moves");
  const size_t num_bishops = m_num_pieces[bishop_piece];
  for (unsigned bishop_idx = 0; bishop_idx < num_bishops; ++bishop_idx) {
    const square_t start = m_positions[bishop_piece][bishop_idx];
    for (const int offset : {-11, -9, 9, 11}) {
      square_t cur_square = start + offset;
      while (valid_square(cur_square) &&
             m_pieces[cur_square] == INVALID_PIECE) {
        INFO("Quiet move from %s to %s", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        if (spec & MOVEGEN_NORMAL)
          result.push_back(quiet_move(start, cur_square, bishop_piece));
        cur_square += offset;
      }
      if (valid_square(cur_square) &&
          opposite_colours(bishop_piece, m_pieces[cur_square]) &&
          !is_king(m_pieces[cur_square])) {
        INFO("Capture move from %s to %s", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        if (spec & MOVEGEN_CAPTURES)
          result.push_back(capture_move(start, cur_square, bishop_piece,
                                        m_pieces[cur_square]));
      }
    }
  }

  // Knights
  INFO("Generating knight moves");
  const size_t num_knights = m_num_pieces[knight_piece];
  for (unsigned knight_idx = 0; knight_idx < num_knights; ++knight_idx) {
    const square_t start = m_positions[knight_piece][knight_idx];
    for (const int offset : {-21, -19, -12, -8, 8, 12, 19, 21}) {
      const square_t cur_square = start + offset;
      if (!valid_square(cur_square))
        continue;
      if (m_pieces[cur_square] == INVALID_PIECE) {
        if (spec & MOVEGEN_NORMAL)
          result.push_back(quiet_move(start, cur_square, knight_piece));
      } else if (opposite_colours(knight_piece, m_pieces[cur_square]) &&
                 !is_king(m_pieces[cur_square])) {
        if (spec & MOVEGEN_CAPTURES)
          result.push_back(capture_move(start, cur_square, knight_piece,
                                        m_pieces[cur_square]));
      }
    }
  }

  // Pawns
  INFO("Generating pawn moves");
  const size_t num_pawns = m_num_pieces[pawn_piece];
  for (unsigned pawn_idx = 0; pawn_idx < num_pawns; ++pawn_idx) {
    const square_t start = m_positions[pawn_piece][pawn_idx];
    const int offset = (side == WHITE) ? 10 : -10;
    const square_t cur_square = start + offset;

    // Double pawn moves
    if (spec & MOVEGEN_NORMAL) {
      if (side == WHITE && get_square_row(start) == RANK_2 &&
          m_pieces[start + 10] == INVALID_PIECE &&
          m_pieces[start + 20] == INVALID_PIECE) {
        result.push_back(double_move(start, start + 20, pawn_piece));
      }
      if (side == BLACK && get_square_row(start) == RANK_7 &&
          m_pieces[start - 10] == INVALID_PIECE &&
          m_pieces[start - 20] == INVALID_PIECE) {
        result.push_back(double_move(start, start - 20, pawn_piece));
      }
    }

    // Single pawn moves
    if (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE) {
      if (get_square_row(cur_square) == RANK_1 ||
          get_square_row(cur_square) == RANK_8) {
        if (spec & MOVEGEN_CAPTURES) {
          for (const piece_t promote_piece : promote_pieces) {
            result.push_back(
                promote_move(start, cur_square, pawn_piece, promote_piece));
          }
        }
      } else {
        if (spec & MOVEGEN_NORMAL)
          result.push_back(quiet_move(start, start + offset, pawn_piece));
      }
    }

    if (spec & MOVEGEN_CAPTURES) {
      // Normal capture moves
      INFO("Generating pawn capture moves");
      const square_t capture1 = cur_square - 1, capture2 = cur_square + 1;
      if (valid_square(capture1) && m_pieces[capture1] != INVALID_PIECE &&
          opposite_colours(pawn_piece, m_pieces[capture1]) &&
          !is_king(m_pieces[capture1])) {
        if (get_square_row(capture1) == RANK_1 ||
            get_square_row(capture1) == RANK_8) {
          for (const piece_t promote_piece : promote_pieces) {
            result.push_back(promote_capture_move(start, capture1, pawn_piece,
                                                  promote_piece,
                                                  m_pieces[capture1]));
          }
        } else {
          result.push_back(
              capture_move(start, capture1, pawn_piece, m_pieces[capture1]));
        }
      }
      if (valid_square(capture2) && m_pieces[capture2] != INVALID_PIECE &&
          opposite_colours(pawn_piece, m_pieces[capture2]) &&
          !is_king(m_pieces[capture2])) {
        if (get_square_row(capture2) == RANK_1 ||
            get_square_row(capture2) == RANK_8) {
          for (const piece_t promote_piece : promote_pieces) {
            result.push_back(promote_capture_move(start, capture2, pawn_piece,
                                                  promote_piece,
                                                  m_pieces[capture2]));
          }
        } else {
          result.push_back(
              capture_move(start, capture2, pawn_piece, m_pieces[capture2]));
        }
      }
    }
  }

  // En-passant capture
  if (spec & MOVEGEN_CAPTURES && m_en_passant != INVALID_SQUARE) {
    const int offset = (side == WHITE) ? 10 : -10;
    const square_t start1 = m_en_passant + 1 - offset;
    const square_t start2 = m_en_passant - 1 - offset;
    // 'start1' and 'start2' are the two possible starting squares for an
    // en-passant capture: we don't have to check that they are valid since
    // invalid squares will always have invalid pieces
    if (m_pieces[start1] == pawn_piece) {
      result.push_back(en_passant_move(start1, m_en_passant, pawn_piece));
    }
    if (m_pieces[start2] == pawn_piece) {
      result.push_back(en_passant_move(start2, m_en_passant, pawn_piece));
    }
  }

  // King
  INFO("Generating king moves");
  const square_t start = m_positions[king_piece][0];
  for (const int offset : {-11, -10, -9, -1, 1, 9, 10, 11}) {
    const square_t cur_square = start + offset;
    const piece_t piece = m_pieces[cur_square];
    if (!valid_square(cur_square))
      continue;
    if (piece == INVALID_PIECE) {
      if (spec & MOVEGEN_NORMAL)
        result.push_back(quiet_move(start, cur_square, king_piece));
    } else if (opposite_colours(king_piece, piece) && !is_king(piece)) {
      if (spec & MOVEGEN_CAPTURES)
        result.push_back(capture_move(start, cur_square, king_piece, piece));
    }
  }

  // Castling
  INFO("Generating castling moves");
  if (spec & MOVEGEN_NORMAL) {
    if (side == WHITE) {
      if (!square_attacked(E1, BLACK)) {
        if (m_castle_state & WHITE_SHORT && m_pieces[F1] == INVALID_PIECE &&
            m_pieces[G1] == INVALID_PIECE && m_pieces[H1] == WHITE_ROOK &&
            !square_attacked(F1, BLACK)) {
          result.push_back(castle_move(E1, G1, WHITE_KING, SHORT_CASTLE_MOVE));
        }
        if (m_castle_state & WHITE_LONG && m_pieces[D1] == INVALID_PIECE &&
            m_pieces[C1] == INVALID_PIECE && m_pieces[B1] == INVALID_PIECE &&
            m_pieces[A1] == WHITE_ROOK && !square_attacked(D1, BLACK)) {
          result.push_back(castle_move(E1, C1, WHITE_KING, LONG_CASTLE_MOVE));
        }
      }
    } else if (side == BLACK) {
      if (!square_attacked(E8, WHITE)) {
        if (m_castle_state & BLACK_SHORT && m_pieces[F8] == INVALID_PIECE &&
            m_pieces[G8] == INVALID_PIECE && m_pieces[H8] == BLACK_ROOK &&
            !square_attacked(F8, WHITE)) {
          result.push_back(castle_move(E8, G8, BLACK_KING, SHORT_CASTLE_MOVE));
        }
        if (m_castle_state & BLACK_LONG && m_pieces[D8] == INVALID_PIECE &&
            m_pieces[C8] == INVALID_PIECE && m_pieces[B8] == INVALID_PIECE &&
            m_pieces[A8] == BLACK_ROOK && !square_attacked(D8, WHITE)) {
          result.push_back(castle_move(E8, C8, BLACK_KING, LONG_CASTLE_MOVE));
        }
      }
    }
  }

  ASSERT(result.size() <= MAX_POSITION_MOVES);
  return result;
}

std::vector<move_t> Board::legal_moves(const int spec) const noexcept {
  std::vector<move_t> result;
  Board tmp = *this;
  const auto pseudo_moves = this->pseudo_moves(spec);
  for (const move_t move : pseudo_moves) {
    if (tmp.make_move(move))
      result.push_back(move);
    tmp.unmake_move();
  }
  return result;
}

void print_simple_move_list(const std::vector<move_t> &move_list) {
  std::set<std::string> moves;
  for (const move_t move : move_list)
    moves.insert(simple_string_from_move(move));
  for (const std::string &move : moves)
    std::cout << move << ", ";
  std::cout << std::endl;
}

void print_move_list(const std::vector<move_t> &move_list) {
  std::set<std::string> moves;
  for (const move_t move : move_list)
    moves.insert(string_from_move(move));
  for (const std::string &move : moves)
    std::cout << move << ", ";
  std::cout << std::endl;
}
