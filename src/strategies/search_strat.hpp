
#pragma once

#include "board.hpp"
#include "evaluate.hpp"
#include "move.hpp"
#include "strategy.hpp"

#include <vector>

class SearchStrategy : Strategy {
  const bool m_use_book;
  const int m_depth;
  const float m_seconds;

public:
  SearchStrategy(const bool use_book, const int depth, const float max_seconds)
      : m_use_book(use_book), m_depth(depth), m_seconds(max_seconds) {}
  void init(const Board &board) override {}
  move_t make_move(const Board &board,
                   const std::vector<move_t> &move_list) override {
    if (m_use_book) {
      const move_t book_move = opening_book.query(board, 10);
      if (book_move != 0)
        return book_move;
    }
    SearchInfo info(m_seconds, m_depth, false);
    return search(info, board);
  }
};
