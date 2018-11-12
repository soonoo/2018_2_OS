//
// 2018. 11. 12
// 2014722023 홍순우
// 3-1 numgen.c
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  int i;
  FILE *fp = fopen("./temp.txt", "w");
  if(!fp) return 0;

  // write file
  srand((unsigned)time(NULL));
  for(i = 0; i < MAX_PROCESSES * 2; i++) {
    fprintf(fp, "%d\n", rand() + 1);
  }
  fclose(fp);
}
