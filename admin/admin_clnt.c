#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "admin.h"
#include "config.h"
#include "log.h"
#include "svc.h"
#include "util.h"

static struct option l_opts[] = {
  { "port", required_argument, 0, 'p' },
  { "version", no_argument, 0, 'V' },
  { "help", no_argument, 0, 'h' },
  { 0, 0, 0, 0 },
};
static char* s_opts = "Vhp:";

static void
usage(int status)
{
  if (status) {
    fprintf(stderr, "Try '%s --help' for more information.\n", basename);
    exit(status);
  }

  printf("SS0 filesystem Administration utility, version " VERSION "\n\n"
         "Usage: %s [OPTION] SERVER TARGET OPERATION\n"
         "-p, --port <port>          Port to send\n"
         "-V, --version              Display version and exit\n"
         "-h, --help                 Print this help and exit\n",
         basename);
  exit(0);
}

static int
parse_svc(char* value)
{
  if (!strcmp("master", value) || !strcmp("msd", value))
    return SVC_MASTER;
  else if (!strcmp("storage", value) || !strcmp("ssd", value))
    return SVC_STORAGE;
  else {
    eprintf("unknown server type: %s\n", value);
    exit(EINVAL);
  }
}

static int
parse_mode(char* value)
{
  if (!strcmp("system", value) || !strcmp("sys", value))
    return MODE_SYSTEM;
  else {
    eprintf("unknown mode: %s\n", value);
    exit(EINVAL);
  }
}

static int
parse_op(char* value)
{
  if (!strcmp("down", value))
    return OP_DOWN;
  else {
    eprintf("unknown operation: %s\n", value);
    exit(EINVAL);
  }
}

static void
opt_handler(int opt, char* value, void* data)
{
  static int idx = 0;

  admin_job_t* job = data;
  cntl_req_t* req = &job->req;
  int rc;

  switch (opt) {
    case 'p':
      if ((rc = STR2NUM(value, job->port)))
        INVAL_OPT(opt, value, usage);
      break;
    case 'V':
      printf("%s version " VERSION "\n", basename);
      exit(0);
    case 'h':
      usage(0);
    case '?':
      if (idx == 0)
        req->svc = parse_svc(value);
      else if (idx == 1)
        req->mode = parse_mode(value);
      else if (idx == 2)
        req->op = parse_op(value);
      else {
        eprintf("invalid argument value '%s'", value);
        usage(EINVAL);
      }
      idx++;
      break;
    default:
      usage(EINVAL);
  }
}

int
main(int argc, char** argv)
{
  admin_job_t job;
  cntl_req_t* req = &job.req;

  ZEROING(job);
  parse_opt(argc, argv, l_opts, s_opts, opt_handler, &job);

  if (req->svc == SVC_INVAL) {
    eprintf("missing server operand");
    usage(1);
  } else if (req->mode == MODE_INVAL) {
    eprintf("missing mode operand");
    usage(1);
  } else if (req->op == OP_INVAL) {
    eprintf("missing operation operand");
    usage(1);
  }

  return send_request(&job);
}
