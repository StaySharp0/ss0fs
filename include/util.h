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

#endif /* __UTIL_H__ */
