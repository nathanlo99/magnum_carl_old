
#include "transposition_table.hpp"

#include "perf_counter.hpp"

#include <set>
#include <vector>

TranspositionTable transposition_table;
TranspositionTable quiescence_table;

TableEntry TranspositionTable::query(const hash_t hash) const {
  perf_counter.increment("TT_query");
  const auto it = m_table.find(hash);
  if (it == m_table.end())
    return TableEntry();
  return it->second;
}

void TranspositionTable::insert(const Board &board, const move_t best_move,
                                const int depth, const int value,
                                const NodeType type) {
  perf_counter.increment("TT_insert");
  const hash_t hash = board.hash();
  const size_t half_move = board.m_half_move;
  const TableEntry previous_entry = query(hash);
  const TableEntry new_entry = {hash, best_move, depth, value, type, half_move};

  bool replace = false;

  if (previous_entry.depth > depth) {
    // If the previous entry searched deeper, keep that one
    perf_counter.increment("TT_insert_depth_too_low");
    replace = false;
  } else if (previous_entry.depth < depth) {
    // If the new entry searched deeper, keep it
    perf_counter.increment("TT_insert_depth_improved");
    replace = true;
  } else if (previous_entry.type == Exact) {
    // Now that the depths are the same, we must keep exact entries
    perf_counter.increment("TT_insert_no_replacing_exact");
    replace = false;
  } else if (type == Exact) {
    perf_counter.increment("TT_insert_new_exact");
    replace = true;
  } else if (type == Lower) {
    if (previous_entry.type == Lower) {
      perf_counter.increment("TT_insert_lower_to_lower");
      replace = previous_entry.value > value;
    } else if (previous_entry.type == Upper) {
      perf_counter.increment("TT_insert_lower_to_upper");
      replace = true;
    }
  } else if (type == Upper) {
    perf_counter.increment("TT_insert_upper_to_upper");
    replace = previous_entry.type == Upper && previous_entry.value > value;
  }

  if (replace) {
    perf_counter.increment("TT_insert_replace");
    m_table[hash] = new_entry;
  }
}

std::vector<move_t> get_pv(const Board &board) {
  Board tmp(board);
  std::set<hash_t> seen;
  std::vector<move_t> result;
  while (true) {
    const TableEntry entry = transposition_table.query(tmp.hash());
    if (entry.type == NodeType::None || entry.best_move == 0)
      break;
    const move_t pv_move = entry.best_move;
    tmp.make_move(pv_move);
    result.push_back(pv_move);
    if (seen.count(tmp.hash()) > 0) {
      result.push_back(0);
      break;
    }
    seen.insert(tmp.hash());
  }
  return result;
}

std::string get_pv_string(const Board &board) {
  Board tmp(board);
  const std::vector<move_t> pv_moves = get_pv(board);
  std::stringstream result;
  for (size_t i = 0; i < pv_moves.size(); ++i) {
    const move_t move = pv_moves[i];
    if (move == 0) {
      ASSERT(i + 1 == pv_moves.size());
      result << "...";
    } else {
      result << tmp.algebraic_notation(move);
    }
    tmp.make_move(move);
    if (i + 1 < pv_moves.size())
      result << ", ";
  }
  return result.str();
}
