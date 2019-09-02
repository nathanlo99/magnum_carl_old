
#ifndef SQUARE_H
#define SQUARE_H

#include <cstdint>

#include "assert.h"

/* We use the 120-board representation
 *   000 001 002 003 004 005 006 007 008 009
 *   010 011 012 013 014 015 016 017 018 019
 *   020 021 022 023 024 025 026 027 028 029
 *   030 031 032 033 034 035 036 037 038 039
 *   040 041 042 043 044 045 046 047 048 049
 *   050 051 052 053 054 055 056 057 058 059
 *   060 061 062 063 064 065 066 067 068 069
 *   070 071 072 073 074 075 076 077 078 079
 *   080 081 082 083 084 085 086 087 088 089
 *   090 091 092 093 094 095 096 097 098 099
 *   100 101 102 103 104 105 106 107 108 109
 *   110 111 112 113 114 115 116 117 118 119
 */

using square_t = uint16_t;

enum Square {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8,
  INVALID_SQUARE = 99,
};

enum SquareRank {
  RANK_1 = 0, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8,
};

enum SquareFile {
  FILE_1 = 0, FILE_2, FILE_3, FILE_4, FILE_5, FILE_6, FILE_7, FILE_8,
};

constexpr inline square_t get_square_64_rc(const int row, const int col) {
  ASSERT(0 <= row && row < 8);
  ASSERT(0 <= col && col < 8);
  return 8 * row + col;
}
constexpr inline square_t get_square_120_rc(const int row, const int col) {
  ASSERT(0 <= row && row < 8);
  ASSERT(0 <= col && col < 8);
  return (row + 2) * 10 + (col + 1);
}

#define LUT_SQUARE
#ifdef LUT_SQUARE

constexpr inline bool valid_square(const square_t square) {
  constexpr bool _valid_square[120] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };
  ASSERT(0 <= square && square < 120);
  return _valid_square[square];
}

constexpr inline int get_square_row(const square_t square) {
  constexpr int _get_square_row[120] = {
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 0, 0, 0, 0, 0, 0, 0, 0, 9,
    9, 1, 1, 1, 1, 1, 1, 1, 1, 9,
    9, 2, 2, 2, 2, 2, 2, 2, 2, 9,
    9, 3, 3, 3, 3, 3, 3, 3, 3, 9,
    9, 4, 4, 4, 4, 4, 4, 4, 4, 9,
    9, 5, 5, 5, 5, 5, 5, 5, 5, 9,
    9, 6, 6, 6, 6, 6, 6, 6, 6, 9,
    9, 7, 7, 7, 7, 7, 7, 7, 7, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  };
  ASSERT(valid_square(square));
  return _get_square_row[square];
}

constexpr inline int get_square_col(const square_t square) {
  constexpr int _get_square_col[120] = {
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 0, 1, 2, 3, 4, 5, 6, 7, 9,
    9, 0, 1, 2, 3, 4, 5, 6, 7, 9,
    9, 0, 1, 2, 3, 4, 5, 6, 7, 9,
    9, 0, 1, 2, 3, 4, 5, 6, 7, 9,
    9, 0, 1, 2, 3, 4, 5, 6, 7, 9,
    9, 0, 1, 2, 3, 4, 5, 6, 7, 9,
    9, 0, 1, 2, 3, 4, 5, 6, 7, 9,
    9, 0, 1, 2, 3, 4, 5, 6, 7, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  };
  ASSERT(valid_square(square));
  return _get_square_col[square];
}

constexpr inline int get_square_64(const square_t square) {
  constexpr int _get_square_64[120] = {
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99,  0,  1,  2,  3,  4,  5,  6,  7, 99,
    99,  8,  9, 10, 11, 12, 13, 14, 15, 99,
    99, 16, 17, 18, 19, 20, 21, 22, 23, 99,
    99, 24, 25, 26, 27, 28, 29, 30, 31, 99,
    99, 32, 33, 34, 35, 36, 37, 38, 39, 99,
    99, 40, 41, 42, 43, 44, 45, 46, 47, 99,
    99, 48, 49, 50, 51, 52, 53, 54, 55, 99,
    99, 56, 57, 58, 59, 60, 61, 62, 63, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
  };
  ASSERT(valid_square(square));
  return _get_square_64[square];
}

constexpr inline square_t get_square_120(const square_t square) {
  constexpr int _get_square_120[64] = {
    21, 22, 23, 24, 25, 26, 27, 28,
    31, 32, 33, 34, 35, 36, 37, 38,
    41, 42, 43, 44, 45, 46, 47, 48,
    51, 52, 53, 54, 55, 56, 57, 58,
    61, 62, 63, 64, 65, 66, 67, 68,
    71, 72, 73, 74, 75, 76, 77, 78,
    81, 82, 83, 84, 85, 86, 87, 88,
    91, 92, 93, 94, 95, 96, 97, 98,
  };
  ASSERT(0 <= square && square < 64);
  return _get_square_120[square];
}

#else
constexpr inline bool valid_square(const square_t square) {
  ASSERT(0 <= square && square < 120);
  const int row = square / 10, col = square % 10;
  return 2 <= row && row <= 9 && 1 <= col && col <= 8;
}
constexpr inline square_t get_square_row(const square_t square) {
  ASSERT(valid_square(square));
  return square / 10 - 2;
}
constexpr inline square_t get_square_col(const square_t square) {
  ASSERT(valid_square(square));
  return square % 10 - 1;
}
constexpr inline square_t get_square_64(const square_t square) {
  ASSERT(valid_square(square));
  return get_square_64_rc(get_square_row(square), get_square_col(square));
}
constexpr inline square_t get_square_120(const square_t square) {
  ASSERT(0 <= square && square < 64);
  const int row = square / 8, col = square % 8;
  return get_square_120_rc(row, col);
}

#endif /* ifdef LUT_SQUARE */

inline std::string string_from_square(const square_t square) {
  if (square == INVALID_SQUARE)
    return "-";
  ASSERT(valid_square(square));
  const size_t row = get_square_row(square), col = get_square_col(square);
  return {(char)(col + 'a'), (char)(row + '1')};
}

#endif /* end of include guard: SQUARE_H */
