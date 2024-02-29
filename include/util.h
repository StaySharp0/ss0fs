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

extern const char* basename;

typedef void (*opt_handler_t)(int opt, char* value, void* data);
void
parse_opt(int argc,
          char** argv,
          struct option* l_opts,
          char* s_opts,
          opt_handler_t handler,
          void* data);

#endif // !__UTIL_H__
