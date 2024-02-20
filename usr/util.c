/*
 * common utility functions
 *
 * Copyright (C) 2024 StaySharp0 Yongjun Kim <staysharp0@gmail.com>
 */
#include <fcntl.h>
#include <unistd.h>

#include "log.h"
#include "util.h"

void
set_signal(__sighandler_t handler)
{
  struct sigaction sa_old;
  struct sigaction sa_new;

  sa_new.sa_handler = handler;
  sigemptyset(&sa_new.sa_mask);

  sa_new.sa_flags = 0;
  sigaction(SIGPIPE, &sa_new, &sa_old);
  sigaction(SIGINT, &sa_new, &sa_old);
  sigaction(SIGTERM, &sa_new, &sa_old);
}

int
set_non_blocking(int fd)
{
  int err;

  if ((err = fcntl(fd, F_GETFL)) < 0) {
    eprintf_errno("unable to get fd flags\n");
  } else {
    if ((err = fcntl(fd, F_SETFL, err | O_NONBLOCK)) < 0)
      eprintf_errno("unable to set fd flags");
    else
      err = 0;
  }

  return err;
}