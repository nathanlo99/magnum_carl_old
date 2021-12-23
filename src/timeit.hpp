
#pragma once

#include <chrono>
#include <utility>

inline std::chrono::time_point<std::chrono::high_resolution_clock> now() {
  return std::chrono::high_resolution_clock::now();
}

inline float seconds_since(
    const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
  const auto finish = std::chrono::high_resolution_clock::now();
  const auto ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start)
          .count();
  return ns / 1e9;
}

template <typename Func, typename... Args>
size_t timeit(Func f, Args &&...args) {
  const auto start = now();
  f(std::forward<Args>(args)...);
  const auto finish = now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start)
      .count();
}
