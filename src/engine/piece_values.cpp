
#include "piece_values.hpp"

int piece_values[16][120];

void init_piece_values() {
  for (piece_t piece = 0; piece < 16; ++piece) {
    for (square_t sq = 0; sq < 120; ++sq) {
      if (!valid_piece(piece) || !valid_square(sq)) {
        piece_values[piece][sq] = 0;
      } else {
        const bool white_piece = piece < 8;
        const square_t flipped_square = white_piece ? flip_square(sq) : sq;
        const square_t table_idx = get_square_64(flipped_square);
        const int raw_offset = square_table[piece][table_idx];
        const int offset = white_piece ? raw_offset : -raw_offset;
        piece_values[piece][sq] = base_piece_values[piece] + offset;
      }
    }
  }
}
