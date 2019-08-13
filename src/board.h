
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <string>
#include <array>
#include <vector>
#include <string_view>
#include <ostream>

#include "piece.h"
#include "square.h"
#include "castle_state.h"

class Board {
  std::array<piece_t, 120> m_pieces;
  std::array<std::vector<square_t>, 16> m_positions;
  bool m_next_move_colour;
  castle_t m_castle_state;
  square_t m_en_passant;
  unsigned int m_fifty_move;
  unsigned int m_full_move;
  // std::vector<Move> m_history;

  constexpr static const char* startFEN =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

public:
  Board(const std::string &fen = Board::startFEN);

  std::string to_string() const;
};

std::ostream& operator<<(std::ostream &os, const Board& board);

#endif /* end of include guard: BOARD_H */
