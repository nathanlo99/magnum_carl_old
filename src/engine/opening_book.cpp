
#include "opening_book.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

OpeningBook opening_book;

void OpeningBook::read(const std::string &file_name, const size_t num_moves) {
  std::cout << "Loading games from '" << file_name << "'..." << std::endl;
  std::ifstream in(file_name, std::ifstream::in);

  std::string game;
  size_t game_idx = 0;
  while (std::getline(in, game)) {
    game_idx++;
    std::istringstream iss(game);
    std::string move_str;
    Board board;
    while (iss >> move_str && board.m_half_move < num_moves) {
      bool played = false;
      const auto legal_moves = board.legal_moves();
      for (const move_t list_move : legal_moves) {
        if (board.algebraic_notation(list_move) == move_str) {
          m_book[board.hash()].push_back(list_move);
          board.make_move(list_move);
          played = true;
          break;
        }
      }
      if (!played) {
        std::cout << game_idx << " " << board.m_half_move << " " << move_str
                  << std::endl;
        break;
      }
    }
  }
  std::cout << "Catalogued " << m_book.size() << " positions into opening book"
            << std::endl;
}

std::vector<move_t> OpeningBook::query_all(const Board &board) const {
  const auto it = m_book.find(board.hash());
  if (it == m_book.end())
    return std::vector<move_t>();
  return it->second;
}

move_t OpeningBook::query(const Board &board) const {
  const auto moves = query_all(board);
  if (moves.empty())
    return 0;
  const size_t idx = random_hash() % moves.size();
  return moves[idx];
}

std::string OpeningBook::book_moves_string(const Board &board) const {
  std::stringstream result;

  std::vector<move_t> moves = query_all(board);
  std::map<move_t, int> freqs;
  for (const move_t move : moves) {
    freqs[move]++;
  }
  std::vector<std::pair<int, move_t>> freq_pairs;
  for (const auto &[move, freq] : freqs) {
    freq_pairs.emplace_back(freq, move);
  }
  std::sort(freq_pairs.begin(), freq_pairs.end(),
            std::greater<std::pair<int, move_t>>());

  const auto legal_moves = board.legal_moves();
  for (const auto &[freq, move] : freq_pairs) {
    result << board.algebraic_notation(move) << ": " << freq << ", ";
  }
  return result.str();
}
