/*
 * Logging and Tracing Library
 *
 * Copyright (C) 2024 StaySharp0 Yongjun Kim <staysharp0@gmail.com>
 */

#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>

#include <types.h>

extern const char program_name[];
extern bool_t is_debug;

#define eprintf(fmt, args...)                                                  \
  do {                                                                         \
    fprintf(stderr, "%s: " fmt "\n", program_name, ##args);                    \
  } while (0)
#define eprintf_errno(fmt, args...)                                            \
  do {                                                                         \
    fprintf(stderr, "%s: " fmt " - %m\n", program_name, ##args);               \
  } while (0)
#define dprintf(fmt, args...)                                                  \
  do {                                                                         \
    if (is_debug)                                                              \
      fprintf(stderr,                                                          \
              "[%*.*s:%*.*s](%4d) " fmt,                                       \
              8,                                                               \
              8,                                                               \
              __FILE__,                                                        \
              10,                                                              \
              10,                                                              \
              __FUNCTION__,                                                    \
              __LINE__,                                                        \
              ##args);                                                         \
  } while (0)

#endif
