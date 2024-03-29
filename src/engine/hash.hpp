
#pragma once

#include <cmath>
#include <cstdint>
#include <random>

#include "piece.hpp"
#include "square.hpp"
#include "types.hpp"

void init_hash() noexcept;
hash_t random_hash() noexcept;
extern hash_t piece_hash[120][16];
extern hash_t castle_hash[16];
extern hash_t enpas_hash[120];
extern hash_t side_hash;
