/*
 * SS0FSD control functions
 *
 * Copyright (C) 2024 StaySharp0 Yongjun Kim <staysharp0@gmail.com>
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "event.h"
#include "log.h"
#include "ss0cli.h"
#include "ss0d.h"
#include "util.h"

static int ctl_fd = 0;

extern short control_port;

enum ctl_state_e
{
  CTL_HDR_RECV = 0,
  CTL_PDU_RECV,
  CTL_HDR_SEND,
  CTL_PDU_SEND,
};
typedef uint8_t ctl_state_t;

typedef struct ctl_s
{
  ctl_state_t state;
  int progress;
  cli_req_t req;
  cli_res_t res;
} ctl_t;

static ctl_t*
ctl_alloc(void)
{
  ctl_t* ctl;

  if (!(ctl = CALLOC(sizeof(*ctl)))) {
    eprintf("can't allocate control");
    return NULL;
  }

  ctl->state = CTL_HDR_RECV;

  dprintf("control: %p\n", ctl);
  return ctl;
}

static void
ctl_free(ctl_t* ctl)
{
  dprintf("control: %p\n", ctl);

  if (ctl)
    free(ctl);
}

static int
do_op_stop(ctl_t* ctl)
{
  NOP;
  return 0;
}

static int
ctl_execute(ctl_t* ctl)
{
  cli_req_t* req = &ctl->req;

  switch (req->op) {
#define OP(name, key)                                                          \
  case OP_##name:                                                              \
    do_op_##key(ctl);                                                          \
    break;
#include "op.list"
    default:
      break;
  }
  return 0;
}

static int
ctl_submit(int fd, ctl_t* ctl)
{
  int rtn = -1;

  ctl->state = CTL_HDR_SEND;
  ctl->progress = 0;
  ctl->res.status = ctl_execute(ctl);

  if ((rtn = modify_event(fd, EPOLLOUT)))
    eprintf("failed to modify event out\n");

  return rtn;
}

static void
ctl_recv_send_handler(int fd, int evts, void* data)
{
  ctl_t* ctl = data;
  cli_req_t* req = &ctl->req;
  cli_res_t* res = &ctl->res;
  int ret, remain;
  char* buf;

  switch (ctl->state) {
    case CTL_HDR_RECV:
      buf = (char*)req + ctl->progress;
      remain = sizeof(*req) - ctl->progress;

      if ((ret = read(fd, buf, remain)) < 0) {
        if (errno != EAGAIN)
          goto out;
      }

      if ((ctl->progress += ret) == sizeof(*req)) {
        ctl_submit(fd, ctl);
      }
      break;
    case CTL_HDR_SEND:
      buf = (char*)res + ctl->progress;
      remain = sizeof(*res) - ctl->progress;

      if ((ret = write(fd, buf, remain)) < 0) {
        if (errno != EAGAIN)
          goto out;
      }
      break;
  }

  return;
out:
  if (req->op == OP_STOP)
    is_active = 0;

  del_event(fd);
  close(fd);
  ctl_free(ctl);
}

static void
ctl_handler(int accept_fd, int evts, void* data)
{
  int fd = 0;
  struct sockaddr addr;
  struct ucred cred;
  socklen_t len;
  ctl_t* ptr;

  /* ipc accept */
  len = sizeof(addr);
  if ((fd = accept(accept_fd, (struct sockaddr*)&addr, &len)) < 0) {
    eprintf_errno("can't accept a new connection");
    return;
  }

  /* ipc permission */
  len = sizeof(cred);
  if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, (void*)&cred, &len) < 0) {
    eprintf_errno("can't get sockopt");
    goto out;
  }

  if (cred.uid != getuid() || cred.gid != getgid()) {
    errno = EPERM;
    eprintf_errno("can't accept a new connection");
    goto out;
  }

  /* non-blocking socket */
  if (set_non_blocking(fd) < 0) {
    eprintf("failed to set a socket non-blocking");
    goto out;
  }

  /* fd handler */
  if (!(ptr = ctl_alloc()))
    goto out;

  if (add_event(fd, EPOLLIN, ctl_recv_send_handler, ptr)) {
    eprintf("failed to add a socket to epoll %d", fd);
    ctl_free(ptr);
    goto out;
  }

  return;
out:
  if (fd > 0)
    close(fd);
}

void
init_ctl_ipc(void)
{
  int fd = 0;
  const char* path = NULL;
  struct stat st;
  char ctl_path[256];
  struct sockaddr_un addr;

  if (!(path = getenv(ENV_IPC_SOCKET))) {
    path = SS0_IPC_NAMESPACE;

    if (stat(SS0_IPC_DIR, &st) < 0)
      mkdir(SS0_IPC_DIR, 0755);
  }

  snprintf(ctl_path, STR_SIZE(ctl_path), "%s.%d", path, control_port);
  if (strlen(ctl_path) > STR_SIZE(addr.sun_path)) {
    eprintf("control path too long: %s", ctl_path);
    goto out;
  }

  if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
    eprintf_errno("can't open a socket");
    goto out;
  }

  unlink(ctl_path);
  ZEROING(addr);
  addr.sun_family = AF_LOCAL;
  strcpy(addr.sun_path, ctl_path);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    eprintf_errno("can't bind a socket");
    goto out;
  }

  if (listen(fd, 32) < 0) {
    eprintf_errno("can't listen a socket");
    goto out;
  }

  if (add_event(fd, EPOLLIN, ctl_handler, NULL))
    goto out;

  ctl_fd = fd;

  return;
out:
  if (fd > 0)
    close(fd);

  exit(1);
}

void
close_ctl_ipc(void)
{
  del_event(ctl_fd);

  if (ctl_fd > 0)
    close(ctl_fd);
}
