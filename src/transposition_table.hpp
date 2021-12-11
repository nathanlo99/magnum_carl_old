
#pragma once

#include "board.hpp"
#include "hash.hpp"
#include "move.hpp"

#include <unordered_map>

// The transposition table currently stores a mapping from position hashes, to
// an entry containing the best move, the evaluation, and the depth.

// Searches can then use this information to provide better move ordering, and
// faster evaluations

struct TableEntry {
  move_t best_move;
  int value;
  int depth;

  TableEntry() : best_move(0), value(0), depth(0) {}
  TableEntry(move_t best_move, int value, int depth)
      : best_move(best_move), value(value), depth(depth) {}
};

using Table = std::unordered_map<hash_t, TableEntry>;

extern Table transposition_table;
