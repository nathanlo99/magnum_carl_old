
#pragma once

#include "board.hpp"
#include "evaluate.hpp"
#include "hash.hpp"
#include "move.hpp"

#include <unordered_map>

// The transposition table currently stores a mapping from position hashes, to
// an entry containing the best move, the evaluation, the depth, the node type,
// and the half move at which this was computed.

// Searches can then use this information to provide better move ordering, and
// faster evaluations
struct TableEntry {
  hash_t hash = 0;
  move_t best_move = 0;
  int depth = 0;
  int min_value = MATE;
  int max_value = -MATE;

  bool valid() const { return min_value <= max_value; }
  int value() const { return min_value == -MATE ? max_value : min_value; }

  std::string to_string() const {
    std::stringstream result;
    result << "{";
    result << " depth: " << depth;
    result << ", value: [" << min_value << ", " << max_value << "]";
    result << "}";
    return result.str();
  }
};

// TODO: Will probably roll my own implementation of this once things are more
// stable
class TranspositionTable {
  using Table = std::unordered_map<hash_t, TableEntry>;
  Table m_table;

public:
  size_t size() const noexcept { return m_table.size(); }
  TableEntry query(const hash_t hash) const;
  void insert(const Board &board, const move_t best_move, const int depth,
              const int min_value, const int max_value);
  std::vector<move_t> get_pv(const Board &board) const;
  std::string get_pv_string(const Board &board) const;
};

extern TranspositionTable transposition_table;
