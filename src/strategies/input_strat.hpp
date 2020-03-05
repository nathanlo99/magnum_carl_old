
#pragma once

#include "strategies/strategy.hpp"
#include <iostream>
#include <string>
#include <vector>

class InputStrategy : Strategy {
public:
  void init(Board board) override {}
  size_t choose(Board board, const std::vector<move_t> &move_list) override {
    std::cout << board << std::endl;
    std::string input;
    while (std::getline(std::cin, input)) {
      for (size_t idx = 0; idx < move_list.size(); ++idx) {
        const move_t move = move_list[idx];
        if (input == string_from_move(move))
          return idx;
      }
      std::cout << "Invalid move: " << input << std::endl;
      std::cout << "Legal moves here are: " << std::endl;
      print_move_list(move_list);
    }
    throw std::runtime_error("No input");
  }
};
