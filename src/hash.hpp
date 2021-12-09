
#pragma once

#include <cmath>
#include <cstdint>
#include <random>

#include "defs.hpp"
#include "piece.hpp"
#include "square.hpp"

hash_t random_hash() noexcept;
extern hash_t piece_hash[120][16];
extern hash_t castle_hash[16];
extern hash_t enpas_hash[120];
extern hash_t side_hash;

void init_hash() noexcept;
