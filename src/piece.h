
#ifndef PIECE_H
#define PIECE_H

#include <cstdint>  // for uint8_t

#include "assert.h" // for ASSERT

using piece_t = uint8_t;

// Organized (hopefully) to optimize bit variance
enum Piece {
  WK =  6, WQ =  0, WR =  1, WB =  4, WN =  5, WP =  2,
  BK = 14, BQ =  8, BR =  9, BB = 12, BN = 13, BP = 10,
  INVALID_PIECE = 15,
};

#ifdef LUT_PIECE
const static bool _is_valid_piece[16] = {
  1, 1, 1, 0, 1, 1, 1, 0,
  1, 1, 1, 0, 1, 1, 1, 0,
};
constexpr inline bool is_valid_piece(piece_t piece) {
  ASSERT(0 <= piece && piece <= 16);
  return _is_valid_piece[piece];
}
const static bool _is_king[16] = {
  0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 1, 0,
};
constexpr inline bool is_king(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _is_king[piece];
}
const static bool _is_major[16] = {
  1, 1, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0,
};
constexpr inline bool is_major(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _is_major[piece];
}
const static bool _is_minor[16] = {
  0, 0, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0,
};
constexpr inline bool is_minor(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _is_minor[piece];
}
const static bool _is_pawn[16] = {
  0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0,
};
constexpr inline bool is_pawn(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _is_pawn[piece];
}
const static bool _get_side[16] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1,
};
constexpr inline bool get_side(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return _get_side[piece];
}
#else
constexpr inline bool is_valid_piece(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  return (piece & 3) != 3;
}
constexpr inline bool is_king(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece == WK || piece == BK;
}
constexpr inline bool is_major(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece == WQ || piece == WR || piece == BQ || piece == BR;
}
constexpr inline bool is_minor(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece == WB || piece == WN || piece == BB || piece == BN;
}
constexpr inline bool is_pawn(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece == WP || piece == BP;
}
constexpr inline bool get_side(piece_t piece) {
  ASSERT(0 <= piece && piece < 16);
  ASSERT(is_valid_piece(piece));
  return piece / 8;
}
#endif /* #if LUT_PIECE */


#endif /* end of include guard: PIECE_H */
