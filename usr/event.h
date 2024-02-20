#ifndef _EVENT_H
#define _EVENT_H

#include <list.h>

typedef void (*event_handler_t)(int fd, int evts, void* data);

typedef struct event_data_s
{
  struct list_head list;

  int fd;
  void* data;
  event_handler_t handler;
} event_data_t;

void
init_event_loop(void);

void
run_event_loop(void);

int
add_event(int fd, int evts, event_handler_t handler, void* data);

void
del_event(int fd);

#endif /* _EVENT_H */
