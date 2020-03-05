
#ifndef TEST_PERFT_H
#define TEST_PERFT_H

#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include "timeit.hpp"

struct perft_t {
  std::string fen;
  std::vector<std::pair<int, size_t>> expected;
  perft_t(const std::string &_fen, const std::vector<std::pair<int, size_t>> _expected):
    fen(_fen), expected(_expected) {}

  std::string to_string() const noexcept {
    std::stringstream res;
    res << "{\n  \"" << fen << "\",\n  " << "{\n";
    for (const auto& [depth, expect_num] : expected) {
      res << "    " << depth << ": " << expect_num << ", \n";
    }
    res << "  }\n}\n";
    return res.str();
  }
};

static std::vector<perft_t> load_perft(const std::string &file_name = "tests/perft.txt") {
  std::vector<perft_t> result;
  std::ifstream file{file_name};
  std::string line, fen;
  std::vector<std::pair<int, size_t>> expected;
  while (std::getline(file, line)) {
    expected.clear();

    size_t pos = 0;
    std::string token;
    unsigned depth = 0;
    while ((pos = line.find("; ")) != std::string::npos) {
      token = line.substr(0, pos);
      if (depth == 0)
        fen = token;
      else
        expected.push_back(std::make_pair(depth, std::stol(token)));
      depth++;
      line.erase(0, pos + 2);
    }
    expected.push_back(std::make_pair(depth, std::stol(line)));
    result.emplace_back(fen, expected);
  }

  return result;
}

size_t do_perft(Board &board, const int depth, const bool reset=true) {
  static std::map<size_t, std::map<hash_t, size_t>> memo;
  if (reset) memo.clear();
  if (depth == 0) return 1;

  // Query memoization data structure
  const hash_t cur_hash = board.hash();
  auto &depth_map = memo[depth];
  const auto &it = depth_map.find(cur_hash);
  if (it != depth_map.end())
    return it->second;

  // Not present, compute and add to memo
  size_t result = 0;
  for (const move_t move : board.pseudo_moves()) {
    if (board.make_move(move))
      result += do_perft(board, depth - 1, false);
    board.unmake_move();
  }
  depth_map[cur_hash] = result;
  return result;
}

void do_perft_div(Board &board, const int depth) {
  if (depth == 0) return;
  for (const move_t move : board.pseudo_moves()) {
    if (board.make_move(move)) {
      const size_t div_result = do_perft(board, depth - 1);
      std::cout << string_from_move(move) << " " << div_result << "\n";
    }
    board.unmake_move();
  }
}

bool test_perft(const std::string &file_name, int max_depth = 5) {
  const std::vector<perft_t> tests = load_perft(file_name);
  for (const auto &perft : tests) {
    Board board(perft.fen);

    for (const auto &test: perft.expected) {
      int depth;
      size_t expect_num;
      std::tie(depth, expect_num) = test;
      if (depth > max_depth) continue;
      const auto diff = timeit([&]{
        const size_t actual_num = do_perft(board, depth);
        if (actual_num != expect_num) {
          do_perft_div(board, depth);
          ASSERT_MSG(actual_num == expect_num,
            "Perft failed for %s with depth %d: expected %lu but got %lu", perft.fen.c_str(), depth, expect_num, actual_num);
        }
      });
      std::cout << "Done perft " << perft.fen << " with depth " << depth << " with " << expect_num << " nodes" << "\n";
      std::cout << "Took " << diff << " ns " << "(" << diff / expect_num << " ns / move" << "), " << "(" << 1e6 * expect_num / diff << "KNps" << ")" << "\n";
    }
  }
  return 0;
}

#endif /* end of include guard: TEST_PERFT_H */
