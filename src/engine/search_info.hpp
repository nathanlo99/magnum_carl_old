
#pragma once

#include "timeit.hpp"
#include <chrono>

struct SearchInfo {
  // We check whether the search should stop, once every 'refresh_frequency'
  // nodes in our search. This should be small enough so we don't waste time
  // finding the current time every node, but large enough that it does not lead
  // to a significant lag in responding to GUI commands
  static const int refresh_frequency = 1 << 16;

  using time_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
  time_t start_time;                   // The start time, as a time point
  float seconds_to_search = 1000000.0; // Number of seconds to search

  int depth = 10000;   // Maximum depth to search
  int time_set = true; // Whether a maximum time is set

  int moves_to_go = 0;   // Moves to go until the next time control
  bool infinite = false; // True if we are searching infinitely

  long nodes = 0; // Number of nodes searched so far

  volatile bool has_quit = false;   // Received a quit interrupt
  volatile bool is_stopped = false; // Stop for any reason

  SearchInfo() : start_time(now()) {}
};
