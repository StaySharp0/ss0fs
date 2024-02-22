/*
 * SS0 filesystem command-line interface
 *
 * Copyright (C) 2024 StaySharp0 Yongjun Kim <staysharp0@gmail.com>
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <types.h>
#include <unistd.h>

#include <util.h>

#include "log.h"
#include "ss0cli.h"

const char program_name[] = SS0CLI;
bool_t is_debug = true;
short int control_port = 0;

static struct option l_opts[] = {
  { "op", required_argument, 0, 'o' },
  { "control-port", required_argument, 0, 'C' },
  { "version", no_argument, 0, 'V' },
  { "help", no_argument, 0, 'h' },
  { 0, 0, 0, 0 },
};
static char* s_opts = "VhC:o:";

static void
usage(int status)
{
  if (status) {
    fprintf(stderr, "Try `" SS0CLI " --help' for more information.\n");
    exit(EINVAL);
  }

  printf("SS0 filesystem command-line interface, version " SS0D_VERSION "\n\n"
         "Usage: " SS0CLI " [OPTION]\n"
         "-o, --op <operation>       <operate> ss0 daemon\n"
         "-C, --control-port <port>  use control port <port>\n"
         "-V, --version              print version and exit\n"
         "-h, --help                 display this help and exit\n");
  exit(0);
}

static cli_op_t
parse_op(char* str)
{
#define OP(name, key)                                                          \
  if (!strcmp(#key, str))                                                      \
    return OP_##name;
#include "op.list"

  eprintf("unknown operation: %s", str);
  exit(1);
}

static void
opt_handler(int opt, char* value, void* data)
{
  cli_req_t* req = (cli_req_t*)data;

  switch (opt) {
    case 'o':
      req->op = parse_op(value);
      break;
    case 'C':
      if (ATON_GE(value, control_port, 0))
        INVAL_OPT(opt, value, usage);
      break;
    case 'V':
      version();
    case 'h':
      usage(0);
    case '?':
      break;
    default:
      usage(1);
  }
}

int
connect_ctl_ipc(void)
{
  int fd = 0;
  struct sockaddr_un addr;
  const char* path = NULL;
  char ctl_path[256];

  if (!(path = getenv(ENV_IPC_SOCKET)))
    path = SS0_IPC_NAMESPACE;

  snprintf(ctl_path, STR_SIZE(ctl_path), "%s.%d", path, control_port);
  if (strlen(ctl_path) > STR_SIZE(addr.sun_path)) {
    eprintf("control path too long: %s", ctl_path);
    errno = EINVAL;
    goto out;
  }

  if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
    eprintf_errno("can't create a socket");
    goto out;
  }

  ZEROING(addr);
  addr.sun_family = AF_LOCAL;
  strcpy(addr.sun_path, ctl_path);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    goto out;

  return fd;
out:
  if (fd > 0)
    close(fd);

  return -1;
}

static void
await_daemon_stop(void)
{
  int fd;

  while (true) {
    fd = connect_ctl_ipc();

    if (errno == ECONNREFUSED)
      break;

    close(fd);
    usleep(100 * 1000);
  }
}
static int
recv_res(int fd, cli_req_t* req)
{
  int ret = -1;
  cli_res_t res;

retry:
  if ((ret = recv(fd, &res, sizeof(res), MSG_WAITALL)) < 0) {
    if (errno == EAGAIN)
      goto retry;
    else if (errno == EINTR)
      eprintf("interrupted by a signal");
    else
      eprintf_errno("can't get the response");

    return errno;
  } else if (ret == 0) {
    eprintf("ss0 daemon closed the socket\n");
    return 1;
  } else if (ret != sizeof(res)) {
    eprintf("a partial response\n");
    return 1;
  }

  if (req->op == OP_STOP) {
    await_daemon_stop();
  }

  return 0;
}

static int
send_req(cli_req_t* req)
{
  int fd = 0, ret = -1, rtn = 0;

  if ((fd = connect_ctl_ipc()) < 0)
    eprintf_errno("can't connect to ss0 daemon");

  ret = write(fd, req, sizeof(*req));
  if (ret < 0 || ret != sizeof(*req)) {
    eprintf_errno("failed to send request header to ss0 daemon");
    rtn = errno;
    goto out;
  }

  rtn = recv_res(fd, req);

out:
  if (fd > 0)
    close(fd);

  return rtn;
}

int
main(int argc, char** argv)
{
  cli_req_t req;

  ZEROING(req);
  parse_opt(argc, argv, l_opts, s_opts, opt_handler, &req);

  return send_req(&req);
}
