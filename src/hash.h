
#ifndef HASH_H
#define HASH_H

#include <cstdint>
#include <random>
#include <cmath>

#include "defs.h"
#include "piece.h"
#include "square.h"

extern hash_t piece_hash[120][16];
extern hash_t castle_hash[16];
extern hash_t enpas_hash[120];
extern hash_t side_hash;

void init_hash() noexcept;

#endif /* end of include guard: HASH_H */
