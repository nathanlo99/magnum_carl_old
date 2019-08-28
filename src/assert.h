
#ifndef ASSERT_H
#define ASSERT_H

#include <cstdio>
#include <cstdlib>

#if defined(DEBUG)
  #define ERROR(s) \
    printf("[ERROR] Test failed: %s (%s:%s:%d)\n", \
    s, __FILE__, __func__, __LINE__)
  #define ASSERT(expr) \
    do { \
      if (!(expr)) { ERROR(#expr); exit(1); } \
    } while (0)
  #define ASSERT_MSG(expr, ...) \
    do { \
      if (!(expr)) { \
        ERROR(#expr); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        exit(1); \
      } \
    } while (0)
  #define ASSERT_IF(cond, expr) \
    do { \
      if (!(cond) || !(expr)) { ERROR(#expr); exit(1); } \
    } while (0)
  #define ASSERT_IF_MSG(cond, expr, ...) \
    do { \
      if (!(cond) || !(expr)) { \
        ERROR(#expr); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        exit(1); \
      } \
    } while (0)
#elif !defined(INVISIBLE_ASSERTS)
  #define ASSERT(expr) \
    do { if ((!(expr))) __builtin_unreachable(); } while (0)
  #define ASSERT_MSG(expr, ...) ASSERT(expr)
  #define ASSERT_IF(cond, expr) ASSERT(!(expr) || (cond))
  #define ASSERT_IF_MSG(cond, expr, ...) ASSERT(!(expr) || (cond))
#else
  #define ASSERT(expr) do {} while (0)
  #define ASSERT_MSG(expr, ...) do {} while (0)
  #define ASSERT_IF(cond, expr) do {} while (0)
  #define ASSERT_IF_MSG(cond, expr, ...) do {} while (0)
#endif

#endif /* end of include guard: ASSERT_H */
