
#include <chrono>

template <typename Func, typename ... Args>
size_t timeit(Func f, Args&&... args) {
  const auto start = std::chrono::high_resolution_clock::now();
  f(std::forward<Args>(args)...);
  const auto finish = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
}
