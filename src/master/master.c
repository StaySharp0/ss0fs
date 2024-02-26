#include <stdio.h>

#include "svc.h"

int
main(int argc, char** argv)
{
  printf("master svc\n");

  svc_run();

  return 0;
}
