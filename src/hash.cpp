
#include "hash.h"

static bool hash_flag = 0;
static inline hash_t random_hash() noexcept {
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  return gen();
}

hash_t piece_hash[120][16];
hash_t castle_hash[16];
hash_t enpas_hash[120];

void init_hash() noexcept {
  if (hash_flag) return;
  for (unsigned sq = 0; sq < 120; ++sq) {
    if (valid_square(sq)) {
      enpas_hash[sq] = random_hash();
    } else {
      enpas_hash[sq] = 0;
    }
    for (unsigned piece = 0; piece < 16; ++piece) {
      if (valid_square(sq) && is_valid_piece(piece)) {
        piece_hash[sq][piece] = random_hash();
      } else {
        piece_hash[sq][piece] = 0;
      }
    }
  }
  for (unsigned castle = 0; castle < 16; ++castle) {
    castle_hash[castle] = random_hash();
  }
  hash_flag = 1;
}
