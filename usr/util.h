#ifndef _UTIL_H
#define _UTIL_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_ROOT_USER (getuid() == 0)

#define ARR_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define STR_SIZE(x) (sizeof(x) - 1)

#define ALLOC(size)                                                            \
  ({                                                                           \
    void* ptr;                                                                 \
    if ((ptr = calloc(1, size)) == NULL)                                       \
      eprintf("%m\n");                                                         \
    ptr;                                                                       \
  })

#define ZEROING(var) memset(&var, 0, sizeof(var))

static inline void
version(void)
{
  printf(SS0D_VERSION "\n");
  exit(0);
}

void
set_signal(__sighandler_t handler);

int
set_non_blocking(int fd);

#endif
