
#pragma once

#include "assert.hpp"

#include <iostream>
#include <map>
#include <string>

// Adding a very thin wrapper on a hashmap for performance counting in case I
// want to make this concurrent at some point
class PerfCounter {
  std::map<std::string, int> m_counters;

public:
  void clear() { m_counters.clear(); }
  void reset(const std::string &counter_name) { m_counters[counter_name] = 0; }
  void increment(const std::string &counter_name) {
    m_counters[counter_name]++;
  }
  int get_value(const std::string &counter_name) {
    return m_counters[counter_name];
  }
  void dump(const std::string &pattern = "") const {
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
