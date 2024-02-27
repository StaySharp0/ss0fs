#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

extern const char* basename;

#define eprintf(fmt, args...)                                                  \
  do {                                                                         \
    fprintf(stderr, "%s: " fmt "\n", basename, ##args);                        \
  } while (0)
#define eprintf_errno(fmt, args...)                                            \
  do {                                                                         \
    fprintf(stderr, "%s: " fmt " - %m\n", basename, ##args);                   \
  } while (0)

#define dprintf(fmt, args...)                                                  \
  do {                                                                         \
    fprintf(stderr,                                                            \
            "[%*.*s:%*.*s](%4d) " fmt "\n",                                    \
            8,                                                                 \
            8,                                                                 \
            __FILE__,                                                          \
            10,                                                                \
            10,                                                                \
            __FUNCTION__,                                                      \
            __LINE__,                                                          \
            ##args);                                                           \
  } while (0)

#endif // !__LOG_H__
