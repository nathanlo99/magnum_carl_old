
#ifndef ASSERT_H
#define ASSERT_H

#include <cstdio>
#include <cstdlib>

#if defined(INFO)
#define INFO(...) \
  do { \
    printf("[INFO] "); \
    printf(__VA_ARGS__); \
    printf(" (%s:%s:%d)\n", __FILE__, __func__, __LINE__); \
  } while (0)
#else
#define INFO(...) do {} while (0)
#endif

#define WARN(s) \
  printf("[WARN] %s (%s:%s:%d)\n", \
  s, __FILE__, __func__, __LINE__)
#define ERROR(s) \
  printf("[ERROR] %s (%s:%s:%d)\n", \
  s, __FILE__, __func__, __LINE__)

#if defined(DEBUG)
  #define WASSERT(expr) \
    do { \
      if (!(expr)) { WARN("Test failed: " #expr); } \
    } while (0)
  #define WASSERT_MSG(expr, ...) \
    do { \
      if (!(expr)) { \
        WARN("Test failed: " #expr); \
        printf(__VA_ARGS__); \
        printf("\n"); \
      } \
    } while (0)
  #define WASSERT_IF(cond, expr) WASSERT(!(cond) || (expr));
  #define WASSERT_IF_MSG(cond, expr, ...) \
    WASSERT_MSG(!(cond) || (expr), __VA_ARGS__);
  #define ASSERT(expr) \
    do { \
      if (!(expr)) { ERROR("Test failed: " #expr); exit(1); } \
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
  #define ASSERT_IF(cond, expr) ASSERT(!(cond) || (expr));
  #define ASSERT_IF_MSG(cond, expr, ...) \
    ASSERT_MSG(!(cond) || (expr), __VA_ARGS__);
#elif !defined(INVISIBLE_ASSERTS)
  #define WASSERT(expr) do { if ((!(expr))) __builtin_unreachable(); } while (0)
  #define WASSERT_MSG(expr, ...) ASSERT(expr)
  #define WASSERT_IF(cond, expr) WASSERT(!(cond) || (expr));
  #define WASSERT_IF_MSG(cond, expr, ...) WASSERT(!(cond) || (expr));
  #define ASSERT(expr) do { if ((!(expr))) __builtin_unreachable(); } while (0)
  #define ASSERT_MSG(expr, ...) ASSERT(expr)
  #define ASSERT_IF(cond, expr) ASSERT(!(cond) || (expr));
  #define ASSERT_IF_MSG(cond, expr, ...) ASSERT(!(cond) || (expr));
#else
  #define WASSERT(expr) do {} while (0)
  #define WASSERT_MSG(expr, ...) do {} while (0)
  #define WASSERT_IF(cond, expr) do {} while (0)
  #define WASSERT_IF_MSG(cond, expr, ...) do {} while (0)
  #define ASSERT(expr) do {} while (0)
  #define ASSERT_MSG(expr, ...) do {} while (0)
  #define ASSERT_IF(cond, expr) do {} while (0)
  #define ASSERT_IF_MSG(cond, expr, ...) do {} while (0)
#endif

#endif /* end of include guard: ASSERT_H */
