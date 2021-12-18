
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
    std::cout << board << std::endl;

    try {
      const move_t move = (board.m_side_to_move == WHITE)
                              ? white_strat.make_move(board, move_list)
                              : black_strat.make_move(board, move_list);
      board.make_move(move);
      result.moves.push_back(move);
    } catch (const std::exception &e) {
      result.result = 0;
      std::cout << "An error occurred within a strategy: '" << e.what() << "'"
                << std::endl;
      return result;
    }
  }
  if (board.is_drawn() || !board.king_in_check()) {
    result.result = 0;
  } else {
    result.result = (board.m_side_to_move == WHITE) ? -1 : 1;
  }
  return result;
}

void print_game_result(const game_record &record) {
  Board board(record.start_fen);
  std::cout << "Starting position:" << std::endl;
  std::cout << board << std::endl;
  for (const move_t move : record.moves) {
    board.make_move(move);
    std::cout << "Played: " << string_from_move(move) << std::endl;
    std::cout << board << std::endl;
  }
  switch (record.result) {
  case -1:
    std::cout << "Black wins!" << std::endl;
    break;
  case 0:
    std::cout << "Draw!" << std::endl;
    break;
  case 1:
    std::cout << "White wins!" << std::endl;
    break;
  default:
    break;
  }
}

game_record manual_play_white(const int max_depth, const float max_seconds,
                              const std::string &fen = Board::startFEN) {
  return simulate_game(InputStrategy(),
                       SearchStrategy(true, max_depth, max_seconds), fen);
}

game_record manual_play_black(const int max_depth, const float max_seconds,
                              const std::string &fen = Board::startFEN) {
  return simulate_game(SearchStrategy(true, max_depth, max_seconds),
                       InputStrategy(), fen);
}

game_record simulate_random(const std::string &fen = Board::startFEN) {
  return simulate_game(RandomStrategy(), RandomStrategy(), fen);
}

game_record simulate_negamax(const int max_depth, const float max_seconds,
                             const std::string &fen = Board::startFEN) {
  return simulate_game(SearchStrategy(true, max_depth, max_seconds),
                       SearchStrategy(true, max_depth, max_seconds), fen);
}
