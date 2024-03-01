#include <fcntl.h>

#include "util.h"

const char* basename = NULL;

void
parse_opt(int argc,
          char** argv,
          struct option* l_opts,
          char* s_opts,
          opt_handler_t handler,
          void* data)
{
  extern char* optarg;
  extern int optind, optopt, opterr;

  int nargc = argc, opt, l_idx;
  char** nargv = argv;
  const char* opt_str;

  if ((basename = strrchr(argv[0], '/')) != NULL)
    basename++;
  else
    basename = argv[0];

  /* ignore getopt_long err msg */
  opterr = 0;

parse:
  opt = getopt_long(nargc, nargv, s_opts, l_opts, &l_idx);
  opt_str = nargv[optind - 1];

  /* unknown short option */
  if (opt == '?' && optopt) {
    eprintf("unrecognized option '-%c'", optopt);
    handler(-1, NULL, data);
  }
  /* unknwon long option */
  else if (opt == '?' && opt_str) {
    eprintf("unrecognized option '%s'", opt_str);
    handler(-1, NULL, data);
  }

  /* non-option arguments */
  if (opt == -1 && optind < nargc) {
    opt = '?';
    optarg = nargv[optind];
    nargc -= optind;
    nargv += optind;
    optind = 1;
  }

  if (opt != -1) {
    handler(opt, optarg, data);
    goto parse;
  }
}
