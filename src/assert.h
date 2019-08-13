
#ifndef ASSERT_H
#define ASSERT_H

#include <cstdio>
#include <cstdlib>

#if defined(DEBUG)
#define ASSERT(expr) \
  do { if (!(expr)) { printf("[ERROR] Test failed: %s (%s:%d)\n", #expr, __FILE__, __LINE__); exit(1); } } while (0)
#else
#define ASSERT(expr) do { if ((!(expr))) __builtin_unreachable(); } while (0)
#endif

#endif /* end of include guard: ASSERT_H */
