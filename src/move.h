
#ifndef MOVE_H
#define MOVE_H

#include <cstdint>

#include "board.h"

/*
MOVE:
- from square - 8 bits
- to square - 8 bits
- flag - 4 bits
- moving piece - 4 bits
- captured piece - 4 bits

FLAG:
promoted | captured | spec1 | spec0
TOTAL: 28 bits
*/

using move_t = uint32_t;

enum MoveFlag {
  QUIET_MOVE = 0, DOUBLE_PAWN_MOVE = 1,
  SHORT_CASTLE_MOVE = 2, LONG_CASTLE_MOVE = 3,
  CAPTURE_MOVE = 4, EN_PASSANT_MOVE = 5,
  PROMOTE_KNIGHT_MOVE = 8,
  PROMOTE_BISHOP_MOVE = 9,
  PROMOTE_ROOK_MOVE = 10,
  PROMOTE_QUEEN_MOVE = 11,
  PROMOTE_KNIGHT_CAPTURE_MOVE = 12,
  PROMOTE_BISHOP_CAPTURE_MOVE = 13,
  PROMOTE_ROOK_CAPTURE_MOVE = 14,
  PROMOTE_QUEEN_CAPTURE_MOVE = 15,
};

constexpr inline move_t
create_move(const square_t from, const square_t to, const MoveFlag flag,
  const piece_t moving, const piece_t captured) {
  return (from << 0) | (to << 8) | (flag << 16) | (moving << 20)
  | (captured << 24);
}

constexpr inline square_t move_from(const move_t move) {
  return (move >> 0) & 0xFF;
}
constexpr inline square_t move_to(const move_t move) {
  return (move >> 8) & 0xFF;
}
constexpr inline MoveFlag move_flag(const move_t move) {
  return (MoveFlag)((move >> 16) & 0xF);
}
constexpr inline piece_t moved_piece(const move_t move) {
  return (move >> 20) & 0xF;
}
constexpr inline piece_t captured_piece(const move_t move) {
  return (move >> 24) & 0xF;
}
constexpr inline bool move_captured(const move_t move) {
  return (move >> 18) & 1;
}

inline void validate_move(const move_t move, const Board board) {
  ASSERT_MSG(valid_square(move_from(move)),
    "Move contained invalid from square");
  ASSERT_MSG(valid_square(move_to(move)),
    "Move contained invalid to square");
  ASSERT_MSG(move_flag(move) != 6 && move_flag(move) != 7,
    "Move contained invalid flag");
  ASSERT_MSG(valid_piece(moved_piece(move)),
    "Move contained invalid moved piece");
  ASSERT_MSG(valid_piece(captured_piece(move))
    || captured_piece(move) == INVALID_PIECE,
    "Move contained neither vaild captured piece nor INVALID_PIECE");
  ASSERT_IF_MSG(move_captured(move), valid_piece(captured_piece(move)),
    "Piece was captured, but invalid captured piece in move");
}

constexpr inline move_t
quiet_move(const square_t from, const square_t to, const piece_t moving) {
  return create_move(from, to, QUIET_MOVE, moving, INVALID_PIECE);
}


#endif /* end of include guard: MOVE_H */
