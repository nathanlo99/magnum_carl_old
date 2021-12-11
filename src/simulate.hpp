
#pragma once

#include "board.hpp"
#include "strategies/input_strat.hpp"
#include "strategies/random_strat.hpp"
#include "strategies/search_strat.hpp"
#include <iostream>
#include <string>
#include <vector>

struct game_record {
  std::string start_fen;
  int result;
  std::vector<move_t> moves;
};

template <typename WhiteStrategy, typename BlackStrategy>
game_record simulate_game(WhiteStrategy white_strat, BlackStrategy black_strat,
                          const std::string &fen = Board::startFEN) {
  game_record result;
  result.start_fen = fen;
  Board board(fen);
  white_strat.init(board);
  black_strat.init(board);
  while (true) {
    const auto &move_list = board.legal_moves();
    if (board.is_drawn() || move_list.empty())
      break;
    const move_t move = (board.m_next_move_colour == WHITE)
                            ? white_strat.make_move(board, move_list)
                            : black_strat.make_move(board, move_list);
    board.make_move(move);
    result.moves.push_back(move);
  }
  if (board.is_drawn() || !board.king_in_check()) {
    result.result = 0;
  } else {
    result.result = (board.m_next_move_colour == WHITE) ? -1 : 1;
  }
  return result;
}

game_record manual_play_white(const std::string &fen = Board::startFEN) {
  return simulate_game(InputStrategy(), SearchStrategy(), fen);
}

game_record manual_play_black(const std::string &fen = Board::startFEN) {
  return simulate_game(SearchStrategy(), InputStrategy(), fen);
}

game_record simulate_random(const std::string &fen = Board::startFEN) {
  return simulate_game(RandomStrategy(), RandomStrategy(), fen);
}

game_record simulate_negamax(const std::string &fen = Board::startFEN) {
  return simulate_game(SearchStrategy(), SearchStrategy(), fen);
}
