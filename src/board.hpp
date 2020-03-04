
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <string>
#include <array>
#include <vector>
#include <string_view>
#include <ostream>

#include "piece.hpp"
#include "square.hpp"
#include "castle_state.hpp"
#include "hash.hpp"

#define VARIANT_CHESS

#ifdef VARIANT_CHESS
// NOTE: The max number of any type of piece in play. Keep as small as possible.
enum { MAX_NUM_PIECES = 16 };
// NOTE: The max number of possible moves in any position.
enum { MAX_MOVES = 256 };
#endif

enum { WHITE = 0, BLACK = 1, INVALID_SIDE = -1 };

struct history_t {
  move_t move;
  castle_t castle_state;
  square_t en_passant;
  unsigned int fifty_move;
  hash_t hash;
};

struct Board {
  std::array<piece_t, 120> m_pieces;
  std::array<std::array<square_t, MAX_NUM_PIECES>, 16> m_positions;
  std::array<unsigned, 16> m_num_pieces;
  bool m_next_move_colour;
  castle_t m_castle_state;
  square_t m_en_passant;
  unsigned int m_fifty_move;
  unsigned int m_half_move;
  hash_t m_hash;
  std::vector<history_t> m_history;

  hash_t compute_hash() const noexcept;
  void validate_board() const noexcept;

public:
  constexpr static const char* startFEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  Board(const std::string &fen = Board::startFEN) noexcept;

  inline hash_t hash() const noexcept {
    ASSERT_MSG(m_hash == compute_hash(), "Hash invariant broken");
    return m_hash;
  }
  inline piece_t piece_at(const square_t square) const noexcept {
    ASSERT(0 <= square && square < 120);
    return m_pieces[square];
  }
  inline bool can_castle(const int castle_flag) const noexcept {
    ASSERT(castle_flag == WHITE_LONG || castle_flag == WHITE_SHORT
      || castle_flag == BLACK_LONG || castle_flag == BLACK_SHORT);
    return (m_castle_state & castle_flag) != 0;
  }

  std::string fen() const noexcept;
  std::string to_string() const noexcept;

  bool square_attacked(const square_t sq, const bool side) const noexcept;
  bool king_in_check() const noexcept;
  std::vector<move_t> pseudo_moves(const int side = INVALID_SIDE) const noexcept;
  std::vector<move_t> legal_moves() const noexcept;
  inline bool is_drawn() const noexcept { return m_half_move > 1000 || m_fifty_move > 75; }
  inline void remove_piece(const square_t sq) noexcept;
  inline void add_piece(const square_t sq, const piece_t piece) noexcept;
  inline void set_castle_state(const castle_t state) noexcept;
  inline void set_en_passant(const square_t sq) noexcept;
  inline void move_piece(const square_t from, const square_t to) noexcept;
  inline void update_castling(const square_t sq) noexcept;
  inline void switch_colours() noexcept;
  bool make_move(const move_t move) noexcept;
  void unmake_move() noexcept;
};

std::ostream& operator<<(std::ostream &os, const Board& board) noexcept;

#endif /* end of include guard: BOARD_H */
