
#pragma once

#include <string>
#include "board.hpp"
#include <iostream>
#include <vector>
#include "strategies/random_strat.hpp"
#include "strategies/input_strat.hpp"

struct game_record {
  std::string fen;
  int result;
  std::vector<move_t> moves;
};

template <typename WhiteStrategy, typename BlackStrategy>
game_record simulate_game(WhiteStrategy white_strat, BlackStrategy black_strat, const std::string &fen = Board::startFEN) {
  game_record result;
  result.fen = fen;
  Board board(fen);
  white_strat.init(board);
  black_strat.init(board);
  while (true) {
    const auto &move_list = board.legal_moves();
    if (move_list.empty()) break;
    const size_t move_idx = (board.m_next_move_colour == WHITE) ? white_strat.choose(board, move_list) : black_strat.choose(board, move_list);
    board.make_move(move_list[move_idx]);
    result.moves.push_back(move_list[move_idx]);
  }
  if (board.is_drawn() || !board.king_in_check()) {
    result.result = 0;
  } else {
    result.result = (board.m_next_move_colour == WHITE) ? -1 : 1;
  }
  return result;
}

game_record manual_play(const std::string &fen = Board::startFEN) {
  return simulate_game(InputStrategy(), RandomStrategy(), fen);
}

game_record simulate_random(const std::string &fen = Board::startFEN) {
  return simulate_game(RandomStrategy(), RandomStrategy(), fen);
}
