//
// 2018. 11. 12
// 2014722023 홍순우
// 3-1 thread.c
// 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/sem.h>
#include <pthread.h>
#include <time.h>

int sem_id = 0;
FILE *fp;
int offset = 0;
union semun sem_union;
struct timespec start, end;
void* thread_func(void *arg);

int main()
{
  fp = fopen("./temp.txt", "r+");
  pthread_t tid[MAX_PROCESSES * 2];
  int i = 0;
  if(!fp) return 0;

  // create semaphore
  sem_id = semget((key_t)12345, 1, 0666 | IPC_CREAT);
  sem_union.val = 1;
  semctl(sem_id, 0, SETVAL, sem_union);

  clock_gettime(CLOCK_REALTIME, &start);
  for(i = 0; i < MAX_PROCESSES * 2; i++) {
    pthread_create(&tid[i], NULL, thread_func, NULL);
  }

  for(i = 0; i < MAX_PROCESSES * 2; i++) {
    pthread_join(tid[i], NULL);
  }
  
  semctl(sem_id, 0, IPC_RMID, 0);
  // wait all thread to return
  return 0;
}

void* thread_func(void *arg)
{
  int num1, num2;
  int a, b;
  struct sembuf s;

  while(1) {
    // P()
    s.sem_op = -1;
    s.sem_num = 0;
    s.sem_flg = SEM_UNDO;
    if(semop(sem_id, &s, 1) == -1) continue;

    // read two integers
    fseek(fp, offset, SEEK_SET);
    a = fscanf(fp, "%d", &num1);
    b = fscanf(fp, "%d", &num2);
    offset = ftell(fp);

    // return if EOF is detected
    if(a == -1 || b == -1) {
      fseek(fp, 0, SEEK_END);

      clock_gettime(CLOCK_REALTIME, &end);
      // print reults
      printf(" %.8f\n", (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec) / 1000000000));
      exit(0);
    }

    // write result
    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%d\n", num1 * num2);
    fflush(fp);

    // V()
    s.sem_op = 1;
    s.sem_num = 0;
    s.sem_flg = SEM_UNDO;
    semop(sem_id, &s, 1);
  }

}
