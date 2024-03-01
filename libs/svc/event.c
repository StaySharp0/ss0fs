#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "list.h"
#include "log.h"
#include "svc_internal.h"
#include "types.h"
#include "util.h"

static int ep_fd = 0;
static bool_t need_refresh = false;
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
  event_data_t* ed;

retry:
  if ((nr_evt = epoll_wait(ep_fd, evts, SIZE_ARR(evts), -1)) < 0) {
    if (errno != EINTR) {
      eprintf_errno("can't handle error in epoll");
      exit(1);
    }
  }

  for (i = 0; i < nr_evt; i++) {
    ed = evts[i].data.ptr;
    ed->handler(ed->fd, evts[i].events, ed->data);

    if (need_refresh) {
      need_refresh = false;
      goto retry;
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

static event_data_t*
lookup(int fd)
{
  event_data_t* ed;

  list_for_each_entry(ed, &eds, link)
  {
    if (ed->fd == fd)
      return ed;
  }

  return NULL;
}

int
event_modify(int fd, int evtf)
{
  int ret;
  struct epoll_event ep_evt;
  event_data_t* ed;

  if (!(ed = lookup(fd))) {
    eprintf("can't find event: %d", fd);
    return -EINVAL;
  }

  ZEROING(ep_evt);
  ep_evt.events = evtf;
  ep_evt.data.ptr = ed;

  return epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &ep_evt);
}

void
event_del(int fd)
{
  event_data_t* ed;

  if (!(ed = lookup(fd))) {
    eprintf("can't find event: %d", fd);
    return;
  }

  if (epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
    eprintf_errno("failed to delete event: %d", fd);

  list_del(&ed->link);
  free(ed);

  need_refresh = true;
}
