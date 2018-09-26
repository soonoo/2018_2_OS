/*
 * 2018. 08. 25 OSLAB_2
 * Author: 2014722023 Soonwoo Hong
*/

#include "ftracehooking.h"
#define __NR_ftrace 351

void **syscall_table = (void **)SYSCALL_TABLE;
asmlinkage int (* real_ftrace)(pid_t pid);
pid_t tracing_pid = 0;
EXPORT_SYMBOL(tracing_pid);

asmlinkage pid_t sys_ftrace(pid_t pid)
{
  tracing_pid = pid;
  printk(KERN_INFO "%d\n", tracing_pid);
  printk(KERN_INFO "HELLO WORLD\n");
  return 1;
}

/*
 * replaces __NR_ftrace call to custom ftrace
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

