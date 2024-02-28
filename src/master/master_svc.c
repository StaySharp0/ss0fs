#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "svc.h"

static struct option l_opts[] = {
  { "foreground", no_argument, 0, 'f' },
  { "port", required_argument, 0, 'p' },
  { "version", no_argument, 0, 'V' },
  { "help", no_argument, 0, 'h' },
  { 0, 0, 0, 0 },
};

static char* s_opts = "fVhp:";

static void
usage(int status)
{
  if (status) {
    fprintf(stderr, "Try `%s --help' for more information.\n", basename);
    exit(EINVAL);
  }

  printf("SS0 filesystem Master Server Daemon, version " VERSION "\n\n"
         "Usage: %s [OPTION]\n"
         "-f, --foreground           Run the program in the foreground\n"
         "-p, --port <port>          Port to listen on\n"
         "-V, --version              Display version and exit\n"
         "-h, --help                 Print this help and exit\n",
         basename);
  exit(0);
}

static void
opt_handler(int opt, char* value, void* data)
{

  switch (opt) {
    case 'f':
      svc_set_mode(MODE_SVC_FG);
      break;
    case 'V':
      printf("%s version " VERSION "\n", basename);
      exit(0);
    case 'h':
      usage(0);
    case '?':
      break;
    default:
      usage(1);
  }
}

int
main(int argc, char** argv)
{
  svc_parse_opt(argc, argv, l_opts, s_opts, opt_handler, NULL);

  svc_run();

  return 0;
}
