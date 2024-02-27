#ifndef __SVC_H__
#define __SVC_H__

#include <getopt.h>

#include "types.h"

extern const char* basename;

/*
 * Modes of server
 */
#define MODE_SVC_FG 0x00000001 /* foreground mode */

typedef void (*opt_handler_t)(int opt, char* value, void* data);
void
svc_parse_opt(int argc,
              char** argv,
              struct option* l_opts,
              char* s_opts,
              opt_handler_t handler,
              void* data);

void
svc_set_mode(flag_t flag);

void
svc_run(void);

#endif // !__SVC_H__
