#ifndef _SS0ADM_H
#define _SS0ADM_H

#include <stdint.h>

#define ENV_IPC_SOCKET "SS0_IPC_SOCKET"
#define SS0_IPC_DIR "/var/run/ss0d"
#define SS0_IPC_NAMESPACE SS0_IPC_DIR "/socket"

enum cli_op_e
{
  OP_MIN = 0,
  OP_STOP,
  OP_MAX = UINT16_MAX
};
typedef uint16_t cli_op_t;

typedef struct cli_req_s
{
  cli_op_t op;
} cli_req_t;

extern int
connect_ctl_ipc(void);

#endif
