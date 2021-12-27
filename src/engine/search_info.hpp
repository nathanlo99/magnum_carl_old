
#pragma once

#include "timeit.hpp"
#include <atomic>
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
  int depth = 100;                     // Maximum depth to search
  bool infinite = false;               // True if we are searching infinitely
  bool send_info = false;

  long nodes = 0; // Number of nodes searched so far

  std::atomic<bool> has_quit = false; // Received a quit interrupt from UCI
  bool is_stopped = false;            // Stopped search for any reason

  SearchInfo() : start_time(now()) {}
  SearchInfo(const float seconds_to_search, const int depth,
             const bool infinite, const bool send_info = true)
      : start_time(now()), seconds_to_search(seconds_to_search), depth(depth),
        infinite(infinite), send_info(send_info), has_quit(false),
        is_stopped(false) {}
  SearchInfo(const SearchInfo &other)
      : SearchInfo(other.seconds_to_search, other.depth, other.infinite) {}
};
