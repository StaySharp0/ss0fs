#ifndef __SVC_H__
#define __SVC_H__

#include "types.h"

/*
 * Modes of server
 */
#define MODE_SVC_FG 0x00000001 /* foreground mode */

void
svc_set_mode(flag_t flag);

void
svc_run(void);

#endif // !__SVC_H__
