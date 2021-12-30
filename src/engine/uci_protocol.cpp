
#include "uci_protocol.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "move.hpp"
#include "search_info.hpp"
#include "transposition_table.hpp"
#include "util.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

std::vector<SearchThread> search_threads;
std::mutex search_threads_mutex;

SearchThread::SearchThread(const Board &board, const float seconds_to_search,
                           const int depth, const bool infinite,
                           const bool send_info)
    : m_board(board), m_info(std::make_unique<SearchInfo>(
                          seconds_to_search, depth, infinite, send_info)) {
  m_thread = std::thread([&]() {
    search(*m_info, m_board);
    std::thread(remove_thread, m_thread.get_id()).detach();
  });
}

void remove_thread(const std::thread::id id) {
  std::lock_guard<std::mutex> guard(search_threads_mutex);
  const auto it =
      std::find_if(search_threads.begin(), search_threads.end(),
                   [=](SearchThread &t) { return t.m_thread.get_id() == id; });
  if (it != search_threads.end()) {
    it->m_thread.detach();
    search_threads.erase(it);
  }
}

void ponder(const Board &board) {
  std::lock_guard<std::mutex> guard(search_threads_mutex);
  search_threads.emplace_back(board, 0, 100, true, false);
}

void stop_all() {
  std::lock_guard<std::mutex> guard(search_threads_mutex);
  for (auto &thread : search_threads)
    thread.join();
  search_threads.clear();
}

namespace UCIProtocol {

void send_info(const std::string &str) {
  std::cout << "info string " << str << std::endl;
}

void send_identity() {
  std::cout << "id name magnum_carl" << std::endl;
  std::cout << "id author nathanlo99" << std::endl;
  std::cout << "uciok" << std::endl;
}

// go depth 6 wtime 180000 time 180000 binc 1000 winc 1000 movetime 1000
// movestogo 40
void process_go_command(const std::vector<std::string> &tokens,
                        const Board &board) {
  int depth = 100;
  int moves_to_go = 30;
  int remaining_time = -1;
  int increment = 0;
  bool infinite = true;
  int move_time = -1;
  const int side = board.m_side_to_move;

  size_t token_idx = 1;
  while (token_idx < tokens.size()) {
    const std::string token = tokens[token_idx];
    const std::string next_token =
        (token_idx + 1 < tokens.size()) ? tokens[token_idx + 1] : "";
    if (token == "infinite") {
      ;
    } else if (token == "wtime") {
      if (side == WHITE) {
        remaining_time = std::stoi(next_token);
      }
      token_idx++;
    } else if (token == "btime") {
      if (side == BLACK) {
        remaining_time = std::stoi(next_token);
      }
      token_idx++;
    } else if (token == "winc") {
      if (side == WHITE) {
        increment = std::stoi(next_token);
      }
      token_idx++;
    } else if (token == "binc") {
      if (side == BLACK) {
        increment = std::stoi(next_token);
      }
      token_idx++;
    } else if (token == "movestogo") {
      moves_to_go = std::stoi(next_token);
      token_idx++;
    } else if (token == "movetime") {
      move_time = std::stoi(next_token);
      token_idx++;
    } else if (token == "depth") {
      depth = std::stoi(next_token);
      token_idx++;
    } else {
      // std::cout << "error: unrecognized command/option " << token <<
      // std::endl;
    }
    token_idx++;
  }

  std::stringstream ss;
  ss << "info string [remaining time: " << remaining_time
     << ", increment: " << increment << ", movestogo: " << moves_to_go << "]";
  send_info(ss.str());

  std::lock_guard<std::mutex> guard(search_threads_mutex);
  if (move_time != -1) {
    // If the go command provides a search time, just run with that
    search_threads.emplace_back(board, move_time / 1000.0, depth, false, true);
  } else if (remaining_time != -1) {
    const int move_time_in_ms = (remaining_time - 5000) / moves_to_go;
    float seconds_to_search = (move_time_in_ms + increment / 2) / 1000.0;
    seconds_to_search = std::max(0.05f, seconds_to_search - 0.03f);
    search_threads.emplace_back(board, seconds_to_search, depth, false, true);
  } else {
    search_threads.emplace_back(board, 0, depth, true, true);
  }
}

// position fen
// position startpos
// ... moves e2e4 e7e5 ...
Board parse_position_command(const std::string &line) {
  // Parse the position first
  const size_t fen_idx = line.find("fen");
  Board board; // Initialized to the start position by default
  if (fen_idx != std::string::npos) {
    board = Board(line.substr(fen_idx + 4));
  }

  const size_t moves_idx = line.find("moves");
  if (moves_idx != std::string::npos) {
    // TODO: Remove the copy by using string_view's
    const std::string move_str = line.substr(moves_idx + 6);
    const std::vector<std::string> tokens = split(move_str, " ");
    for (const std::string &move_str : tokens) {
      const move_t move = parse_move(board, move_str);
      if (move == 0)
        continue;
      board.make_move(move);
    }
  }
  send_info(board.fen());
  return board;
}

void start_loop() {
  std::cin.setf(std::ios::unitbuf);
  std::cout.setf(std::ios::unitbuf);

  Board board;
  transposition_table.clear();
  bool quit = false;

  std::string line;
  while (!quit) {
    if (!std::getline(std::cin, line) || line[0] == '\n')
      continue;
    send_info("Received command: [" + line + "]");
    const std::vector<std::string> tokens = split(line, " ");
    if (tokens[0] == "isready") {
      std::cout << "readyok" << std::endl;

    } else if (tokens[0] == "position") {
      board = parse_position_command(line);

    } else if (tokens[0] == "ucinewgame") {
      board = Board();

    } else if (tokens[0] == "go") {
      stop_all();
      process_go_command(tokens, board); // Spawns a search thread
      std::lock_guard<std::mutex> guard(search_threads_mutex);
      send_info("There are " + std::to_string(search_threads.size()) +
                " search threads");
    } else if (tokens[0] == "quit") {
      stop_all();
      break;

    } else if (tokens[0] == "uci") {
      send_identity();
    } else if (tokens[0] == "stop") {
      stop_all();
    } else {
      // std::cout << "unrecognized command/option " << tokens[0] << " in line "
      //           << std::endl;
      // std::cout << line << std::endl;
    }
  }
}

}; // namespace UCIProtocol
