
#pragma once

enum Castle {
  WHITE_SHORT = 1,
  WHITE_LONG = 2,
  WHITE_CASTLE = WHITE_SHORT | WHITE_LONG, // 3
  BLACK_SHORT = 4,
  BLACK_LONG = 8,
  BLACK_CASTLE = BLACK_SHORT | BLACK_LONG, // 12
};

constexpr int castle_mask[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // Invalid row
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // Invalid row
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15, // File 1
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // File 2
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // File 3
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // File 4
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // File 5
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // File 6
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // File 7
    15, 7,  15, 15, 15, 3,  15, 15, 11, 15, // File 8
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // Invalid row
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, // Invalid row
};
