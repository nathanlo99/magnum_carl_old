
#include "board.hpp"
#include "piece.hpp"
#include "hash.hpp"
#include "move.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

Board::Board(const std::string &fen) noexcept {
  m_pieces.fill(INVALID_PIECE);
  m_num_pieces.fill(0);
  for (unsigned piece = 0; piece < 16; ++piece) {
    m_positions[piece].fill(INVALID_SQUARE);
  }

  // Part 1: The board state
  unsigned square_idx = A8; // 91
  const char* next_chr = fen.data();
  const char* const end_ptr = fen.data() + fen.size();
  while (*next_chr != ' ') {
    const char chr = *next_chr;
    if (chr == '/') {
      // To the next row
      ASSERT(square_idx % 10 == 9);
      square_idx -= 18;
    } else if ('1' <= chr && chr <= '8') {
      // A number indicates that number of empty squares
      const unsigned num_spaces = chr - '0';
      for (unsigned i = 0; i < num_spaces; ++i)
        m_pieces[square_idx + i] = INVALID_PIECE;
      square_idx += num_spaces;
      ASSERT(square_idx % 10 == 9 || valid_square(square_idx));
    } else {
      // Otherwise, it better be a character corresponding to a valid piece
      const piece_t piece_idx = piece_from_char(chr);
      ASSERT(valid_square(square_idx));
      ASSERT(valid_piece(piece_idx));
      m_pieces[square_idx] = piece_idx;
      ASSERT_MSG(m_num_pieces[piece_idx] < MAX_NUM_PIECES,
        "Too many (%u) pieces of type %u", m_num_pieces[piece_idx], piece_idx);
      m_positions[piece_idx][m_num_pieces[piece_idx]] = square_idx;
      m_num_pieces[piece_idx]++;
      square_idx++;
      ASSERT(square_idx % 10 == 9 || valid_square(square_idx));
    }
    next_chr++;
  }
  ASSERT_MSG(square_idx == 29,
    "Unexpected last square (%u) after parsing FEN board", square_idx);

  // Part 2: Side to move
  next_chr++;
  ASSERT_MSG(*next_chr == 'w' || *next_chr == 'b',
    "Invalid FEN side (%c)", *next_chr);
  m_next_move_colour = (*next_chr == 'w') ? WHITE : BLACK;

  // Part 3: Castle state
  next_chr += 2;
  m_castle_state = 0;
  if (*next_chr == '-') {
    next_chr++;
  } else {
    while (*next_chr != ' ') {
      switch (*next_chr) {
        case 'K':
          m_castle_state |= WHITE_SHORT; break;
        case 'Q':
          m_castle_state |= WHITE_LONG; break;
        case 'k':
          m_castle_state |= BLACK_SHORT; break;
        case 'q':
          m_castle_state |= BLACK_LONG; break;
        default:
          ASSERT_MSG(0,
            "Invalid character (%c) in castling permission specifications",
            *next_chr);
      }
      next_chr++;
    }
  }
  ASSERT(*next_chr == ' ');

  // Part 4: En passant square
  // TODO: Check whether an en passant capture is even possible.
  // If not, zero the en passant square as it is irrelevant.
  next_chr++;
  if (*next_chr == '-') {
    m_en_passant = INVALID_SQUARE;
    next_chr += 1;
  } else {
    const unsigned row = *(next_chr + 1) - '1', col = *next_chr - 'a';
    ASSERT_IF(m_next_move_colour == WHITE, row == RANK_6);
    ASSERT_IF(m_next_move_colour == BLACK, row == RANK_3);
    m_en_passant = get_square_120_rc(row, col);
    if (m_pieces[m_en_passant - 1] == INVALID_PIECE
      && m_pieces[m_en_passant + 1] == INVALID_PIECE) {
      WARN("Elided en passant square");
      m_en_passant = INVALID_SQUARE;
    }
    next_chr += 2;
  }

  ASSERT(*next_chr == ' ');

  // Part 5: Half move counter
  m_fifty_move = 0;
  next_chr++;
  while (*next_chr != ' ') {
    ASSERT_MSG('0' <= *next_chr && *next_chr <= '9',
      "Invalid digit (%c) in half move counter", *next_chr);
    m_fifty_move = 10 * m_fifty_move + (*next_chr - '0');
    next_chr++;
  }
  ASSERT(*next_chr == ' ');

  // Part 6: Full move counter
  m_full_move = 0;
  next_chr++;
  while (next_chr != end_ptr) {
    ASSERT_MSG('0' <= *next_chr && *next_chr <= '9',
      "Invalid digit (%c) in full move counter", *next_chr);
    m_full_move = 10 * m_full_move + (*next_chr - '0');
    next_chr++;
  }
  ASSERT_MSG(next_chr == end_ptr, "FEN string too long");

  m_hash = compute_hash();
}

void Board::validate_board() const noexcept {
  static std::array<unsigned, 16> piece_count;
  piece_count.fill(0);
  for (unsigned sq = 0; sq < 120; ++sq) {
    ASSERT_MSG(valid_piece(m_pieces[sq]) || m_pieces[sq] == INVALID_PIECE,
      "Piece %u at %u is neither valid nor INVALID_PIECE", m_pieces[sq], sq);
    piece_count[m_pieces[sq]]++;
  }
  ASSERT_MSG(m_num_pieces[WHITE_KING] == 1,
    "White has too few/many (%u) kings", m_num_pieces[WHITE_KING]);
  ASSERT_MSG(m_num_pieces[BLACK_KING] == 1,
    "Black has too few/many (%u) kings", m_num_pieces[BLACK_KING]);
  for (unsigned piece = 0; piece < 16; ++piece) {
    ASSERT_MSG(valid_piece(piece) || m_num_pieces[piece] == 0,
      "Invalid piece %u has non-zero count %u", piece, m_num_pieces[piece]);
    const unsigned end = m_num_pieces[piece];
    ASSERT_MSG(!valid_piece(piece)
            || m_num_pieces[piece] == piece_count[piece],
      "Too few/many (%u) pieces of type %u, expected %u",
        m_num_pieces[piece], piece, piece_count[piece]);
    ASSERT_MSG(end <= MAX_NUM_PIECES,
      "Too many (%u) pieces of type %u", end, piece);
    for (unsigned num = 0; num < end; ++num) {
      const square_t sq = m_positions[piece][num];
      ASSERT_MSG(m_pieces[sq] == piece,
        "m_positions[%u][%u] inconsistent with m_pieces[%u]", piece, num, sq);
      for (unsigned compare_idx = num + 1; compare_idx < end; ++compare_idx) {
        ASSERT_MSG(m_positions[piece][num] != m_positions[piece][compare_idx],
          "Repeated position of piece %u at indices %u and %u",
            piece, num, compare_idx);
      }
    }
  }
  ASSERT_MSG(0 <= m_castle_state && m_castle_state < 16,
    "Castle state (%u) out of range", m_castle_state);
  ASSERT_MSG(valid_square(m_en_passant) || m_en_passant == INVALID_SQUARE,
    "En passant square (%u) not valid nor INVALID_SQUARE", m_en_passant);
  ASSERT_MSG(m_next_move_colour != BLACK || m_en_passant == INVALID_SQUARE
    || get_square_row(m_en_passant) == RANK_3,
    "En passant square (%s - %u) not on row 3 on black's turn",
      string_from_square(m_en_passant).c_str(), m_en_passant);
  ASSERT_MSG(m_next_move_colour != WHITE || m_en_passant == INVALID_SQUARE
    || get_square_row(m_en_passant) == RANK_6,
    "En passant square (%s - %u) not on row 6 on white's turn",
      string_from_square(m_en_passant).c_str(), m_en_passant);
}

std::string Board::fen() const noexcept {
  std::stringstream result;
  validate_board();

  // Part 1. The board state
  unsigned square_idx = A8;
  unsigned blank_count = 0;
  while (square_idx != 29) {
    if (square_idx % 10 == 9) {
      if (blank_count != 0) {
        ASSERT_MSG(blank_count <= 8,
          "Too many (%u) blank squares in a row", blank_count);
        result << (char)('0' + blank_count);
        blank_count = 0;
      }
      result << '/';
      square_idx -= 18;
    }
    const piece_t piece = m_pieces[square_idx];
    if (piece == INVALID_PIECE) {
      blank_count++;
    } else {
      // Piece will be valid as board was validated
      if (blank_count != 0) {
        result << (char)('0' + blank_count);
        blank_count = 0;
      }
      result << char_from_piece(piece);
    }
    square_idx++;
  }
  if (blank_count != 0) {
    result << (char)('0' + blank_count);
  }
  result << ' ';

  // Part 2: Side to move
  result << (m_next_move_colour == WHITE ? 'w' : 'b') << ' ';

  // Part 3: Castle state
  if (m_castle_state == 0) {
    result << '-';
  } else {
    if (m_castle_state & WHITE_SHORT)
      result << 'K';
    if (m_castle_state & WHITE_LONG)
      result << 'Q';
    if (m_castle_state & BLACK_SHORT)
      result << 'k';
    if (m_castle_state & BLACK_LONG)
      result << 'q';
  }
  result << ' ';

  // Part 4: En passant square
  result << string_from_square(m_en_passant) << ' ';

  // Part 5: Half move counter
  result << m_fifty_move << ' ';

  // Part 6: Full move counter
  result << m_full_move;

  return result.str();
}

hash_t Board::compute_hash() const noexcept {
  validate_board();
  hash_t res = 0;
  for (unsigned sq = 0; sq < 120; ++sq) {
    const piece_t piece = m_pieces[sq];
    ASSERT_MSG(0 <= piece && piece < 16,
      "Out of range piece (%u) in square", piece);
    ASSERT_MSG(valid_piece(piece) || piece_hash[sq][piece] == 0,
      "Invalid piece (%u) had non-zero hash (%llu)",
        piece, piece_hash[sq][piece]);
    res ^= piece_hash[sq][piece];
  }
  res ^= castle_hash[m_castle_state];
  ASSERT_MSG(enpas_hash[INVALID_SQUARE] == 0,
    "Invalid square had non-zero hash (%llu)", enpas_hash[INVALID_SQUARE]);
  res ^= enpas_hash[m_en_passant];
  res ^= (m_next_move_colour * side_hash);
  return res;
}

std::string Board::to_string() const noexcept {
  validate_board();
  std::stringstream result;
  result << "+---- BOARD ----+" << '\n';
  for (int row = 7; row >= 0; --row) {
    result << '|';
    for (int col = 0; col < 8; ++col) {
      const square_t square = get_square_120_rc(row, col);
      const piece_t piece_idx = m_pieces[square];
      result << char_from_piece(piece_idx) << '|';
    }
    result << '\n';
  }
  result << "+---------------+\n";
  result << "TO MOVE: ";
  result << ((m_next_move_colour == WHITE) ? "WHITE" : "BLACK") << '\n';
  result << "EN PASS: " << string_from_square(m_en_passant) << '\n';
  result << "FIFTY  : " << m_fifty_move << '\n';
  result << "MOVE#  : " << m_full_move << '\n';
  result << "HASH   : ";
  result << std::setw(16) << std::setfill('0') << std::hex << hash() << '\n';
  result << "FEN    : " << fen() << '\n';
  return result.str();
}

std::ostream& operator<<(std::ostream &os, const Board& board) noexcept {
  return os << board.to_string();
}

bool Board::square_attacked(const square_t sq, const bool side) const noexcept {
  const piece_t king_piece   = (side == WHITE) ? WHITE_KING   : BLACK_KING,
                knight_piece = (side == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT,
                pawn_piece   = (side == WHITE) ? WHITE_PAWN   : BLACK_PAWN;

  if (valid_piece(m_pieces[sq]) && get_side(m_pieces[sq]) == side)
    return false;

  // Pawns
  const auto &pawn_offsets = {(side == WHITE) ? -9 : 9, (side == WHITE) ? -11 : 11};
  for (const int offset : pawn_offsets)
    if (m_pieces[sq + offset] == pawn_piece)
      return true;

  // King
  const auto &king_offsets = {-11, -10, -9, -1, 1, 9, 10, 11};
  for (const int offset : king_offsets)
    if (m_pieces[sq + offset] == king_piece)
      return true;

  // Knights
  const auto &knight_offsets = {-21, -19, -12, -8, 8, 12, 19, 21};
  for (const int offset : knight_offsets)
    if (m_pieces[sq + offset] == knight_piece)
      return true;

  // Diagonals
  const auto &diagonal_offsets = {-11, -9, 9, 11};
  for (const int offset : diagonal_offsets) {
    square_t cur_square = sq + offset;
    while (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE)
      cur_square += offset;
    if (valid_square(cur_square) && get_side(m_pieces[cur_square]) == side && is_diag(m_pieces[cur_square]))
      return true;
  }

  // Orthogonals
  const auto &orthogonal_offsets = {-10, -1, 1, 10};
  for (const int offset : orthogonal_offsets) {
    square_t cur_square = sq + offset;
    while (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE)
      cur_square += offset;
    if (valid_square(cur_square) && get_side(m_pieces[cur_square]) == side && is_ortho(m_pieces[cur_square]))
      return true;
  }

  return false;
}

std::vector<move_t> Board::legal_moves(const int _side) const noexcept {
  validate_board();
  std::vector<move_t> result;
  result.reserve(MAX_MOVES);

  const int side = (_side != INVALID_SIDE) ? _side : m_next_move_colour;
  ASSERT_MSG(side == WHITE || side == BLACK, "Invalid side (%u)", side);

  const piece_t king_piece   = (side == WHITE) ? WHITE_KING   : BLACK_KING,
                queen_piece  = (side == WHITE) ? WHITE_QUEEN  : BLACK_QUEEN,
                rook_piece   = (side == WHITE) ? WHITE_ROOK   : BLACK_ROOK,
                bishop_piece = (side == WHITE) ? WHITE_BISHOP : BLACK_BISHOP,
                knight_piece = (side == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT,
                pawn_piece   = (side == WHITE) ? WHITE_PAWN   : BLACK_PAWN;

  const piece_t promote_pieces[4] = {
    static_cast<piece_t>(queen_piece ^ 8u),  static_cast<piece_t>(rook_piece ^ 8u),
    static_cast<piece_t>(bishop_piece ^ 8u), static_cast<piece_t>(knight_piece ^ 8u),
  };

  // Queens
  for (unsigned queen_idx = 0; queen_idx < m_num_pieces[queen_piece]; ++queen_idx) {
    const square_t start = m_positions[queen_piece][queen_idx];
    for (int offset : {-11, -10, -9, -1, 1, 9, 10, 11}) {
      square_t cur_square = start + offset;
      while (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE) {
        // printf("Quiet move from %s to %s\n",
        //   string_from_square(start).c_str(), string_from_square(cur_square).c_str());
        result.push_back(quiet_move(start, cur_square, queen_piece));
        cur_square += offset;
      }
      if (valid_square(cur_square) && opposite_colours(queen_piece, m_pieces[cur_square])) {
        // printf("Capture move from %s to %s\n",
        //  string_from_square(start).c_str(), string_from_square(cur_square).c_str());
        result.push_back(capture_move(start, cur_square, queen_piece, m_pieces[cur_square]));
      }
    }
  }

  // Rooks
  for (unsigned rook_idx = 0; rook_idx < m_num_pieces[rook_piece]; ++rook_idx) {
    const square_t start = m_positions[rook_piece][rook_idx];
    for (int offset : {-10, -1, 1, 10}) {
      square_t cur_square = start + offset;
      while (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE) {
        // printf("Quiet move from %s to %s\n",
        //   string_from_square(start).c_str(), string_from_square(cur_square).c_str());
        result.push_back(quiet_move(start, cur_square, rook_piece));
        cur_square += offset;
      }
      if (valid_square(cur_square) && opposite_colours(rook_piece, m_pieces[cur_square])) {
        // printf("Capture move from %s to %s\n",
        //  string_from_square(start).c_str(), string_from_square(cur_square).c_str());
        result.push_back(capture_move(start, cur_square, rook_piece, m_pieces[cur_square]));
      }
    }
  }

  // Bishops
  for (unsigned bishop_idx = 0; bishop_idx < m_num_pieces[bishop_piece]; ++bishop_idx) {
    const square_t start = m_positions[bishop_piece][bishop_idx];
    for (const int offset : {-11, -9, 9, 11}) {
      square_t cur_square = start + offset;
      while (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE) {
        // printf("Quiet move from %s to %s\n",
        //   string_from_square(start).c_str(), string_from_square(cur_square).c_str());
        result.push_back(quiet_move(start, cur_square, bishop_piece));
        cur_square += offset;
      }
      if (valid_square(cur_square) && opposite_colours(bishop_piece, m_pieces[cur_square])) {
        // printf("Capture move from %s to %s\n",
        //  string_from_square(start).c_str(), string_from_square(cur_square).c_str());
        result.push_back(capture_move(start, cur_square, bishop_piece, m_pieces[cur_square]));
      }
    }
  }

  // Knights
  for (unsigned knight_idx = 0; knight_idx < m_num_pieces[knight_piece]; ++knight_idx) {
    const square_t start = m_positions[knight_piece][knight_idx];
    for (const int offset : {-21, -19, -12, -8, 8, 12, 19, 21}) {
      const square_t cur_square = start + offset;
      if (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE)
        result.push_back(quiet_move(start, cur_square, knight_piece));
      else if (valid_square(cur_square) && opposite_colours(knight_piece, m_pieces[cur_square]))
        result.push_back(capture_move(start, cur_square, knight_piece, m_pieces[cur_square]));
    }
  }

  // Pawns
  for (unsigned pawn_idx = 0; pawn_idx < m_num_pieces[pawn_piece]; ++pawn_idx) {
    const square_t start = m_positions[pawn_piece][pawn_idx];
    // Double pawn moves
    if (side == WHITE && get_square_row(start) == RANK_2
      && m_pieces[start + 10] == INVALID_PIECE && m_pieces[start + 20] == INVALID_PIECE) {
      result.push_back(double_move(start, start + 20, pawn_piece));
    }
    if (side == BLACK && get_square_row(start) == RANK_7
      && m_pieces[start - 10] == INVALID_PIECE && m_pieces[start - 20] == INVALID_PIECE) {
      result.push_back(double_move(start, start - 20, pawn_piece));
    }

    // Single pawn moves
    const int offset = (side == WHITE) ? 10 : -10;
    const square_t cur_square = start + offset;
    if (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE) {
      if (get_square_row(cur_square) == RANK_1 || get_square_row(cur_square) == RANK_8) {
        for (const piece_t promote_piece : promote_pieces) {
          result.push_back(promote_move(start, cur_square, pawn_piece, promote_piece));
        }
      } else {
        result.push_back(quiet_move(start, start + offset, pawn_piece));
      }
    }

    // Normal capture moves
    const square_t capture1 = cur_square - 1, capture2 = cur_square + 1;
    if (valid_square(capture1)
      && m_pieces[capture1] != INVALID_PIECE
      && opposite_colours(pawn_piece, m_pieces[capture1])) {
      result.push_back(capture_move(start, capture1, pawn_piece, m_pieces[capture1]));
    }
    if (valid_square(capture2)
      && m_pieces[capture2] != INVALID_PIECE
      && opposite_colours(pawn_piece, m_pieces[capture2])) {
      result.push_back(capture_move(start, capture2, pawn_piece, m_pieces[capture2]));
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
    if (valid_square(cur_square)) {
      if (m_pieces[cur_square] == INVALID_PIECE) {
        result.push_back(quiet_move(start, cur_square, king_piece));
      } else if (opposite_colours(king_piece, m_pieces[cur_square])) {
        result.push_back(capture_move(start, cur_square, king_piece, m_pieces[cur_square]));
      }
    }
  }

  // Castling
  if (side == WHITE) {
    if (m_castle_state & WHITE_SHORT
      && m_pieces[E1] == WHITE_KING && m_pieces[F1] == INVALID_PIECE
      && m_pieces[G1] == INVALID_PIECE && m_pieces[H1] == WHITE_ROOK
      && !square_attacked(E1, BLACK) && !square_attacked(F1, BLACK)) {
      // If G1 is attacked, this move will be rejected after being made
      result.push_back(castle_move(E1, G1, WHITE_KING, SHORT_CASTLE_MOVE));
    }
    if (m_castle_state & WHITE_LONG
      && m_pieces[E1] == WHITE_KING && m_pieces[D1] == INVALID_PIECE
      && m_pieces[C1] == INVALID_PIECE && m_pieces[B1] == INVALID_PIECE
      && m_pieces[A1] == WHITE_ROOK && !square_attacked(E1, BLACK)
      && !square_attacked(D1, BLACK)) {
      // If C1 is attacked, this move will be rejected after being made
      result.push_back(castle_move(E1, C1, WHITE_KING, LONG_CASTLE_MOVE));
    }
  } else if (side == BLACK) {
    if (m_castle_state & BLACK_SHORT
      && m_pieces[E8] == BLACK_KING && m_pieces[F8] == INVALID_PIECE
      && m_pieces[G8] == INVALID_PIECE && m_pieces[H8] == BLACK_ROOK
      && !square_attacked(E8, WHITE) && !square_attacked(F8, WHITE)) {
      result.push_back(castle_move(E8, G8, BLACK_KING, SHORT_CASTLE_MOVE));
    }
    if (m_castle_state & BLACK_LONG
      && m_pieces[E8] == BLACK_KING && m_pieces[D8] == INVALID_PIECE
      && m_pieces[C8] == INVALID_PIECE && m_pieces[B8] == INVALID_PIECE
      && m_pieces[A8] == BLACK_ROOK && !square_attacked(E8, WHITE)
      && !square_attacked(D8, WHITE)) {
      result.push_back(castle_move(E8, C8, BLACK_KING, LONG_CASTLE_MOVE));
    }
  }

  ASSERT(result.size() <= MAX_MOVES);
  return result;
}
