
#include "transposition_table.hpp"
#include <set>
#include <vector>

TranspositionTable transposition_table;

TableEntry combine(const TableEntry &cur_entry, const TableEntry &new_entry) {
  // For now, only insert entries which are refinements of previous ones
  if (cur_entry.depth > new_entry.depth)
    return cur_entry;
  if (cur_entry.depth < new_entry.depth)
    return new_entry;
  const int min_value = std::max(cur_entry.min_value, new_entry.min_value);
  const int max_value = std::min(cur_entry.max_value, new_entry.max_value);
  ASSERT_MSG(min_value <= max_value,
             "Got an empty range (%d - %d) for the score", min_value,
             max_value);
  TableEntry result = {cur_entry.hash, 0, cur_entry.depth, min_value,
                       max_value};
  // TODO: Validate this choice of best move
  result.best_move = (result.value() == new_entry.value())
                         ? new_entry.best_move
                         : cur_entry.best_move;
  return result;
}

TableEntry TranspositionTable::query(const hash_t hash) const {
  const auto it = m_table.find(hash);
  if (it == m_table.end())
    return TableEntry();
  return it->second;
}

void TranspositionTable::insert(const Board &board, const move_t best_move,
                                const int depth, const int min_value,
                                const int max_value) {
  const hash_t hash = board.hash();
  const TableEntry previous_entry = query(hash);
  const TableEntry new_entry = {hash, best_move, depth, min_value, max_value};
  m_table[hash] = combine(previous_entry, new_entry);
}

std::vector<move_t> TranspositionTable::get_pv(const Board &board) const {
  Board tmp(board);
  std::set<hash_t> seen;
  std::vector<move_t> result;
  while (true) {
    const TableEntry entry = query(tmp.hash());
    if (!entry.valid() || entry.best_move == 0) {
      break;
    }

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

std::string TranspositionTable::get_pv_string(const Board &board) const {
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
