
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

#include "log.h"
#include "oom.h"
#include "svc.h"
#include "svc_internal.h"

#define IS_ROOT_USER (getuid() == 0)

const char* basename = NULL;
bool_t is_active = true;
static flag_t svc_mode = 0;

static inline void
set_signal(__sighandler_t handler)
{
  struct sigaction sa_old;
  struct sigaction sa_new;

  /* TODO: 임시 핸들러, stop cli 구현 후 제거 필요 */
  sa_new.sa_handler = handler;
  sigemptyset(&sa_new.sa_mask);

  sa_new.sa_flags = 0;
  sigaction(SIGPIPE, &sa_new, &sa_old);
  sigaction(SIGINT, &sa_new, &sa_old);
  sigaction(SIGTERM, &sa_new, &sa_old);
}

static void
set_oom(void)
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
set_nr_open(void)
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

  if ((max = atoi(buf)) == 0 && errno) {
    eprintf_errno("can't convert '%s' to integer value", buf);
    exit(1);
  }

set_rlimit:
  rlim.rlim_cur = rlim.rlim_max = max;

  if ((ret = setrlimit(RLIMIT_NOFILE, &rlim)) < 0) {
    eprintf_errno("can't set RLIMIT_NOFILE(%d)", max);
    exit(1);
  }
}

static void
init_daemon(void)
{
  const bool_t is_daemon = !(svc_mode & MODE_SVC_FG);

  if (is_daemon && daemon(0, 0))
    exit(1);
}

static void
exit_svc(int signo)
{
  if (is_active) {
    exit_event_loop();
    is_active = false;
  }
}

void
svc_parse_opt(int argc,
              char** argv,
              struct option* l_opts,
              char* s_opts,
              opt_handler_t handler,
              void* data)
{
  extern char* optarg;
  extern int optind;
  int opt, l_idx;

  if ((basename = strrchr(argv[0], '/')) != NULL)
    basename++;
  else
    basename = argv[0];

  do {
    opt = getopt_long(argc, argv, s_opts, l_opts, &l_idx);
    if (opt == -1 && optind < argc) {
      opt = '?';
      optarg = argv[optind];
      argc -= optind;
      argv += optind;
      optind = 1;
    }

    if (opt != -1) {
      //      dprintf("opt '%c': '%s'\n", opt, optarg);
      handler(opt, optarg, data);
    }
  } while (opt != -1);
}

void
svc_set_mode(flag_t flag)
{
  svc_mode |= flag;
}

void
svc_run(void)
{
  set_signal(exit_svc);
  set_oom();
  set_nr_open();

  init_daemon();
  init_event_loop();

  run_event_loop();

  exit_svc(0);
}
