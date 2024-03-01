#ifndef __SVC_INTERNAL_H__
#define __SVC_INTERNAL_H__

#include "list.h"
#include "types.h"

/*
 * event.c
 */
typedef void (*event_handler_t)(int fd, int events, void* data);

typedef struct
{
  event_handler_t handler;
  int fd;
  void* data;
  struct list_head link;
} event_data_t;

extern void
init_event_loop(void);

extern void
run_event_loop(void);

extern void
close_event_loop(void);

extern int
event_add(int fd, int evtf, event_handler_t handler, void* data);

extern int
event_modify(int fd, int evtf);

extern void
event_del(int fd);

/*
 * contorl.c
 */
extern void
init_cntl_ipc(port_t cntl_port);

extern void
close_cntl_ipc(void);

#endif // !__SVC_INTERNAL_H__
