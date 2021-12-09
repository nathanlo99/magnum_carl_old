
#pragma once

#include <cstdint> // for uint8_t
#include <string>

#include "assert.hpp" // for ASSERT
#include "defs.hpp"

// Organized (hopefully) to optimize bit variance
// colour | major | spec-1 | spec-0
enum Piece {
  WHITE_QUEEN = 0,
  WHITE_ROOK = 1,
  WHITE_PAWN = 2,
  WHITE_BISHOP = 4,
  WHITE_KNIGHT = 5,
  WHITE_KING = 6,
  BLACK_QUEEN = 8,
  BLACK_ROOK = 9,
  BLACK_PAWN = 10,
  BLACK_BISHOP = 12,
  BLACK_KNIGHT = 13,
  BLACK_KING = 14,
  INVALID_PIECE = 15, // 3, 7, 11 unused
};

#ifdef LUT_PIECE

constexpr inline bool valid_piece(const piece_t piece) {
  constexpr bool _valid_piece[16] = {
      1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
  };
  ASSERT(0 <= piece && piece <= 16);
  return _valid_piece[piece];
}

constexpr inline bool is_king(const piece_t piece) {
  constexpr bool _is_king[16] = {
      0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  };
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return _is_king[piece];
}

constexpr inline bool is_castle(const piece_t piece) {
  constexpr bool _is_castle[16] = {
      0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
  };
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return _is_castle[piece];
}

constexpr inline bool is_major(const piece_t piece) {
  constexpr bool _is_major[16] = {
      1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
  };
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return _is_major[piece];
}

constexpr inline bool is_minor(const piece_t piece) {
  constexpr bool _is_minor[16] = {
      0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
  };
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return _is_minor[piece];
}

constexpr inline bool is_pawn(const piece_t piece) {
  constexpr bool _is_pawn[16] = {
      0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  };
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return _is_pawn[piece];
}

constexpr inline bool is_diag(const piece_t piece) {
  constexpr bool _is_diag[16] = {
      1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
  };
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return _is_diag[piece];
}

constexpr inline bool is_ortho(const piece_t piece) {
  constexpr bool _is_ortho[16] = {
      1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
  };
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return _is_ortho[piece];
}

constexpr inline int get_side(const piece_t piece) {
  constexpr int _get_side[16] = {
      0, 0, 0, -1, 0, 0, 0, -1, 1, 1, 1, -1, 1, 1, 1, -1,
  };
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return _get_side[piece];
}

#else
constexpr inline bool valid_piece(const piece_t piece) {
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return (piece & 3) != 3;
}

constexpr inline bool is_king(const piece_t piece) {
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return piece == WHITE_KING || piece == BLACK_KING;
}

constexpr inline bool is_castle(const piece_t piece) {
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return piece == WHITE_KING || piece == WHITE_ROOK || piece == BLACK_KING ||
         piece == BLACK_ROOK;
}

constexpr inline bool is_major(const piece_t piece) {
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return piece == WHITE_QUEEN || piece == WHITE_ROOK || piece == BLACK_QUEEN ||
         piece == BLACK_ROOK;
}

constexpr inline bool is_minor(const piece_t piece) {
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return piece == WHITE_BISHOP || piece == WHITE_KNIGHT ||
         piece == BLACK_BISHOP || piece == BLACK_KNIGHT;
}

constexpr inline bool is_pawn(const piece_t piece) {
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return piece == WHITE_PAWN || piece == BLACK_PAWN;
}

constexpr inline bool is_diag(const piece_t piece) {
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return piece == WHITE_QUEEN || piece == WHITE_BISHOP ||
         piece == BLACK_QUEEN || piece == BLACK_BISHOP;
}

constexpr inline bool is_ortho(const piece_t piece) {
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return piece == WHITE_QUEEN || piece == WHITE_ROOK || piece == BLACK_QUEEN ||
         piece == BLACK_ROOK;
}

constexpr inline int get_side(const piece_t piece) {
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  if (!valid_piece(piece))
    return -1;
  return piece / 8;
}

#endif /* #if LUT_PIECE */

constexpr inline piece_t piece_from_char(const char chr) {
  const char *PIECE_CHAR = "QRP BNK qrp bnk ";
  for (int i = 0; i < 16; ++i)
    if (PIECE_CHAR[i] == chr)
      return i;
  return INVALID_PIECE;
}

constexpr inline char char_from_piece(const piece_t piece) {
  const char *PIECE_CHAR = "QRP BNK qrp bnk ";
  ASSERT_MSG(0 <= piece && piece < 16, "Given piece (%u) out of range", piece);
  return PIECE_CHAR[piece];
}

constexpr inline bool opposite_colours(const piece_t piece1,
                                       const piece_t piece2) {
  ASSERT_MSG(valid_piece(piece1), "Invalid piece1 (%u)", piece1);
  ASSERT_MSG(valid_piece(piece2), "Invalid piece2 (%u)", piece2);
  return (piece1 ^ piece2) >> 3;
}
