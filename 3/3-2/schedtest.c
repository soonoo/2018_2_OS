//
// 2018. 11. 12
// 2014722023 홍순우
// 3-2 schedtest.c
//

// macro for scheduling policy constants
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/sem.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sched.h>

int main()
{
  int num1;
  int i = 0;
  int status;
  int a;
  union semun sem_union;
  struct sembuf sem;
  pid_t pid;
  struct timespec start, end;
  FILE *fp_w;
  struct sched_param param = { 0 };
  char pathname[512];

  clock_gettime(CLOCK_REALTIME, &start);
  for(i = 0; i < MAX_PROCESSES; i++) {
    pid = fork();
    if(pid == 0) break;
  }

  // parent
  if(pid) {
    // wait children processes
    while(wait(&status) != -1);

    clock_gettime(CLOCK_REALTIME, &end);
    
    // print reults
    printf(" %.8f\n", (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec) / 1000000000));
  } else {
    // child

    // change scheduling policy
    //sched_setscheduler(0, SCHED_OTHER, &param);
    //sched_setscheduler(0, SCHED_BATCH, &param);
    //sched_setscheduler(0, SCHED_IDLE, &param);
    //sched_setscheduler(0, SCHED_FIFO, &param);
    sched_setscheduler(0, SCHED_RR, &param);

    sprintf(pathname, "./temp/%d\0", i);
    fp_w = fopen(pathname, "r");
    if(!fp_w) return 0;

    // read two integer
    a = fscanf(fp_w, "%d", &num1);
    a = fscanf(fp_w, "%d", &num1);
    a = fscanf(fp_w, "%d", &num1);
    fclose(fp_w);
  }
}
