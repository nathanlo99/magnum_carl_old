
#include "board.h"
#include "piece.h"

#include <string>
#include <sstream>

/*
Halfmove clock: This is the number of halfmoves since the last capture or pawn advance. This is used to determine if a draw can be claimed under the fifty-move rule.
Fullmove number: The number of the full move. It starts at 1, and is incremented after Black's move.

*/
Board::Board(const std::string &fen) {

  // Part 1: The board state
  size_t square_idx = A8; // 91
  const size_t next_space = fen.find(' ');
  for (size_t fen_idx = 0; fen_idx < next_space; ++fen_idx) {
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
      piece_t piece_idx = piece_from_char(chr);
      ASSERT(valid_square(square_idx));
      ASSERT(is_valid_piece(piece_idx));
      m_pieces[square_idx] = piece_idx;
      square_idx++;
      ASSERT(square_idx % 10 == 9 || valid_square(square_idx));
    }
  }
  ASSERT(square_idx == 29);

  // Part 2: Side to move
  const size_t side_idx = next_space + 1;
  ASSERT(fen[side_idx] == 'w' || fen[side_idx] == 'b');
  m_next_move_colour = fen[side_idx] == 'b';

  // Part 3: Castle state
  size_t castle_idx = side_idx + 2;
  m_castle_state = 0;
  if (fen[castle_idx] == '-') {
    ++castle_idx;
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
        ASSERT(!"Invalid character in castling permission specifications");

      ++castle_idx;
    }
  }
  ASSERT(fen[castle_idx] == ' ');

  // Part 4: En passant square
  size_t en_passant_idx = castle_idx + 1;
  if (fen[en_passant_idx] == '-') {
    m_en_passant = INVALID_SQUARE;
    en_passant_idx += 1;
  } else {
    const size_t row = fen[en_passant_idx] - 'a', col = fen[en_passant_idx + 1] - '1';
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
  ASSERT(full_move_idx == fen.size());
}

std::string Board::to_string() const {
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
  result << "+---------------+";
  return result.str();
}

std::ostream& operator<<(std::ostream &os, const Board& board) {
  return os << board.to_string();
}
