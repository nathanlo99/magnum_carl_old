
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
  None = 0,
  Exact, // (PV-Node) This node provides an exact evaluated score at the depth
  Lower, // (All-Node) This node was a beta-cut: the score is only a lower bound
  Upper, // (Cut-Node) This score is an upper bound: no moves exceeded value
};

struct TableEntry {
  hash_t hash = 0;
  move_t best_move = 0;
  int depth = 0;
  int value = 0;
  NodeType type = None;
  int half_move = 0;
};

// TODO: Will probably roll my own implementation of this once things are more
// stable
class TranspositionTable {
  using Table = std::unordered_map<hash_t, TableEntry>;
  Table m_table;

public:
  size_t size() const noexcept { return m_table.size(); }
  TableEntry query(const hash_t hash) const {
    const auto it = m_table.find(hash);
    if (it == m_table.end())
      return TableEntry();
    return it->second;
  }

  void insert(const hash_t hash, const move_t best_move, const int depth,
              const int value, const NodeType type, const int half_move) {
    if (m_table.count(hash) > 0) {
      const TableEntry previous_entry = query(hash);
      // For now, only insert entries which are refinements of previous ones
      if (previous_entry.depth > depth)
        return;
      // WARN("Overwriting existing entry in transposition_table");
    }
    m_table[hash] = {hash, best_move, depth, value, type, half_move};
  }
};

extern TranspositionTable transposition_table;
