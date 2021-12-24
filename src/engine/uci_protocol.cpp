
#include "board.hpp"
#include "evaluate.hpp"
#include "move.hpp"
#include "search_info.hpp"
#include "transposition_table.hpp"
#include "util.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

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
SearchInfo process_go_command(const std::vector<std::string> &tokens,
                              const Board &board) {
  SearchInfo info;
  int depth = 10000;
  int moves_to_go = 30;
  int move_time = -1;
  int remaining_time = -1;
  int increment = 0;
  info.time_set = false;
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

  if (move_time != -1) {
    remaining_time = move_time;
    moves_to_go = 1;
  }

  info.depth = depth;

  if (remaining_time != -1) {
    info.time_set = true;
    const int move_time_in_ms = remaining_time / moves_to_go;
    info.seconds_to_search = (move_time_in_ms + increment) / 1000.0;
  }

  // std::cout << "time: " << info.seconds_to_search << ", depth: " <<
  // info.depth << ", timeset: " << info.time_set << std::endl;

  info.start_time = now();
  return info;
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

  // send_identity();

  Board board;
  SearchInfo info;
  transposition_table.clear();
  std::vector<std::thread> search_threads;

  std::string line;
  while (!info.has_quit) {
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
      send_info("Waiting for search threads to complete...");
      for (auto &thread : search_threads)
        thread.join();
      search_threads.clear();
      send_info("All search threads are complete");
      info = process_go_command(tokens, board);
      search_threads.push_back(
          std::thread(get_best_move, std::ref(info), board));
      send_info("There are " + std::to_string(search_threads.size()) +
                " search threads");
    } else if (tokens[0] == "quit") {
      info.has_quit = true;
      for (auto &thread : search_threads)
        thread.join();
      search_threads.clear();
    } else if (tokens[0] == "uci") {
      send_identity();
    } else if (tokens[0] == "stop") {
      info.is_stopped = true;
      for (auto &thread : search_threads)
        thread.join();
      search_threads.clear();
    } else {
      // std::cout << "unrecognized command/option " << tokens[0] << " in line "
      //           << std::endl;
      // std::cout << line << std::endl;
    }
  }
}

}; // namespace UCIProtocol
