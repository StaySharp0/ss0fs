#ifndef _UTIL_H
#define _UTIL_H

#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_ROOT_USER (getuid() == 0)

#define ARR_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define STR_SIZE(x) (sizeof(x) - 1)

#define CALLOC(size)                                                           \
  ({                                                                           \
    void* ptr;                                                                 \
    if ((ptr = calloc(1, size)) == NULL)                                       \
      eprintf("%m");                                                           \
    ptr;                                                                       \
  })
#define ZEROING(var) memset(&var, 0, sizeof(var))

#define ATON(str, val)                                                         \
  ({                                                                           \
    errno = 0;                                                                 \
    char* ptr;                                                                 \
    unsigned long long ull_val;                                                \
    ull_val = strtoull(str, &ptr, 0);                                          \
    val = (typeof(val))ull_val;                                                \
    if (val != ull_val)                                                        \
      errno = ERANGE;                                                          \
    errno;                                                                     \
  })
#define ATON_GE(str, val, minv)                                                \
  ({                                                                           \
    errno = ATON(str, val);                                                    \
    if (!errno && (val < minv))                                                \
      errno = ERANGE;                                                          \
    errno;                                                                     \
  })

#define INVAL_OPT(ch, ipt, usage)                                              \
  ({                                                                           \
    if (errno != EINVAL)                                                       \
      eprintf_errno("invalid argument value '-%c': '%s'", ch, ipt);            \
    else                                                                       \
      eprintf("invalid argument value '-%c': '%s'", ch, ipt);                  \
    usage(errno);                                                              \
  })

#define NOP                                                                    \
  do {                                                                         \
  } while (false);

typedef void (*opt_handler_t)(int opt, char* value, void* data);
void
parse_opt(int argc,
          char** argv,
          struct option* l_opts,
          char* s_opts,
          opt_handler_t handler,
          void* data);

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
