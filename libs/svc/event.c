#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "log.h"
#include "types.h"
#include "util.h"

static int epfd = 0;

void
init_event_loop(void)
{
  if ((epfd = epoll_create(4096)) < 0) {
    eprintf_errno("can't create epoll");
    exit(1);
  }
}

void
run_event_loop(void)
{
  extern bool_t is_active;
  int nr_evt, i;
  struct epoll_event evts[1024];

retry:
  if ((nr_evt = epoll_wait(epfd, evts, SIZE_ARR(evts), -1))) {
    if (errno != EINTR) {
      eprintf_errno("can't handle error in epoll");
      exit(1);
    }

    for (i = 0; i < nr_evt; i++) {
      printf("[%d/%d] epoll event\n", i, nr_evt);
    }
  }

  if (is_active)
    goto retry;
}

void
exit_event_loop(void)
{
  if (epfd > 0)
    close(epfd);
}
