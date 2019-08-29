
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
#include "hash.h"

// NOTE: The max number of any type of piece in play. Keep as small as possible.
enum { MAX_NUM_PIECES = 16 };
enum { WHITE = 0, BLACK = 1 };

class Board {
  std::array<piece_t, 120> m_pieces;
  std::array<std::array<square_t, MAX_NUM_PIECES>, 16> m_positions;
  std::array<unsigned, 16> m_num_pieces;
  bool m_next_move_colour;
  castle_t m_castle_state;
  square_t m_en_passant;
  unsigned int m_fifty_move;
  unsigned int m_full_move;
  hash_t m_hash;

  // std::vector<Move> m_history;

  hash_t compute_hash() const noexcept;
  void validate_board() const noexcept;

public:
  constexpr static const char* startFEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  Board(const std::string &fen = Board::startFEN) noexcept;

  std::string fen() const noexcept;
  std::string to_string() const noexcept;

  inline hash_t hash() const noexcept {
    ASSERT_MSG(m_hash == compute_hash(), "Hash invariant broken");
    return m_hash;
  }

  inline piece_t piece_at(int idx) const noexcept {
    ASSERT(0 <= idx && idx < 120);
    return m_pieces[idx];
  }
  inline bool can_castle(int castle_flag) const noexcept {
    ASSERT(castle_flag == WHITE_LONG || castle_flag == WHITE_SHORT
      || castle_flag == BLACK_LONG || castle_flag == BLACK_SHORT);
    return (m_castle_state & castle_flag) != 0;
  }
};

std::ostream& operator<<(std::ostream &os, const Board& board) noexcept;

#endif /* end of include guard: BOARD_H */
