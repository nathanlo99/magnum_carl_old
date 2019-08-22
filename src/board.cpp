
#include "board.h"
#include "piece.h"
#include "hash.h"

#include <iomanip>
#include <string>
#include <sstream>

Board::Board(const std::string &fen) noexcept {
  // Part 1: The board state
  for (unsigned sq = 0; sq < 120; ++sq) {
    m_pieces[sq] = INVALID_PIECE;
  }
  for (unsigned piece = 0; piece < 16; ++piece) {
    m_num_pieces[piece] = 0;
    for (unsigned num = 0; num < MAX_NUM_PIECES; ++num) {
      m_positions[piece][num] = INVALID_SQUARE;
    }
  }

  size_t square_idx = A8; // 91
  size_t fen_idx = 0;
  while (fen[fen_idx] != ' ') {
    const char chr = fen[fen_idx];
    if (chr == '/') {
      ASSERT(square_idx % 10 == 9);
      square_idx -= 18;
    } else if ('1' <= chr && chr <= '8') {
      const size_t num_spaces = chr - '0';
      for (size_t i = 0; i < num_spaces; ++i)
        m_pieces[square_idx + i] = INVALID_PIECE;
      square_idx += num_spaces;
      ASSERT(square_idx % 10 == 9 || valid_square(square_idx));
    } else {
      const piece_t piece_idx = piece_from_char(chr);
      ASSERT(valid_square(square_idx));
      ASSERT(is_valid_piece(piece_idx));
      m_pieces[square_idx] = piece_idx;
      ASSERT_MSG(m_num_pieces[piece_idx] < MAX_NUM_PIECES,
        "Too many (%u) pieces of type %u", m_num_pieces[piece_idx], piece_idx);
      m_positions[piece_idx][m_num_pieces[piece_idx]] = square_idx;
      m_num_pieces[piece_idx]++;
      square_idx++;
      ASSERT(square_idx % 10 == 9 || valid_square(square_idx));
    }
    fen_idx++;
  }
  ASSERT(square_idx == 29);

  // Part 2: Side to move
  const size_t side_idx = fen_idx + 1;
  ASSERT_MSG(fen[side_idx] == 'w' || fen[side_idx] == 'b',
    "Invalid FEN side (%c)", fen[side_idx]);
  m_next_move_colour = (fen[side_idx] == 'w') ? WHITE : BLACK;

  // Part 3: Castle state
  size_t castle_idx = side_idx + 2;
  m_castle_state = 0;
  if (fen[castle_idx] == '-') {
    castle_idx++;
  } else {
    while (fen[castle_idx] != ' ') {
      if (fen[castle_idx] == 'K')
        m_castle_state |= WHITE_SHORT;
      else if (fen[castle_idx] == 'Q')
        m_castle_state |= WHITE_LONG;
      else if (fen[castle_idx] == 'k')
        m_castle_state |= BLACK_SHORT;
      else if (fen[castle_idx] == 'q')
        m_castle_state |= BLACK_LONG;
      else
        ASSERT_MSG(0,
          "Invalid character in castling permission specifications");

      castle_idx++;
    }
  }
  ASSERT(fen[castle_idx] == ' ');

  // Part 4: En passant square
  size_t en_passant_idx = castle_idx + 1;
  if (fen[en_passant_idx] == '-') {
    m_en_passant = INVALID_SQUARE;
    en_passant_idx += 1;
  } else {
    const size_t row = fen[en_passant_idx    ] - 'a',
                 col = fen[en_passant_idx + 1] - '1';
    m_en_passant = get_square_120_rc(row, col);
    en_passant_idx += 2;
  }
  ASSERT(fen[en_passant_idx] == ' ');

  // Part 5: Half move counter
  m_fifty_move = 0;
  size_t fifty_move_idx = en_passant_idx + 1;
  while (fen[fifty_move_idx] != ' ') {
    m_fifty_move = 10 * m_fifty_move + (fen[fifty_move_idx] - '0');
    fifty_move_idx++;
  }
  ASSERT(fen[fifty_move_idx] == ' ');

  // Part 6: Full move counter
  m_full_move = 0;
  size_t full_move_idx = fifty_move_idx + 1;
  while (fen[full_move_idx] != ' ' && full_move_idx < fen.size()) {
    m_full_move = 10 * m_full_move + (fen[full_move_idx] - '0');
    full_move_idx++;
  }
  ASSERT_MSG(full_move_idx == fen.size(), "FEN string too long");

  m_hash = compute_hash();
}

void Board::validate_board() const noexcept {
  for (unsigned sq = 0; sq < 120; ++sq) {
    ASSERT_MSG(is_valid_piece(m_pieces[sq]) || m_pieces[sq] == INVALID_PIECE,
      "Invalid piece %u at %u", m_pieces[sq], sq);
  }
  for (unsigned piece = 0; piece < 16; ++piece) {
    if (!is_valid_piece(piece)) {
      ASSERT_MSG(m_num_pieces[piece] == 0,
        "Invalid piece %u has non-zero count %u", piece, m_num_pieces[piece]);
    }
    for (unsigned num = 0, end = m_num_pieces[piece]; num < end; ++num) {
      ASSERT_MSG(end <= MAX_NUM_PIECES,
        "Too many (%u) pieces of type %u", end, piece);
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
}

std::string Board::fen() const noexcept {
  std::stringstream result;
  validate_board();

  // Part 1. The board state
  size_t square_idx = A8;
  size_t blank_count = 0;
  while (square_idx != 29) {
    if (square_idx % 10 == 9) {
      if (blank_count != 0) {
        ASSERT_MSG(blank_count <= 8,
          "Too many (%zu) blank squares in a row", blank_count);
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
      if (blank_count != 0) {
        result << (char)('0' + blank_count);
        blank_count = 0;
      }
      result << char_from_piece(piece);
    }
    square_idx++;
  }
  result << ' ';

  // Part 2: Side to move
  result << (m_next_move_colour ? 'b' : 'w') << ' ';

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
    ASSERT_MSG(is_valid_piece(piece) || piece_hash[sq][piece] == 0,
      "Invalid piece (%u) had non-zero hash (%llu)",
        piece, piece_hash[sq][piece]);
    res ^= piece_hash[sq][piece];
  }
  res ^= castle_hash[m_castle_state];
  ASSERT_MSG(enpas_hash[INVALID_SQUARE] == 0,
    "Invalid square had non-zero hash (%llu)", enpas_hash[INVALID_SQUARE]);
  res ^= enpas_hash[m_en_passant];
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
  result << "EN PASS: ";
  result << string_from_square(m_en_passant) << '\n';
  result << "FIFTY  : " << m_fifty_move << '\n';
  result << "MOVE#  : " << m_full_move << '\n';
  ASSERT_MSG(compute_hash() == m_hash, "Hash invariant broken");
  result << "HASH   : ";
  result << std::setw(16) << std::setfill('0') << std::hex << m_hash << '\n';
  return result.str();
}

std::ostream& operator<<(std::ostream &os, const Board& board) noexcept {
  return os << board.to_string();
}
