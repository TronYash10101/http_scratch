#ifndef TEST_COLORS
#define TEST_COLORS

#define CLR_RESET "\033[0m"
#define CLR_RED "\033[31m"
#define CLR_GREEN "\033[32m"
#define CLR_YELLOW "\033[33m"

#define PASS(fmt, ...)                                                         \
  do {                                                                         \
    fprintf(stdout, CLR_GREEN "[PASS] " fmt CLR_RESET "\n", ##__VA_ARGS__);    \
    fflush(stdout);                                                            \
  } while (0)

#define FAIL(fmt, ...)                                                         \
  do {                                                                         \
    fprintf(stdout, CLR_RED "[FAIL] " fmt CLR_RESET "\n", ##__VA_ARGS__);      \
    fflush(stdout);                                                            \
  } while (0)
#endif
