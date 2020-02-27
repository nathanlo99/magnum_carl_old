
#ifndef TEST_PERFT_H
#define TEST_PERFT_H

#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

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
  std::string line;
  std::string fen;
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

size_t do_perft(Board &board, int depth) {
  if (depth == 0) return 1;
  size_t result = 0;
  const auto &move_list = board.legal_moves();
  for (const move_t move : move_list) {
    if (board.make_move(move))
      result += do_perft(board, depth - 1);
    board.unmake_move();
  }
  return result;
}

void do_perft_div(Board &board, int depth) {
  if (depth == 0) return;
  std::cout << board << std::endl;
  std::cout << depth << std::endl;
  for (const move_t move : board.legal_moves()) {
    if (board.make_move(move)) {
      const size_t div_result = do_perft(board, depth - 1);
      std::cout << string_from_move(move) << " " << div_result << "\n";
    }
    board.unmake_move();
  }
}

bool test_perft() {
  const std::vector<perft_t> tests = load_perft();
  for (const auto &perft : tests) {
    Board board(perft.fen);
    for (const auto &[depth, expect_num]: perft.expected) {
      const auto start = std::chrono::high_resolution_clock::now();
      size_t actual_num = do_perft(board, depth);
      if (actual_num != expect_num) {
        do_perft_div(board, depth);
        ASSERT_MSG(actual_num == expect_num,
          "Perft failed for %s with depth %d: expected %lu but got %lu", perft.fen.c_str(), depth, expect_num, actual_num);
      }
      const auto finish = std::chrono::high_resolution_clock::now();
      const auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
      std::cout << "Done perft " << perft.fen << " with depth " << depth << " with " << actual_num << " nodes" << "\n";
      std::cout << "Took " << diff << " ns " << "(" << diff / actual_num << " ns / move" << "), " << "(" << 1e6 * actual_num / diff << "KNps" << ")" << "\n";
    }
  }
  return 0;
}

#endif /* end of include guard: TEST_PERFT_H */
