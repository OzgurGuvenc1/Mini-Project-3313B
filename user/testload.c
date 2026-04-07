#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int before, after;
  volatile int i;

  before = getprocload();

  for(i = 0; i < 10000000; i++)
    ;

  after = getprocload();

  printf("runtime before: %d\n", before);
  printf("runtime after: %d\n", after);
  printf("ticks consumed: %d\n", after - before);

  exit(0);
}
