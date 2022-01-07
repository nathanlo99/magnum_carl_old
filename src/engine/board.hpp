
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

  std::vector<move_t>
  pseudo_moves(const bool generate_quiet_moves = true) const noexcept;
  std::vector<move_t>
  legal_moves(const bool generate_quiet_moves = true) const noexcept;
  bool has_legal_moves() const noexcept;

  constexpr bool has_major_pieces(const int side) const noexcept {
    const piece_t rook = (side == WHITE) ? WHITE_ROOK : BLACK_ROOK;
    const piece_t queen = (side == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
    return m_num_pieces[rook] + m_num_pieces[queen] > 0;
  }

  constexpr int num_pieces() const noexcept {
    int result = 0;
    for (piece_t piece = 0; piece < 16; ++piece) {
      result += m_num_pieces[piece];
    }
    return result;
  }

  // A value which can only increase over the course of a game
  constexpr int fifty_move_monovariant() const noexcept {
    // Pawns can only move forward, pieces can only be captured, and pawns may
    // promote

    // Assign 1 to every non-pawn piece, and 2 to a white pawn on the 7th rank,
    // 3 to a white pawn on the 6th rank, etc. and 7 to a white pawn on the
    // second rank. Similarly, assign 2 to a black pawn on the 2nd rank, etc.

    int result = 0;
    result += m_num_pieces[WHITE_KNIGHT] + m_num_pieces[WHITE_BISHOP] +
              m_num_pieces[WHITE_ROOK] + m_num_pieces[WHITE_QUEEN];
    result += m_num_pieces[BLACK_KNIGHT] + m_num_pieces[BLACK_BISHOP] +
              m_num_pieces[BLACK_ROOK] + m_num_pieces[BLACK_QUEEN];
    const size_t num_white_pawns = m_num_pieces[WHITE_PAWN];
    const size_t num_black_pawns = m_num_pieces[BLACK_PAWN];
    for (size_t idx = 0; idx < num_white_pawns; ++idx) {
      result += 9 - get_square_row(m_positions[WHITE_PAWN][idx]);
    }
    for (size_t idx = 0; idx < num_black_pawns; ++idx) {
      result += get_square_row(m_positions[BLACK_PAWN][idx]);
    }
    return 126 - result;
  }

  constexpr bool insufficient_material() const noexcept;
  constexpr inline int count_repetitions() const noexcept {
    int result = 0;
    for (size_t idx = m_half_move - m_fifty_move; idx < m_half_move; ++idx) {
      if (m_history[idx].hash == m_hash)
        result++;
    }
    return result;
  }
  constexpr inline bool is_repeated() const noexcept {
    return count_repetitions() > 0;
  }
  constexpr inline bool is_three_fold() const noexcept {
    return count_repetitions() >= 2;
  }
  bool is_drawn() const noexcept;
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
  void make_null_move() noexcept;
  void unmake_null_move() noexcept;

  std::string algebraic_notation(const move_t move) const;
};

std::ostream &operator<<(std::ostream &os, const Board &board) noexcept;
void print_move_list(const std::vector<move_t> &move_list);
void print_simple_move_list(const std::vector<move_t> &move_list);
void print_algebraic_move_list(const Board &board,
                               const std::vector<move_t> &move_list);
