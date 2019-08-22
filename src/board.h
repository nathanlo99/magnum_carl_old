
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

  constexpr static const char* startFEN =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  hash_t compute_hash() const noexcept;
  void validate_board() const noexcept;

public:
  Board(const std::string &fen = Board::startFEN) noexcept;

  std::string fen() const noexcept;
  std::string to_string() const noexcept;

  hash_t hash() const noexcept {
    ASSERT_MSG(m_hash == compute_hash(), "Hash invariant broken");
    return m_hash;
  }
};

std::ostream& operator<<(std::ostream &os, const Board& board) noexcept;

#endif /* end of include guard: BOARD_H */
