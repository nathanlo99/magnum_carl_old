
#pragma once

#include "board.hpp"
#include "evaluate.hpp"
#include "hash.hpp"
#include "move.hpp"

#include <map>
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
  int value = -SCORE_INFINITY;
  NodeType type = None;
  size_t half_move = 0;

  std::string to_string() const {
    std::stringstream result;
    result << "{";
    result << " depth: " << depth;
    result << ", best_move: " << string_from_move(best_move);
    result << ", value: " << value;
    const std::string type_string = ((type == Exact)   ? "Exact"
                                     : (type == Upper) ? "Upper"
                                                       : "Lower");
    result << ", type: " << type_string;
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
  void clear() noexcept { m_table.clear(); }
  size_t size() const noexcept { return m_table.size(); }
  TableEntry query(const hash_t hash) const;
  void insert(const Board &board, const move_t best_move, const int depth,
              const int value, const NodeType type);
};

std::vector<move_t> get_pv(const Board &board);
std::string get_pv_string(const Board &board);

extern TranspositionTable transposition_table;
