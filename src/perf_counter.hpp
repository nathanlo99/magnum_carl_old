
#pragma once

#include "assert.hpp"

#include <map>
#include <string>

// Adding a very thin wrapper on a hashmap for performance counting in case I
// want to make this concurrent at some point
class PerfCounter {
  std::map<std::string, int> m_counters;

public:
  void reset_all() { m_counters.clear(); }
  void reset(const std::string &counter_name) { m_counters[counter_name] = 0; }
  void increment(const std::string &counter_name) {
    ASSERT(m_counters.count(counter_name) > 0);
    m_counters[counter_name]++;
  }
  int get_value(const std::string &counter_name) {
    ASSERT(m_counters.count(counter_name) > 0);
    return m_counters[counter_name];
  }
};

extern PerfCounter perf_counter;
