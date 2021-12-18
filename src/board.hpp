
#pragma once

#include <array>
#include <map>
#include <ostream>
#include <stdint.h>
#include <string>
#include <unordered_set>
#include <vector>

#include "castle_state.hpp"
#include "hash.hpp"
#include "piece.hpp"
#include "square.hpp"

// NOTE: The max number of any type of piece in play. Keep as small as possible.
enum { MAX_PIECE_FREQ = 10 };
// NOTE: The max number of possible moves in any position.
// SOURCE: https://www.stmintz.com/ccc/index.php?id=424966 (cites 218)
enum { MAX_POSITION_MOVES = 256 };
// NOTE: The initial size of the reserved move list when generating moves
//       Should be close to the twice true average branching factor, which is 35
enum { START_POSITION_MOVES = 64 };

enum { WHITE = 0, BLACK = 1, INVALID_SIDE = -1 };

enum {
  MOVEGEN_NORMAL = 1,
  MOVEGEN_CAPTURES = 2,
  MOVEGEN_PROMOTIONS = 4,
  MOVEGEN_ALL = MOVEGEN_NORMAL | MOVEGEN_CAPTURES | MOVEGEN_PROMOTIONS,
};

struct history_t {
  move_t move;
  castle_t castle_state;
  square_t en_passant;
  unsigned int fifty_move;
  hash_t hash;
};

struct Board {
  std::array<piece_t, 120> m_pieces;
  std::array<std::array<square_t, MAX_PIECE_FREQ>, 16> m_positions;
  std::array<unsigned, 16> m_num_pieces;
  bool m_side_to_move;
  castle_t m_castle_state;
  square_t m_en_passant;
  unsigned int m_fifty_move;
  unsigned int m_half_move;
  hash_t m_hash;
  std::vector<history_t> m_history;
  std::unordered_multiset<hash_t> m_position_freq;

  hash_t compute_hash() const noexcept;
  void validate_board() const noexcept;

public:
  constexpr static const char *startFEN =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  Board(const std::string &fen = Board::startFEN) noexcept;

  constexpr inline hash_t hash() const noexcept {
    ASSERT_MSG(m_hash == compute_hash(), "Hash invariant broken");
    return m_hash;
  }
  constexpr inline piece_t piece_at(const square_t square) const noexcept {
    ASSERT(0 <= square && square < 120);
    return m_pieces[square];
  }
  constexpr inline bool can_castle(const int castle_flag) const noexcept {
    ASSERT(castle_flag == WHITE_LONG || castle_flag == WHITE_SHORT ||
           castle_flag == BLACK_LONG || castle_flag == BLACK_SHORT);
    return (m_castle_state & castle_flag) != 0;
  }

  std::string fen() const noexcept;
  std::string to_string(const int side = WHITE) const noexcept;

  bool square_attacked(const square_t sq, const bool side) const noexcept;
  bool king_in_check() const noexcept;
  std::vector<move_t> pseudo_moves(const int spec = MOVEGEN_ALL) const noexcept;
  std::vector<move_t> legal_moves(const int spec = MOVEGEN_ALL) const noexcept;
  constexpr inline bool is_three_fold() const noexcept {
    // Since the position frequency map is updated with a given position only
    // when that position is _played on_, if we reach a position with frequency
    // two, it has appeared twice, not counting the current board state, which
    // is a threefold repetition.
    return m_position_freq.count(m_hash) >= 2;
  }
  constexpr inline bool is_drawn() const noexcept {
    return m_half_move > 1000 || m_fifty_move >= 50 || is_three_fold();
  }
  bool is_endgame() const noexcept;
  inline void remove_piece(const square_t sq) noexcept;
  inline void add_piece(const square_t sq, const piece_t piece) noexcept;
  inline void set_castle_state(const castle_t state) noexcept;
  inline void set_en_passant(const square_t sq) noexcept;
  inline void move_piece(const square_t from, const square_t to) noexcept;
  inline void update_castling(const square_t from, const square_t to) noexcept;
  inline void switch_colours() noexcept;
  bool make_move(const move_t move) noexcept;
  void unmake_move() noexcept;
};

std::ostream &operator<<(std::ostream &os, const Board &board) noexcept;
void print_move_list(const std::vector<move_t> &move_list);
void print_simple_move_list(const std::vector<move_t> &move_list);

std::string ambiguous_algebraic_notation(const move_t move);
std::string algebraic_notation(const std::vector<move_t> &move_list,
                               const move_t move);
