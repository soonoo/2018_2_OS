#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, const char* argv[])
{
  syscall(__NR_ftrace, getpid());
  int a = open("./abc.txt", O_RDONLY, S_IRUSR);
  printf("pid: %d\n", a);
  //printf("%d\n", errno);
  return 0;
}
