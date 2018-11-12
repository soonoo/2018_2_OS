#include <linux/unistd.h>
#include <sys/types.h>

int main(void)
{
  syscall(__NR_ftrace, 1);
  return 0;
}
