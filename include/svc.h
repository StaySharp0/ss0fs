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
svc_run(port_t cntl_port);

/* control */
enum svc_type_e
{
  SVC_INVAL = 0,
  SVC_MASTER,
  SVC_STORAGE,
  SVC_MAX
};
typedef uint8_t svc_type_t;

#define SVC_T2STR(type)                                                        \
  (type == SVC_MASTER ? "ss0msd" : type == SVC_STORAGE ? "ss0ssd" : "")

enum mode_type_e
{
  MODE_INVAL = 0,
  MODE_SYSTEM,
  MODE_MAX
};
typedef uint8_t mode_type_t;

enum op_type_e
{
  OP_INVAL = 0,
  OP_DOWN,
  OP_MAX
};
typedef uint8_t op_type_t;

/* need 64-bits alignment */
typedef struct
{
  int len;
  svc_type_t svc;
  mode_type_t mode;
  op_type_t op;
  char filler[1];
} cntl_req_t;

/* need 64-bits alignment */
typedef struct
{
  int len;
  int status;
} cntl_res_t;

#endif // !__SVC_H__
