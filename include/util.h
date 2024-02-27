#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <string.h>

#include "log.h"

#define SIZE_ARR(x) (sizeof(x) / sizeof((x)[0]))
#define SIZE_STR_BUF(x) (sizeof(x) - 1)

#define CALLOC(size)                                                           \
  ({                                                                           \
    void* ptr;                                                                 \
    if ((ptr = calloc(1, size)) == NULL)                                       \
      eprintf("%m");                                                           \
    ptr;                                                                       \
  })
#define ZEROING(var) memset(&var, 0, sizeof(var))

#ifndef __GNUC__
#define snprintf_nowarn snprintf
#else
#define snprintf_nowarn(...)                                                   \
  __extension__({                                                              \
    _Pragma("GCC diagnostic push");                                            \
    _Pragma("GCC diagnostic ignored \"-Wformat-truncation\"");                 \
    const int _snprintf_nowarn = snprintf(__VA_ARGS__);                        \
    _Pragma("GCC diagnostic pop");                                             \
    _snprintf_nowarn;                                                          \
  })
#endif

#endif /* __UTIL_H__ */
