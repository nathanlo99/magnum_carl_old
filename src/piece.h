
#ifndef PIECE_H
#define PIECE_H

#include <cstdint>  // for uint8_t

#include "assert.h" // for ASSERT

using piece_t = uint8_t;

// Organized (hopefully) to optimize bit variance
enum Piece {
  WHITE_KING = 6, WHITE_QUEEN =  0, WHITE_ROOK = 1,
  WHITE_BISHOP = 4, WHITE_KNIGHT =  5, WHITE_PAWN = 2,
  BLACK_KING = 14, BLACK_QUEEN =  8, BLACK_ROOK =  9,
  BLACK_BISHOP = 12, BLACK_KNIGHT = 13, BLACK_PAWN = 10,
  INVALID_PIECE = 15,
};

#ifdef LUT_PIECE

constexpr inline bool
is_valid_piece(const piece_t piece) {
  constexpr static bool _is_valid_piece[16] = {
    1, 1, 1, 0, 1, 1, 1, 0,
    1, 1, 1, 0, 1, 1, 1, 0,
  };
  ASSERT(0 <= piece && piece <= 16);
  return _is_valid_piece[piece];
}

constexpr inline bool
is_king(const piece_t piece) {
  constexpr static bool _is_king[16] = {
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
  };
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _is_king[piece];
}

constexpr inline bool
is_major(const piece_t piece) {
  constexpr static bool _is_major[16] = {
    1, 1, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0,
  };
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _is_major[piece];
}

constexpr inline bool
is_minor(const piece_t piece) {
  constexpr static bool _is_minor[16] = {
    0, 0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0, 0,
  };
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _is_minor[piece];
}

constexpr inline bool
is_pawn(const piece_t piece) {
  constexpr static bool _is_pawn[16] = {
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
  };
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _is_pawn[piece];
}

constexpr inline bool
get_side(const piece_t piece) {
  constexpr static bool _get_side[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
  };
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _get_side[piece];
}

#else
constexpr inline bool
is_valid_piece(const piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  return (piece & 3) != 3;
}

constexpr inline bool
is_king(const piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece == WHITE_KING || piece == BLACK_KING;
}

constexpr inline bool
is_major(const piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece == WHITE_QUEEN || piece == WHITE_ROOK
      || piece == BLACK_QUEEN || piece == BLACK_ROOK;
}

constexpr inline bool
is_minor(const piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece == WHITE_BISHOP || piece == WHITE_KNIGHT
      || piece == BLACK_BISHOP || piece == BLACK_KNIGHT;
}

constexpr inline bool
is_pawn(const piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece == WHITE_PAWN || piece == BLACK_PAWN;
}

constexpr inline bool
get_side(const piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece / 8;
}
#endif /* #if LUT_PIECE */

constexpr inline piece_t
piece_from_char(const char chr) {
  const char* PIECE_CHAR = "QRP.BNK.qrp.bnk.";
  for (int i = 0; i < 16; ++i)
    if (PIECE_CHAR[i] == chr)
      return i;
  return INVALID_PIECE;
}

constexpr inline char
char_from_piece(const piece_t piece) {
  const char* PIECE_CHAR = "QRP.BNK.qrp.bnk.";
  ASSERT(0 <= piece && piece < 16);
  return PIECE_CHAR[piece];
}

constexpr inline bool
opposite_colours(const piece_t piece1, const piece_t piece2) {
  ASSERT(is_valid_piece(piece1));
  ASSERT(is_valid_piece(piece2));
  return (piece1 ^ piece2) >> 3;
}

#endif /* end of include guard: PIECE_H */
