
#pragma once

#include "board.hpp"
#include "hash.hpp"
#include "types.hpp"

#include <string>
#include <unordered_map>
#include <vector>

class OpeningBook {
  std::unordered_map<hash_t, std::vector<move_t>> m_book;

public:
  OpeningBook() {}

  void read(const std::string &file_name, const size_t num_moves);

  std::vector<move_t> query_all(const Board &board) const;
  move_t query(const Board &board) const;
  std::string book_moves_string(const Board &board) const;
};

extern OpeningBook opening_book;
