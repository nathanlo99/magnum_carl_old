
#include "board.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "square.hpp"

#include <iostream>
#include <set>
#include <string>
#include <vector>

std::vector<move_t> Board::pseudo_moves() const noexcept {
  const int side = m_next_move_colour;

  const auto &it = m_move_cache.find(m_hash);
  if (it != m_move_cache.end())
    return it->second;

  validate_board();

  std::vector<move_t> result;
  if (is_drawn())
    return result;
  result.reserve(START_POSITION_MOVES);

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
  for (unsigned queen_idx = 0; queen_idx < m_num_pieces[queen_piece];
       ++queen_idx) {
    const square_t start = m_positions[queen_piece][queen_idx];
    for (int offset : {-11, -10, -9, -1, 1, 9, 10, 11}) {
      square_t cur_square = start + offset;
      while (valid_square(cur_square) &&
             m_pieces[cur_square] == INVALID_PIECE) {
        INFO("Quiet move from %s to %s\n", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        result.push_back(quiet_move(start, cur_square, queen_piece));
        cur_square += offset;
      }
      if (valid_square(cur_square) &&
          opposite_colours(queen_piece, m_pieces[cur_square]) &&
          !is_king(m_pieces[cur_square])) {
        INFO("Capture move from %s to %s\n", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        result.push_back(
            capture_move(start, cur_square, queen_piece, m_pieces[cur_square]));
      }
    }
  }

  // Rooks
  for (unsigned rook_idx = 0; rook_idx < m_num_pieces[rook_piece]; ++rook_idx) {
    const square_t start = m_positions[rook_piece][rook_idx];
    for (int offset : {-10, -1, 1, 10}) {
      square_t cur_square = start + offset;
      while (valid_square(cur_square) &&
             m_pieces[cur_square] == INVALID_PIECE) {
        INFO("Quiet move from %s to %s\n", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        result.push_back(quiet_move(start, cur_square, rook_piece));
        cur_square += offset;
      }
      if (valid_square(cur_square) &&
          opposite_colours(rook_piece, m_pieces[cur_square]) &&
          !is_king(m_pieces[cur_square])) {
        INFO("Capture move from %s to %s\n", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        result.push_back(
            capture_move(start, cur_square, rook_piece, m_pieces[cur_square]));
      }
    }
  }

  // Bishops
  for (unsigned bishop_idx = 0; bishop_idx < m_num_pieces[bishop_piece];
       ++bishop_idx) {
    const square_t start = m_positions[bishop_piece][bishop_idx];
    for (const int offset : {-11, -9, 9, 11}) {
      square_t cur_square = start + offset;
      while (valid_square(cur_square) &&
             m_pieces[cur_square] == INVALID_PIECE) {
        INFO("Quiet move from %s to %s\n", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        result.push_back(quiet_move(start, cur_square, bishop_piece));
        cur_square += offset;
      }
      if (valid_square(cur_square) &&
          opposite_colours(bishop_piece, m_pieces[cur_square]) &&
          !is_king(m_pieces[cur_square])) {
        INFO("Capture move from %s to %s\n", string_from_square(start).c_str(),
             string_from_square(cur_square).c_str());
        result.push_back(capture_move(start, cur_square, bishop_piece,
                                      m_pieces[cur_square]));
      }
    }
  }

  // Knights
  for (unsigned knight_idx = 0; knight_idx < m_num_pieces[knight_piece];
       ++knight_idx) {
    const square_t start = m_positions[knight_piece][knight_idx];
    for (const int offset : {-21, -19, -12, -8, 8, 12, 19, 21}) {
      const square_t cur_square = start + offset;
      if (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE)
        result.push_back(quiet_move(start, cur_square, knight_piece));
      else if (valid_square(cur_square) &&
               opposite_colours(knight_piece, m_pieces[cur_square]) &&
               !is_king(m_pieces[cur_square]))
        result.push_back(capture_move(start, cur_square, knight_piece,
                                      m_pieces[cur_square]));
    }
  }

  // Pawns
  for (unsigned pawn_idx = 0; pawn_idx < m_num_pieces[pawn_piece]; ++pawn_idx) {
    const square_t start = m_positions[pawn_piece][pawn_idx];
    // Double pawn moves
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

    // Single pawn moves
    const int offset = (side == WHITE) ? 10 : -10;
    const square_t cur_square = start + offset;
    if (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE) {
      if (get_square_row(cur_square) == RANK_1 ||
          get_square_row(cur_square) == RANK_8) {
        for (const piece_t promote_piece : promote_pieces) {
          result.push_back(
              promote_move(start, cur_square, pawn_piece, promote_piece));
        }
      } else {
        result.push_back(quiet_move(start, start + offset, pawn_piece));
      }
    }

    // Normal capture moves
    const square_t capture1 = cur_square - 1, capture2 = cur_square + 1;
    if (valid_square(capture1) && m_pieces[capture1] != INVALID_PIECE &&
        opposite_colours(pawn_piece, m_pieces[capture1]) &&
        !is_king(m_pieces[capture1])) {
      if (get_square_row(capture1) == RANK_1 ||
          get_square_row(capture1) == RANK_8) {
        for (const piece_t promote_piece : promote_pieces) {
          result.push_back(promote_capture_move(
              start, capture1, pawn_piece, promote_piece, m_pieces[capture1]));
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
          result.push_back(promote_capture_move(
              start, capture2, pawn_piece, promote_piece, m_pieces[capture2]));
        }
      } else {
        result.push_back(
            capture_move(start, capture2, pawn_piece, m_pieces[capture2]));
      }
    }

    // En-pass capture
    if (m_en_passant != INVALID_SQUARE) {
      if (capture1 == m_en_passant && m_pieces[capture1] == INVALID_PIECE) {
        result.push_back(en_passant_move(start, m_en_passant, pawn_piece));
      }
      if (capture2 == m_en_passant && m_pieces[capture2] == INVALID_PIECE) {
        result.push_back(en_passant_move(start, m_en_passant, pawn_piece));
      }
    }
  }

  // King
  const square_t start = m_positions[king_piece][0];
  for (const int offset : {-11, -10, -9, -1, 1, 9, 10, 11}) {
    const square_t cur_square = start + offset;
    const piece_t piece = m_pieces[cur_square];
    if (valid_square(cur_square)) {
      if (piece == INVALID_PIECE) {
        result.push_back(quiet_move(start, cur_square, king_piece));
      } else if (opposite_colours(king_piece, piece) && !is_king(piece)) {
        result.push_back(capture_move(start, cur_square, king_piece, piece));
      }
    }
  }

  // Castling
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

  ASSERT(result.size() <= MAX_POSITION_MOVES);
  return m_move_cache[m_hash] = result;
}

std::vector<move_t> Board::legal_moves() const noexcept {
  std::vector<move_t> result;
  Board tmp = *this;
  for (const move_t move : tmp.pseudo_moves()) {
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
