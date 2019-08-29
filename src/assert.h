
#ifndef ASSERT_H
#define ASSERT_H

#include <cstdio>
#include <cstdlib>

#define WARN(s) \
  printf("[WARN] %s (%s:%s:%d)\n", \
  s, __FILE__, __func__, __LINE__)
#define ERROR(s) \
  printf("[ERROR] %s (%s:%s:%d)\n", \
  s, __FILE__, __func__, __LINE__)

#if defined(DEBUG)
  #define ASSERT(expr) \
    do { \
      if (!(expr)) { ERROR(#expr); exit(1); } \
    } while (0)
  #define ASSERT_MSG(expr, ...) \
    do { \
      if (!(expr)) { \
        ERROR("Test failed: " #expr); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        exit(1); \
      } \
    } while (0)
  #define ASSERT_IF(cond, expr) \
    do { \
      if (cond && !(expr)) { ERROR("Test failed: " #expr); exit(1); } \
    } while (0)
  #define ASSERT_IF_MSG(cond, expr, ...) \
    do { \
      if (cond && !(expr)) { \
        ERROR("Test failed: " #expr); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        exit(1); \
      } \
    } while (0)
#elif !defined(INVISIBLE_ASSERTS)
  #define ASSERT(expr) do { if ((!(expr))) __builtin_unreachable(); } while (0)
  #define ASSERT_MSG(expr, ...) ASSERT(expr)
  #define ASSERT_IF(cond, expr) ASSERT(!(cond) || (expr));
  #define ASSERT_IF_MSG(cond, expr, ...) ASSERT(!(cond) || (expr));
#else
  #define ASSERT(expr) do {} while (0)
  #define ASSERT_MSG(expr, ...) do {} while (0)
  #define ASSERT_IF(cond, expr) do {} while (0)
  #define ASSERT_IF_MSG(cond, expr, ...) do {} while (0)
#endif

#endif /* end of include guard: ASSERT_H */
