
#include "transposition_table.hpp"

#include "perf_counter.hpp"

#include <set>
#include <vector>

TranspositionTable transposition_table;
TranspositionTable quiescence_table;

TableEntry TranspositionTable::query(const hash_t hash) const {
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
  // For now, only insert entries which are refinements of previous ones
  if (previous_entry.depth > depth) {
    perf_counter.increment("TT_insert_depth_too_low");
    replace = false;
  } else if (previous_entry.depth < depth) {
    perf_counter.increment("TT_insert_depth_improved");
    replace = true;
  } else if (previous_entry.value > value) {
    perf_counter.increment("TT_insert_value_too_low");
    replace = false;
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
