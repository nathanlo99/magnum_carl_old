
#pragma once

#include "assert.hpp"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

// Adding a very thin wrapper on a hashmap for performance counting in case I
// want to make this concurrent at some point
class PerfCounter {
  std::map<std::string, int> m_counters;

public:
  void clear() noexcept { m_counters.clear(); }
  void reset(const std::string &counter_name) noexcept {
    m_counters[counter_name] = 0;
  }
  void increment(const std::string &counter_name) noexcept {
    // m_counters[counter_name]++;
  }
  int get_value(const std::string &counter_name) noexcept {
    return m_counters[counter_name];
  }
  void dump(const std::string &pattern = "") const noexcept {
    std::cout << "------------- PERF COUNTER DUMP --------------" << std::endl;
    for (const auto &[counter, value] : m_counters) {
      if (counter.find(pattern) == 0) {
        std::cout << counter << ": " << value << std::endl;
      }
    }
    std::cout << "----------------------------------------------" << std::endl;
  }
};

extern PerfCounter perf_counter;
