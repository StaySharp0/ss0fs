/*
 * SS0 filesystem command-line interface
 *
 * Copyright (C) 2024 StaySharp0 Yongjun Kim <staysharp0@gmail.com>
 */
#include <errno.h>
#include <getopt.h>
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

static short int control_port;
static struct option const l_opts[] = {
  { "control-port", required_argument, 0, 'C' },
  { "version", no_argument, 0, 'V' },
  { "help", no_argument, 0, 'h' },
  { 0, 0, 0, 0 },
};
static char* s_opts = "VhC:";

static void
usage(int status)
{
  if (status) {
    fprintf(stderr, "Try `" SS0CLI " --help' for more information.\n");
    exit(status);
  }

  printf("SS0 filesystem command-line interface, version " SS0D_VERSION "\n\n"
         "Usage: " SS0CLI " [OPTION]\n"
         "-C, --control-port <port>  use control port <port>\n"
         "-V, --version              print version and exit\n"
         "-h, --help                 display this help and exit\n");
  exit(0);
}

static void
parse_opt(int argc, char** argv)
{
  int ch, l_idx;

  while ((ch = getopt_long(argc, argv, s_opts, l_opts, &l_idx)) >= 0) {
    switch (ch) {
      case 'C':
        break;
      case 'V':
        version();
        break;
      case 'h':
        usage(0);
        break;
      default:
        usage(1);
    }
  }
}

static int
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
    eprintf("control path too long: %s\n", ctl_path);
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

int
main(int argc, char** argv)
{
  int fd = 0;

  parse_opt(argc, argv);

  if ((fd = connect_ctl_ipc()) < 0)
    eprintf_errno("can't connect to ss0 daemon");

  return 0;
}
