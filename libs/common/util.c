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
      handler(opt, optarg, data);
    }
  } while (opt != -1);
}
