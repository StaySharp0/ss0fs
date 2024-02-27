#ifndef __SVC_INTERNAL_H__
#define __SVC_INTERNAL_H__

/* event.c */
extern void
init_event_loop(void);

extern void
run_event_loop(void);

extern void
close_event_loop(void);

/* contorl.c */
extern void
init_ctl_ipc(void);

extern void
close_ctl_ipc(void);

#endif // !__SVC_INTERNAL_H__
