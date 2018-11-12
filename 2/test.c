#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, const char* argv[])
{
  char buf[1024];
  pid_t pid = getpid();
  int fd;
  int n;
  syscall(__NR_ftrace, pid);

    fd = open("abc.txt", O_RDONLY);
    close(fd);
  lseek(fd, -14, SEEK_END);
//  read(fd, buf, 14);
  write(STDOUT_FILENO, buf, n);
  syscall(__NR_ftrace, 0);

  return 0;
}


