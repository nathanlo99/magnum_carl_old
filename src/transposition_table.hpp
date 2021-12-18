
#pragma once

#include "board.hpp"
#include "hash.hpp"
#include "move.hpp"

#include <unordered_map>

// The transposition table currently stores a mapping from position hashes, to
// an entry containing the best move, the evaluation, the depth, the node type,
// and the half move at which this was computed.

// Searches can then use this information to provide better move ordering, and
// faster evaluations

enum NodeType {
  Exact, // (PV-Node) This node provides an exact evaluated score at the depth
  Lower, // (All-Node) This node was a beta-cut: the score is only a lower bound
  Upper, // (Cut-Node) This score is an upper bound: no moves exceeded value
};

struct TableEntry {
  hash_t hash = 0;
  move_t best_move = 0;
  int depth = 0;
  int value = 0;
  NodeType type = Exact;
  int half_move = 0;
};

class TranspositionTable {
  using Table = std::unordered_map<hash_t, TableEntry>;
};

extern TranspositionTable transposition_table;
