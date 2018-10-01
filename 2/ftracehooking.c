/*
 * 2018. 08. 25 OSLAB_2
 * Author: 2014722023 Soonwoo Hong
*/

#include "ftracehooking.h"
#define __NR_ftrace 351

void **syscall_table = (void **)SYSCALL_TABLE;
asmlinkage int (* real_ftrace)(pid_t pid);

// tracing_pid keep track of current tracing process id
// if 0, do not trace system call
pid_t tracing_pid = 0;
EXPORT_SYMBOL(tracing_pid);

unsigned int read_bytes = 0;
EXPORT_SYMBOL(read_bytes);

unsigned int write_bytes = 0;
EXPORT_SYMBOL(write_bytes);

// sys_ftrace: set current tracing pid
asmlinkage pid_t sys_ftrace(pid_t pid)
{
  if(tracing_pid) {
    printk(KERN_INFO "[%d] /%s stats [x] read - %d / written - %d\n",
      student_id, current->comm, read_bytes, write_bytes);
  }

  read_bytes = 0;
  write_bytes = 0;
  tracing_pid = pid;

  return tracing_pid;
}

/*
 * replace __NR_ftrace call to custom ftrace
 */
int module_start(void)
{
  make_rw(syscall_table);

  real_ftrace = syscall_table[__NR_ftrace];
  syscall_table[__NR_ftrace] = sys_ftrace;

  make_ro(syscall_table);
  return 0;
}

/*
 * restore original frace's address in systemm call table
 */
void module_end(void)
{
  make_rw(syscall_table);
  syscall_table[__NR_ftrace] = real_ftrace;
  make_ro(syscall_table);
}

module_init(module_start);
module_exit(module_end);
MODULE_LICENSE("GPL");
