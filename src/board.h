
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <string>

#include "square.h"
#include "piece.h"

class Board {
  std::array<Piece, 120>                  m_pieces;
  std::array<std::vector<Square>, 16>     m_positions;
  bool                                    m_next_move_colour;
  char                                    m_castle_state;
  Square                                  m_en_passant;
  unsigned int                            m_fifty_move;
  unsigned int                            m_full_move;
  // std::vector<Move>                       m_history;

  constexpr std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
public:
  Board(const std::string& fen = Board::startFEN);




};


#endif /* end of include guard: BOARD_H */
