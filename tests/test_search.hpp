
#pragma once

#include "assert.hpp"
#include "evaluate.hpp"
#include "test_util.hpp"
#include "transposition_table.hpp"

struct search_test {
  std::string id;
  std::string fen;
  std::string avoid_move;
  std::string best_move;
};

std::vector<search_test> parse_file(const std::string &file_name) {
  std::ifstream in_file(file_name, std::ifstream::in);
  std::string line;
  std::vector<search_test> result;
  while (std::getline(in_file, line)) {
    const auto tokens = split(line, "; ");
    const std::string fen = tokens[0];
    std::map<std::string, std::string> rest;
    for (size_t i = 1; i < tokens.size(); ++i) {
      const std::string token = tokens[i];
      size_t first_space_idx = token.find(' ');
      const std::string key = token.substr(0, first_space_idx);
      const std::string value = token.substr(first_space_idx + 1);
      rest[key] = value;
    }

    const std::string avoid_move = rest["am"];
    const std::string best_move = rest["bm"];
    const std::string id = rest["id"];

    result.push_back({id, fen, avoid_move, best_move});
  }

  return result;
}

bool test_search_set(const std::string &file_name) {
  const auto search_tests = parse_file(file_name);

  for (const auto &search_test : search_tests) {
    Board board(search_test.fen);
    transposition_table.clear();
    std::cout << board << std::endl;
    std::cout << "Best move: " << search_test.best_move << std::endl;
    std::cout << "Avoid move: " << search_test.avoid_move << std::endl;

    const move_t best_move = get_best_move(board, 200, 150.0);
    const std::string best_move_str = board.algebraic_notation(best_move);
    if (best_move_str == search_test.best_move) {
      std::cout << "Correctly found the best move: " << best_move_str
                << std::endl;
    } else if (best_move_str == search_test.avoid_move) {
      std::cout << "Could not avoid the avoid move: " << best_move_str
                << std::endl;
    } else {
      std::cout << "Found an irrelevant possibility: " << best_move_str
                << std::endl;
    }
  }
  return true;
}

bool test_search() {
  bool result = true;
  result &= test_search_set("tests/search_tests/ccr.search");
  result &= test_search_set("tests/search_tests/kaufman.search");
  return result;
}
