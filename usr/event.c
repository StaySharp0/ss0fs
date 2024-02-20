#include <errno.h>

#include <sys/epoll.h>

#include <list.h>
#include <types.h>

#include "event.h"
#include "log.h"
#include "util.h"

static int ep_fd = 0;
static bool_t need_refresh = false;
static LIST_HEAD(evt_list);

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
  int nr_evt, i;
  struct epoll_event evts[1024];
  event_data_t* evt;

  extern bool_t is_active;

  while (is_active) {
    if ((nr_evt = epoll_wait(ep_fd, evts, ARR_SIZE(evts), -1)) < 0) {
      if (errno != EINTR) {
        eprintf_errno("can't handle epoll error");
        exit(1);
      }
    }

    for (i = 0; i < nr_evt; i++) {
      evt = (event_data_t*)evts[i].data.ptr;
      evt->handler(evt->fd, evts[i].events, evt->data);

      if (need_refresh) {
        need_refresh = false;
        break;
      }
    }
  }
}

int
add_event(int fd, int evts, event_handler_t handler, void* data)
{
  struct epoll_event ep_evt;
  event_data_t* evt;
  int err;

  if (!(evt = ALLOC(sizeof(*evt))))
    return -ENOMEM;

  evt->fd = fd;
  evt->data = data;
  evt->handler = handler;

  ZEROING(ep_evt);
  ep_evt.events = evts;
  ep_evt.data.ptr = evt;

  if ((err = epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &ep_evt))) {
    eprintf_errno("failed to add event to epoll");
    free(evt);
  } else {
    list_add(&evt->list, &evt_list);
  }

  return err;
}

static event_data_t*
lookup_event(int fd)
{
  event_data_t* evt;

  list_for_each_entry(evt, &evt_list, list)
  {
    if (evt->fd == fd)
      return evt;
  }

  return NULL;
}

void
del_event(int fd)
{
  event_data_t* evt;

  if (!(evt = lookup_event(fd))) {
    eprintf("can't find event %d\n", fd);
    return;
  }

  if (epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
    eprintf_errno("failed to remove epoll event");

  list_del(&evt->list);
  free(evt);

  need_refresh = true;
}