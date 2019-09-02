
#ifndef TEST_PERFT_H
#define TEST_PERFT_H

#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

struct PerftTest {
  std::string fen;
  std::vector<size_t> expected;

  PerftTest(std::string fen, std::vector<size_t> expected) : fen{fen}, expected{expected} {}

  std::string to_string() const noexcept {
    std::stringstream res;
    res << "{\n  \"" << fen << "\",\n  " << "{\n";
    for (const size_t x : expected) {
      res << "    " << x << ", \n";
    }
    res << "  }\n}\n";
    return res.str();
  }
};

static std::vector<PerftTest> load_perft(const std::string &file_name = "tests/perft.txt") {
  std::vector<PerftTest> result;
  std::ifstream file{file_name};
  std::string line;
  std::string fen;
  std::vector<size_t> expected {1};
  while (std::getline(file, line)) {
    expected = {1};

    size_t pos = 0;
    std::string token;
    unsigned depth = 0;
    while ((pos = line.find("; ")) != std::string::npos) {
        token = line.substr(0, pos);
        if (depth == 0)
          fen = token;
        else
          expected.push_back(std::atol(token.c_str()));
        depth++;
        line.erase(0, pos + 2);
    }
    expected.push_back(std::atol(line.c_str()));
    result.emplace_back(fen, expected);
  }

  return result;
}

bool test_perft() {
  std::vector<PerftTest> tests = load_perft();
  for (const auto &perft : tests) {
    Board board{perft.fen};
    std::cout << board << std::endl;
    const auto &legal_moves = board.legal_moves();
    std::cout << "Legal moves (" << legal_moves.size() << "): [" << std::endl;
    for (const auto &move: legal_moves) {
      std::cout << "  " << string_from_move(move) << std::endl;
    }
    std::cout << "]" << std::endl;
  }
  return 0;
}

#endif /* end of include guard: TEST_PERFT_H */
