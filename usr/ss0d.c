/*
 * SS0 filesystem daemon
 *
 * Copyright (C) 2024 StaySharp0 Yongjun Kim <staysharp0@gmail.com>
 */
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/stat.h>

#include <oom.h>
#include <types.h>
#include <util.h>

#include "event.h"
#include "log.h"
#include "ss0d.h"

const char program_name[] = SS0D;
bool_t is_active = true;
bool_t is_debug = true;
short int control_port;

static struct option const l_opts[] = {
  { "foreground", no_argument, 0, 'f' },
  { "pid-file", required_argument, 0, 'p' },
  { "version", no_argument, 0, 'V' },
  { "help", no_argument, 0, 'h' },
  { 0, 0, 0, 0 },
};
static char* s_opts = "fVhp:";

static void
usage(int status)
{
  if (status) {
    fprintf(stderr, "Try `" SS0D " --help' for more information.\n");
    exit(status);
  }

  printf("SS0 filesystem daemon, version " SS0D_VERSION "\n\n"
         "Usage: " SS0D " [OPTION]\n"
         "-f, --foreground        run the program in the foreground\n"
         "-p, --pid-file          filename specify the pid file\n"
         "-V, --version           print version and exit\n"
         "-h, --help              display this help and exit\n");
  exit(0);
}

static void
parse_opt(int argc, char** argv)
{
  int ch, l_idx;

  while ((ch = getopt_long(argc, argv, s_opts, l_opts, &l_idx)) >= 0) {
    switch (ch) {
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

static void
set_proc_oom(void)
{
  int fd = 0;
  const char *path = OOM_SCORE_ADJ, *score = OOM_SCORE_ADJ_MIN_STR;
  struct stat st;

  if (stat(path, &st) < 0) {
    path = OOM_ADJ;
    score = OOM_DISABLE_STR;
  }

  if ((fd = open(path, O_WRONLY)) < 0) {
    eprintf_errno("can't open %s", path);
    goto out;
  }

  if (write(fd, score, strlen(score)) < 0) {
    eprintf_errno("can't write %s (%s)", path, score);
    goto out;
  }

out:
  if (fd > 0)
    close(fd);

  if (errno && (errno != EACCES) && IS_ROOT_USER)
    exit(1);
}

static void
set_proc_nr_open(void)
{
  int ret, fd = 0, max = 1024 * 1024;
  const char path[] = "/proc/sys/fs/nr_open";
  char buf[64];
  struct rlimit rlim;

  if ((fd = open(path, O_RDONLY)) < 0) {
    eprintf_errno("can't open %s", path);
    goto set_rlimit;
  }

  if ((ret = read(fd, buf, sizeof(buf))) < 0) {
    eprintf_errno("can't read %s", path);
    close(fd);
    exit(1);
  }
  close(fd);

  if ((max = atoi(buf)) == -1 && errno) {
    eprintf_errno("can't read %s (%s)", path, buf);
    exit(1);
  }

set_rlimit:
  rlim.rlim_cur = rlim.rlim_max = max;

  if ((ret = setrlimit(RLIMIT_NOFILE, &rlim)) < 0) {
    eprintf_errno("can't set RLIMIT_NOFILE (%d)", max);
    exit(1);
  }
}

static void
exit_main(int signo)
{
  if (is_active) {
    close_ctl_ipc();
    is_active = false;
  }
}

int
main(int argc, char** argv)
{
  parse_opt(argc, argv);

  set_signal(exit_main);
  set_proc_oom();
  set_proc_nr_open();

  init_event_loop();
  init_ctl_ipc();

  run_event_loop();

  exit_main(0);

  return 0;
}
