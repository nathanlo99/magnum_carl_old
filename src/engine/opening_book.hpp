
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
  void read_book(const std::string &file_name);

  std::vector<move_t> query_all(const Board &board) const;
  move_t query(const Board &board, const size_t min_moves = 1000) const;
  std::string book_moves_string(const Board &board) const;

  static void convert_game_list(const std::string &file_name,
                                const std::string &out_file_name);
};

extern OpeningBook opening_book;
