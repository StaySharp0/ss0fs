#ifndef __UTIL_H__
#define __UTIL_H__

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#define SIZE_ARR(x) (sizeof(x) / sizeof((x)[0]))
#define SIZE_STR_BUF(x) (sizeof(x) - 1)

#define CALLOC(size, ptr)                                                      \
  ({                                                                           \
    if ((ptr = calloc(size, sizeof(*ptr))) == NULL)                            \
      eprintf("%m");                                                           \
    ptr;                                                                       \
  })
#define ZEROING(var) memset(&var, 0, sizeof(var))

#define STR2NUM(str, val)                                                      \
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

extern const char* basename;

typedef void (*opt_handler_t)(int opt, char* value, void* data);
void
parse_opt(int argc,
          char** argv,
          struct option* l_opts,
          char* s_opts,
          opt_handler_t handler,
          void* data);

#define INVAL_OPT(opt, value, usage)                                           \
  ({                                                                           \
    if (errno != EINVAL)                                                       \
      eprintf_errno("invalid option value '-%c': '%s'", opt, value);           \
    else                                                                       \
      eprintf("invalid option value '-%c': '%s'", opt, value);                 \
    usage(errno);                                                              \
  })

#endif // !__UTIL_H__
