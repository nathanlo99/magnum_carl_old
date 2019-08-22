
#ifndef ASSERT_H
#define ASSERT_H

#include <cstdio>
#include <cstdlib>

#if defined(DEBUG)
  #define ASSERT(expr) \
    do { \
      if (!(expr)) { \
        printf("[ERROR] Test failed: %s (%s:%s:%d)\n", \
          #expr, __FILE__, __func__, __LINE__); \
        exit(1); \
      } \
    } while (0)
  #define ASSERT_MSG(expr, ...) \
    do { \
      if (!(expr)) { \
        printf("[ERROR] Test failed: %s (%s:%s:%d)\n", \
          #expr, __FILE__, __func__, __LINE__); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        exit(1); \
      } \
    } while (0)
#elif !defined(INVISIBLE_ASSERTS)
  #define ASSERT(expr) \
    do { \
      if ((!(expr))) __builtin_unreachable(); \
    } while (0)
  #define ASSERT_MSG(expr, ...) ASSERT(expr)
#else
  #define ASSERT(expr) do {} while (0)
  #define ASSERT_MSG(expr, ...) do {} while (0)
#endif

#endif /* end of include guard: ASSERT_H */
