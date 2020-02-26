
#include "hash.hpp"

#include <iostream>

static bool hash_flag = 0;
hash_t random_hash() noexcept {
#ifdef DEBUG
  static const auto seed = 42069; // Constant seed for debugging
#else
  static const auto seed = std::random_device{}();
#endif
  static std::mt19937_64 gen(seed);
  return gen();
}

hash_t piece_hash[120][16];
hash_t castle_hash[16];
hash_t enpas_hash[120];
hash_t side_hash;

void init_hash() noexcept {
  if (hash_flag) return;
  for (unsigned sq = 0; sq < 120; ++sq) {
    if (valid_square(sq)) {
      enpas_hash[sq] = random_hash();
    } else {
      enpas_hash[sq] = 0;
    }
    for (unsigned piece = 0; piece < 16; ++piece) {
      if (valid_square(sq) && valid_piece(piece)) {
        piece_hash[sq][piece] = random_hash();
      } else {
        piece_hash[sq][piece] = 0;
      }
    }
  }
  for (unsigned castle = 0; castle < 16; ++castle) {
    castle_hash[castle] = random_hash();
  }
  side_hash = random_hash();
  hash_flag = 1;
}
