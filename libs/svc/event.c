#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "log.h"
#include "svc_internal.h"
#include "types.h"
#include "util.h"

static int ep_fd = 0;
static LIST_HEAD(eds);

void
init_event_loop(void)
{
  if ((ep_fd = epoll_create(4096)) < 0) {
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
  if ((nr_evt = epoll_wait(ep_fd, evts, SIZE_ARR(evts), -1))) {
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
close_event_loop(void)
{
  if (ep_fd > 0)
    close(ep_fd);
}

int
event_add(int fd, int evtf, event_handler_t handler, void* data)
{
  event_data_t* ed;
  struct epoll_event ep_evt;
  int ret;

  if (!(ed = CALLOC(1, ed)))
    return -ENOMEM;

  ed->fd = fd;
  ed->data = data;
  ed->handler = handler;

  ZEROING(ep_evt);
  ep_evt.events = evtf;
  ep_evt.data.ptr = ed;

  if ((ret = epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &ep_evt))) {
    eprintf_errno("can't add fd: %d", fd);
    free(ed);
  } else {
    list_add(&ed->link, &eds);
  }

  return ret;
}
