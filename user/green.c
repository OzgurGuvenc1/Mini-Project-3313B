#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
spin_forever(void)
{
  volatile uint64 x = 0;

  for(;;){
    x++;
  }
}

static void
usage(void)
{
  fprintf(2, "usage: green score | green status | green burn <ticks> | green throttle <pid> <ticks> | green demo\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  if(argc < 2)
    usage();

  if(strcmp(argv[1], "score") == 0){
    printf("uptime %d ticks, estimated energy %d%%\n", uptime(), uptime_energy());
    exit(0);
  }

  if(strcmp(argv[1], "status") == 0){
    printf("Green-Core status\n");
    printf("  uptime: %d ticks\n", uptime());
    printf("  estimated energy: %d%% busy\n", uptime_energy());
    printf("  demo commands:\n");
    printf("    green demo\n");
    printf("    green burn <ticks> &   (use 0 for infinite burn)\n");
    printf("    green throttle <pid> <ticks>\n");
    exit(0);
  }

  if(strcmp(argv[1], "burn") == 0){
    int duration;
    int start;
    volatile uint64 x = 0;

    if(argc != 3)
      usage();
    duration = atoi(argv[2]);
    start = uptime();
    if(duration == 0)
      printf("green burn pid %d running until killed\n", getpid());
    else if(duration > 0)
      printf("green burn pid %d for %d ticks\n", getpid(), duration);
    else {
      fprintf(2, "green: burn duration must be >= 0\n");
      exit(1);
    }

    while(duration == 0 || uptime() - start < duration){
      x++;
    }
    printf("green burn pid %d done\n", getpid());
    exit(0);
  }

  if(strcmp(argv[1], "throttle") == 0){
    int pid;
    int limit;

    if(argc != 4)
      usage();
    pid = atoi(argv[2]);
    limit = atoi(argv[3]);
    if(throttle(pid, limit) < 0){
      fprintf(2, "green: throttle failed for pid %d\n", pid);
      exit(1);
    }
    printf("pid %d throttle cooldown set to %d ticks\n", pid, limit);
    exit(0);
  }

  if(strcmp(argv[1], "demo") == 0){
    int pids[2];

    for(int i = 0; i < 2; i++){
      int pid = fork();
      if(pid < 0){
        fprintf(2, "green: fork failed\n");
        exit(1);
      }
      if(pid == 0)
        spin_forever();
      pids[i] = pid;
    }

    if(throttle(pids[1], 5) < 0){
      fprintf(2, "green: failed to throttle pid %d\n", pids[1]);
      kill(pids[0]);
      kill(pids[1]);
      wait(0);
      wait(0);
      exit(1);
    }

    printf("demo workers: pid %d normal, pid %d throttled\n", pids[0], pids[1]);
    for(int sample = 0; sample < 5; sample++){
      pause(10);
      printf("sample %d: uptime %d ticks, energy %d%%\n",
             sample + 1, uptime(), uptime_energy());
    }

    kill(pids[0]);
    kill(pids[1]);
    wait(0);
    wait(0);
    exit(0);
  }

  usage();
  return 0;
}
