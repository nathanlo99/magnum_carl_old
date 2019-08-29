
#ifndef MOVE_H
#define MOVE_H

#include <cstdint>

#include "assert.h"
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
constexpr inline bool move_promoted(const move_t move) {
  return (move >> 19) & 1;
}

inline void validate_move(const move_t move, const Board board) {
  // General tests
  ASSERT_MSG(valid_square(move_from(move)),
    "Move contained invalid from square (%u)", move_from(move));
  ASSERT_MSG(valid_square(move_to(move)),
    "Move contained invalid to square (%u)", move_to(move));
  ASSERT_MSG(move_flag(move) != 6 && move_flag(move) != 7,
    "Move contained invalid flag (%u)", move_flag(move));
  ASSERT_MSG(valid_piece(moved_piece(move)),
    "Move contained invalid moved piece (%u)", moved_piece(move));
  ASSERT_MSG(valid_piece(captured_piece(move))
    || captured_piece(move) == INVALID_PIECE,
    "Move contained neither vaild captured piece nor INVALID_PIECE (%u)",
    captured_piece(move));

  // Board checks
  ASSERT_MSG(moved_piece(move) == board.piece_at(move_from(move)),
    "Moved piece (%u) disagrees with board (board[%u] = %u)",
    moved_piece(move), move_from(move), board.piece_at(move_from(move)));
  ASSERT_IF_MSG(move_captured(move),
    captured_piece(move) == board.piece_at(move_to(move)),
    "Captured piece (%u) disagrees with board (board[%u] = %u)",
    captured_piece(move), move_to(move), board.piece_at(move_to(move)));

  // Capture moves
  ASSERT_IF_MSG(move_captured(move), valid_piece(captured_piece(move)),
    "Piece was captured, but invalid captured piece (%u) in move",
    captured_piece(move));
  ASSERT_IF_MSG(move_captured(move),
    opposite_colours(captured_piece(move), moved_piece(move)),
    "Captured own piece (moved = %u, captured = %u)",
    moved_piece(move), captured_piece(move));
  ASSERT_IF_MSG(move_captured(move), !is_king(captured_piece(move)),
    "Captured king (%u)", captured_piece(move));

  // Promotion moves
  ASSERT_IF_MSG(move_promoted(move), is_pawn(moved_piece(move)),
    "Promoted from non-pawn piece (%u)", moved_piece(move));
  ASSERT_IF_MSG(move_promoted(move) && get_side(moved_piece(move)) == WHITE,
    get_square_row(move_to(move)) == RANK_8,
    "White pawn promoted to square (%u) not on 8th rank", move_to(move));
  ASSERT_IF_MSG(move_promoted(move) && get_side(moved_piece(move)) == BLACK,
    get_square_row(move_to(move)) == RANK_1,
    "Black pawn promoted to square (%u) not on 1st rank", move_to(move));

  // Castling moves
  ASSERT_IF_MSG(move_flag(move) == SHORT_CASTLE_MOVE
    && get_side(moved_piece(move)) == WHITE,
    moved_piece(move) == WHITE_KING
    && move_from(move) == E1 && move_to(move) == G1
    && board.can_castle(WHITE_SHORT),
    "Short white castle move: Moved piece (%u) was not wK OR squares (%u -> %u)"
    " did not match OR castling disallowed",
    moved_piece(move), move_from(move), move_to(move));

  ASSERT_IF_MSG(move_flag(move) == LONG_CASTLE_MOVE
    && get_side(moved_piece(move)) == WHITE,
    moved_piece(move) == WHITE_KING
    && move_from(move) == E1 && move_to(move) == C1
    && board.can_castle(WHITE_LONG),
    "Long white castle move: Moved piece (%u) was not wK OR squares (%u -> %u)"
    " did not match OR castling disallowed",
    moved_piece(move), move_from(move), move_to(move));

  ASSERT_IF_MSG(move_flag(move) == SHORT_CASTLE_MOVE
    && get_side(moved_piece(move)) == BLACK,
    moved_piece(move) == BLACK_KING
    && move_from(move) == E8 && move_to(move) == G8
    && board.can_castle(BLACK_SHORT),
    "Short black castle move: Moved piece (%u) was not bK OR squares (%u -> %u)"
    " did not match OR castling disallowed",
    moved_piece(move), move_from(move), move_to(move));

  ASSERT_IF_MSG(move_flag(move) == LONG_CASTLE_MOVE
    && get_side(moved_piece(move)) == BLACK,
    moved_piece(move) == BLACK_KING
    && move_from(move) == E8 && move_to(move) == C8
    && board.can_castle(BLACK_LONG),
    "Long black castle move: Moved piece (%u) was not bK OR squares (%u -> %u)"
    " did not match OR castling disallowed",
    moved_piece(move), move_from(move), move_to(move));
}

constexpr inline move_t
quiet_move(const square_t from, const square_t to, const piece_t moving) {
  return create_move(from, to, QUIET_MOVE, moving, INVALID_PIECE);
}


#endif /* end of include guard: MOVE_H */
