
#pragma once

#include "move.hpp"
#include "test_util.hpp"
#include "timeit.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct perft_t {
  std::string fen;
  int depth;
  size_t expected;
  perft_t(const std::string &fen, const int depth, const size_t expected)
      : fen(fen), depth(depth), expected(expected) {}

  std::string to_string() const noexcept {
    std::stringstream res;
    res << "{ \"" << fen << "\", " << depth << ": " << expected << "}";
    return res.str();
  }
};

static std::vector<perft_t>
load_perft(const std::string &file_name = "tests/perft.txt") {
  std::vector<perft_t> result;
  std::ifstream file{file_name};
  std::string line;
  while (std::getline(file, line)) {
    const std::vector<std::string> tokens = split(line, "; ");
    if (tokens.size() != 3)
      throw "Invalid perft format!";
    const std::string fen = tokens[0];
    const int depth = std::stoi(tokens[1]);
    const size_t expected = std::stol(tokens[2]);
    result.emplace_back(fen, depth, expected);
  }

  return result;
}

size_t do_perft(Board &board, const int depth) {
  if (depth == 0)
    return 1;
  size_t result = 0;
  for (const move_t move : board.pseudo_moves()) {
    if (board.make_move(move))
      result += do_perft(board, depth - 1);
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

std::map<std::string, size_t> get_stockfish_perft(const std::string &fen,
                                                  const int depth) {
  std::stringstream cmd;
  cmd << "echo \"position fen " << fen << "\\n"
      << "go perft " << depth << "\" | references/Stockfish/src/stockfish";
  std::stringstream out(exec(cmd.str()));

  std::map<std::string, size_t> expected;
  std::string line;
  while (std::getline(out, line)) {
    if (line.size() < 4)
      continue;
    if (line[4] == ':')
      expected[line.substr(0, 4)] = std::stol(line.substr(6));
    if (line[5] == ':')
      expected[line.substr(0, 5)] = std::stol(line.substr(7));
  }
  return expected;
}

void do_perft_div(Board &board, const int depth) {
  std::cout << "Starting perft divide with FEN:" << std::endl;
  std::cout << "\t" << board.fen() << std::endl;
  std::cout << "and depth = " << depth << std::endl;

  std::map<std::string, size_t> expected =
      get_stockfish_perft(board.fen(), depth);
  std::vector<move_t> legal_moves = board.legal_moves();

  if (depth <= 1 || legal_moves.size() != expected.size()) {
    std::cout << "Found " << board.legal_moves().size()
              << " legal moves:" << std::endl;
    print_simple_move_list(board.legal_moves());

    std::cout << "Stockfish found " << expected.size()
              << " legal moves:" << std::endl;
    for (const auto &[move, expected_perft] : expected) {
      std::cout << move << ", ";
    }
    std::cout << std::endl;
    return;
  }

  for (const move_t move : legal_moves) {
    if (board.make_move(move)) {
      const size_t div_result = do_perft(board, depth - 1);
      const std::string move_name = simple_string_from_move(move);
      if (div_result != expected[move_name]) {
        std::cout << "Perft divide result after making the move " << move_name
                  << " differed:" << std::endl;
        std::cout << "Board FEN: " << board.fen() << std::endl;
        std::cout << "Expected: " << expected[move_name] << std::endl;
        std::cout << "Got     : " << div_result << std::endl;
        std::cout << "----------------------------" << std::endl;
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
    const int depth = perft.depth;
    const size_t expect_num = perft.expected;
    if (depth > max_depth)
      continue;
    const auto diff = timeit([&] {
      const size_t actual_num = do_perft(board, depth);
      if (actual_num != expect_num) {
        WASSERT_MSG(
            actual_num == expect_num,
            "Perft failed for %s with depth %d: expected %lu but got %lu",
            perft.fen.c_str(), depth, expect_num, actual_num);
        do_perft_div(board, depth);
      }
    });
    std::cout << "Done perft " << perft.fen << " with depth " << depth
              << " with " << expect_num << " nodes\n";
    std::cout << "Took " << diff << " ns "
              << "(" << diff / expect_num << " ns / move), "
              << "(" << 1e6 * expect_num / diff << "KNps)" << std::endl;
  }
  return 0;
}
