
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
    std::cout << board.to_string(board.m_side_to_move) << std::endl;
    std::string input;
    std::cout << "Enter a move: " << std::flush;
    while (std::getline(std::cin, input)) {
      for (const move_t move : move_list) {
        if (input == string_from_move(move) ||
            input == board.algebraic_notation(move))
          return move;
      }
      std::cout << "Invalid move: " << input << std::endl;
      std::cout << "Legal moves here are: " << std::endl;
      print_algebraic_move_list(board, move_list);

      std::cout << "Try again: " << std::flush;
    }
    throw std::runtime_error("No input");
  }
};
