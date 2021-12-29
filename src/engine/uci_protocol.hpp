
#pragma once

#include "board.hpp"
#include "evaluate.hpp"
#include "search_info.hpp"
#include "util.hpp"

#include <string>
#include <thread>
#include <vector>

struct SearchThread {
  Board m_board;
  std::unique_ptr<SearchInfo> m_info;
  std::thread m_thread;

  SearchThread(const Board &board, const float seconds_to_search,
               const int depth, const bool infinite, const bool send_info);

  inline void stop() {
    if (m_info)
      m_info->has_quit.store(true);
  }
  inline void join() {
    stop();
    m_thread.join();
  }
};

extern std::vector<SearchThread> search_threads;
extern std::mutex search_threads_mutex;

void remove_thread(const std::thread::id id);

namespace UCIProtocol {

void send_info(const std::string &str);
void send_identity();
void process_go_command(const std::vector<std::string> &tokens,
                        const Board &board);
Board parse_position_command(const std::string &line);
void start_loop();

}; // namespace UCIProtocol
