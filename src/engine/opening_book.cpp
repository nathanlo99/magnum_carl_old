
#include "opening_book.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

OpeningBook opening_book;

void OpeningBook::convert_game_list(const std::string &file_name,
                                    const std::string &out_file_name) {

  std::ifstream in(file_name, std::ifstream::in);
  std::ofstream out(out_file_name, std::ofstream::out);

  std::unordered_map<hash_t, std::vector<move_t>> book;
  std::unordered_map<hash_t, std::string> hash_to_fen;

  std::string game;
  size_t game_idx = 0;
  while (std::getline(in, game)) {
    game_idx++;
    std::istringstream iss(game);
    std::string move_str;
    Board board;
    while (iss >> move_str) {
      bool played = false;
      const auto legal_moves = board.legal_moves();
      for (const move_t list_move : legal_moves) {
        if (board.algebraic_notation(list_move) == move_str) {
          hash_to_fen[board.hash()] = board.fen();
          book[board.hash()].push_back(list_move);
          board.make_move(list_move);
          played = true;
          break;
        }
      }
      if (!played) {
        throw std::runtime_error("Could not play move: " + move_str);
      }
    }
  }

  // Produce the book
  for (const auto &[hash, moves] : book) {
    if (moves.size() < 10)
      continue;
    const std::string fen = hash_to_fen[hash];
    out << fen << std::endl;
    std::map<move_t, size_t> freqs;
    for (const move_t move : moves) {
      out << move << " ";
    }
    out << std::endl;
  }
}

void OpeningBook::read_book(const std::string &file_name) {
  // std::cout << "Loading book from '" << file_name << "'..." << std::endl;
  std::ifstream in_file(file_name, std::ifstream::in);

  std::string fen, moves;
  size_t line_idx = 0;
  while (std::getline(in_file, fen)) {
    Board board(fen);
    const hash_t hash = board.hash();

    std::getline(in_file, moves);

    std::istringstream iss(moves);
    move_t move;
    while (iss >> move) {
      m_book[hash].push_back(move);
    }
  }
  // std::cout << "Catalogued " << m_book.size() << " positions into opening
  // book"
  //           << std::endl;
}

std::vector<move_t> OpeningBook::query_all(const Board &board) const {
  const auto it = m_book.find(board.hash());
  if (it == m_book.end())
    return std::vector<move_t>();
  return it->second;
}

move_t OpeningBook::query(const Board &board, const size_t min_moves) const {
  const auto moves = query_all(board);
  if (moves.size() < min_moves)
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
