//
// 2018. 11. 12
// 2014722023 홍순우
// 3-1 fork.c
// 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/sem.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>

static int *offset;
int sem_id = 0;

int main()
{
  int num1, num2;
  int i = 0;
  int status;
  int a, b;
  union semun sem_union;
  struct sembuf sem;
  pid_t pid;
  struct timespec start, end;
  FILE *fp_w = fopen("./temp.txt", "r+");
  if(!fp_w) return 0;

  // shared memory for fiel offset
  offset  = mmap(NULL, sizeof *offset, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  *offset = 0;

  // create semaphore
  sem_id = semget((key_t)2014, 1, 0666 | IPC_CREAT);
  sem_union.val = 1;
  semctl(sem_id, 0, SETVAL, sem_union);

  // check execution time
  clock_gettime(CLOCK_REALTIME, &start);
  for(i = 0; i < MAX_PROCESSES * 2; i++) {
    pid = fork();
    if(pid == 0) break;
  }

  // parent
  if(pid) {
    // wait children processes
    while(wait(&status) != -1);

    fclose(fp_w);
    clock_gettime(CLOCK_REALTIME, &end);
    
    // print reults
    printf(" %.8f\n", (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec) / 1000000000));
  } else {
    // child
    while(1) {
      // P()
      sem.sem_op = -1;
      sem.sem_num = 0;
      sem.sem_flg = SEM_UNDO;
      if(semop(sem_id, &sem, 1) == -1) continue;

      // read two integer
      fseek(fp_w, *offset, SEEK_SET);
      a = fscanf(fp_w, "%d", &num1);
      b = fscanf(fp_w, "%d", &num2);
      *offset = ftell(fp_w);

      // return if EOF is detected
      if(a == -1 || b == -1) {
        fseek(fp_w, 0, SEEK_END);
        return 0;
      }

      fseek(fp_w, 0, SEEK_END);
      fprintf(fp_w, "%d\n", num1 * num2);
      fflush(fp_w);

      // V()
      sem.sem_op = 1;
      sem.sem_num = 0;
      sem.sem_flg = SEM_UNDO;
      semop(sem_id, &sem, 1);
    }
  }
}
