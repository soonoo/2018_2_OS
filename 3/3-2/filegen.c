//
// 2018. 11. 12
// 2014722023 홍순우
// 3-2 filegen.c
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
  FILE *fp;
  char dir_name[512];
  char file_name[256];
  int i = 0;
  srand((unsigned)time(NULL));

  mkdir("./temp/", 0777);
  chdir("./temp");
  // create files
  for(i = 0; i < MAX_PROCESSES; i++) {
    sprintf(file_name, "%d", i);

    fp = fopen(file_name, "w");
    fprintf(fp, "%d", rand() + 9);
    fclose(fp);
  }
}
