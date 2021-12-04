
#ifndef TEST_PERFT_H
#define TEST_PERFT_H

#include "timeit.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct perft_t {
  std::string fen;
  std::vector<std::pair<int, size_t>> expected;
  perft_t(const std::string &_fen,
          const std::vector<std::pair<int, size_t>> _expected)
      : fen(_fen), expected(_expected) {}

  std::string to_string() const noexcept {
    std::stringstream res;
    res << "{\n  \"" << fen << "\",\n  "
        << "{\n";
    for (const auto &[depth, expect_num] : expected) {
      res << "    " << depth << ": " << expect_num << ", \n";
    }
    res << "  }\n}\n";
    return res.str();
  }
};

static std::vector<perft_t>
load_perft(const std::string &file_name = "tests/perft.txt") {
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

template <bool memoize>
size_t do_perft(Board &board, const int depth, const bool reset = true);

template <>
size_t do_perft<true>(Board &board, const int depth, const bool reset) {
  static std::map<size_t, std::map<hash_t, size_t>> memo;
  if (reset)
    memo.clear();
  if (depth == 0)
    return 1;

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
      result += do_perft<true>(board, depth - 1, false);
    board.unmake_move();
  }
  depth_map[cur_hash] = result;
  return result;
}

template <>
size_t do_perft<false>(Board &board, const int depth, const bool reset) {
  if (depth == 0)
    return 1;
  size_t result = 0;
  for (const move_t move : board.pseudo_moves()) {
    if (board.make_move(move))
      result += do_perft<false>(board, depth - 1, false);
    board.unmake_move();
  }
  return result;
}

std::string exec(const std::string &cmd) {
  std::array<char, 256> buffer;
  std::stringstream result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                pclose);
  if (!pipe)
    throw std::runtime_error("popen() failed!");
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    result << buffer.data();
  return result.str();
}

void do_perft_div(Board &board, const int depth) {
  std::cout << "Perft divide:" << std::endl;
  std::cout << board.fen() << std::endl;
  std::cout << "Depth = " << depth << std::endl;
  std::cout << board.legal_moves().size() << std::endl;
  print_move_list(board.legal_moves());
  if (depth == 0)
    return;

  std::map<std::string, size_t> actual, expected;
  std::stringstream cmd;
  cmd << "echo \"position fen " << board.fen() << "\\n"
      << "go perft " << depth << "\" | references/Stockfish/src/stockfish";
  std::stringstream out(exec(cmd.str()));
  std::string line;

  while (std::getline(out, line)) {
    if (line.size() < 4 || line[4] != ':')
      continue;
    expected[line.substr(0, 4)] = std::stol(line.substr(6));
  }

  for (const move_t move : board.pseudo_moves()) {
    if (board.make_move(move)) {
      const size_t div_result = do_perft<true>(board, depth - 1);
      std::string move_name = simple_string_from_move(move);
      actual[move_name] = div_result;
      if (div_result != expected[move_name]) {
        std::cout << move_name << std::endl;
        do_perft_div(board, depth - 1);
        return;
      }
    }
    board.unmake_move();
  }
}

bool test_perft(const std::string &file_name, int max_depth) {
  const std::vector<perft_t> tests = load_perft(file_name);
  for (const auto &perft : tests) {
    Board board(perft.fen);

    for (const auto &test : perft.expected) {
      int depth;
      size_t expect_num;
      std::tie(depth, expect_num) = test;
      if (depth > max_depth)
        continue;
      const auto diff = timeit([&] {
        const size_t actual_num = do_perft<true>(board, depth);
        if (actual_num != expect_num) {
          do_perft_div(board, depth);
          ASSERT_MSG(
              actual_num == expect_num,
              "Perft failed for %s with depth %d: expected %lu but got %lu",
              perft.fen.c_str(), depth, expect_num, actual_num);
        }
      });
      std::cout << "Done perft " << perft.fen << " with depth " << depth
                << " with " << expect_num << " nodes"
                << "\n";
      std::cout << "Took " << diff << " ns "
                << "(" << diff / expect_num << " ns / move"
                << "), "
                << "(" << 1e6 * expect_num / diff << "KNps"
                << ")"
                << "\n";
    }
  }
  return 0;
}

#endif /* end of include guard: TEST_PERFT_H */
