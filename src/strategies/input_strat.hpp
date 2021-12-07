
#pragma once

#include "strategy.hpp"
#include <iostream>
#include <string>
#include <vector>

class InputStrategy : Strategy {
public:
  void init(const Board &board) override {}
  move_t make_move(const Board &board,
                   const std::vector<move_t> &move_list) override {
    std::cout << board << std::endl;
    std::string input;
    while (std::getline(std::cin, input)) {
      for (const move_t move : move_list) {
        if (input == string_from_move(move))
          return move;
      }
      std::cout << "Invalid move: " << input << std::endl;
      std::cout << "Legal moves here are: " << std::endl;
      print_move_list(move_list);
    }
    throw std::runtime_error("No input");
  }
};
