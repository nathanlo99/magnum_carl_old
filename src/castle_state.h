
#ifndef CASTLE_STATE_H
#define CASTLE_STATE_H

#include <cstdint>

#include "assert.h"

using castle_t = uint8_t;

enum Castle {
  WHITE_SHORT = 0x1, WHITE_LONG = 0x2,
  BLACK_SHORT = 0x4, BLACK_LONG = 0x8,
};

#endif /* end of include guard: CASTLE_STATE_H */
