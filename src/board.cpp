
#include "board.h"
#include "piece.h"
#include "hash.h"

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
  const char * next_chr = fen.data();
  const char * const end_ptr = fen.data() + fen.size();
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
    m_en_passant = get_square_120_rc(row, col);
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
      "Invalid piece (%u) had non-zero hash (%lu)",
        piece, piece_hash[sq][piece]);
    res ^= piece_hash[sq][piece];
  }
  res ^= castle_hash[m_castle_state];
  ASSERT_MSG(enpas_hash[INVALID_SQUARE] == 0,
    "Invalid square had non-zero hash (%lu)", enpas_hash[INVALID_SQUARE]);
  res ^= enpas_hash[m_en_passant];
  res ^= (m_next_move_colour * side_hash);
  return res;
}

std::string Board::to_string() const noexcept {
  validate_board();
  std::stringstream result;
  result << "+---- BOARD ----+" << '\n';
  // TODO: Optimize this loop by refactoring out the square index instead of
  //   calling get_square_120_rc every iteration (can we trust the compiler?)
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
  return result.str();
}

std::ostream& operator<<(std::ostream &os, const Board& board) noexcept {
  return os << board.to_string();
}
