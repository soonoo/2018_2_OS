/*
 * 2018. 08. 25 OSLAB_2
 * Author: 2014722023 Soonwoo Hong
 */

#include "ftracehooking.h"

extern pid_t tracing_pid;
void **syscall_table = (void **)SYSCALL_TABLE;

// address of original system calls
asmlinkage long (* real_open)(const char *, int, int);
asmlinkage long (* real_close)(unsigned int);
asmlinkage long (* real_read)(unsigned int, const char *, size_t);
asmlinkage long (* real_write)(unsigned int, const char *, size_t);
asmlinkage long (* real_lseek)(unsigned int, off_t, unsigned int);

asmlinkage long ftrace_open(const char * filename, int flags, int mode)
{
  int fd = real_open(filename, flags, mode);
  if(fd == -1 || tracing_pid == 0 || current->pid != tracing_pid)
    return fd;

  struct fdtable *files_table = current->files->fdt;
  struct path files_path = files_table->fd[fd]->f_path;

  char *buf = (char *)kmalloc(GFP_KERNEL, 100 * sizeof(char));
  char *cwd = d_path(&files_path, buf, 100 * sizeof(char));

  printk(KERN_ALERT "Open file with fd %d  %s", fd, cwd);
  kfree(buf);
  //struct file *fff = ff->fd_array;

  printk(KERN_INFO "open\n");
  printk(KERN_INFO "%d", tracing_pid);
  printk(KERN_INFO " %s\n", current->comm);
  printk(KERN_INFO " %d\n", fd);
  return fd;
}

asmlinkage long ftrace_close(unsigned int fd)
{
  if(!(current->pid == tracing_pid))
    return ftrace_close(fd);

  printk(KERN_INFO "close\n");
  return ftrace_close(fd);
}

asmlinkage long ftrace_read(unsigned int fd, const char * buf, size_t count)
{
  if(!(current->pid == tracing_pid))
    return real_read(fd, buf, count);

  printk(KERN_INFO "read\n");
  return real_read(fd, buf, count);
}

asmlinkage long ftrace_write(unsigned int fd, const char * buf, size_t count)
{
  if(!(current->pid == tracing_pid))
    return real_write(fd, buf, count);

  printk(KERN_INFO "write\n");
  return real_write(fd, buf, count);
}

asmlinkage long ftrace_lseek(unsigned int fd, off_t offset, unsigned int origin)
{
  if(!(current->pid == tracing_pid))
    return real_lseek(fd, offset, origin);

  printk(KERN_INFO "lseek\n");
  return real_lseek(fd, offset, origin);
}

/*
 * replaces real system calls to ftrace_ function
 */
int module_start(void)
{
  make_rw(syscall_table);

  real_open = syscall_table[__NR_open];
  //real_close = syscall_table[__NR_close];
  //real_read = syscall_table[__NR_read];
  //real_write = syscall_table[__NR_write];
  //real_lseek = syscall_table[__NR_lseek];

  syscall_table[__NR_open] = ftrace_open;
  //syscall_table[__NR_close] = ftrace_close;
  //syscall_table[__NR_read] = ftrace_read;
  //syscall_table[__NR_write] = ftrace_write;
  //syscall_table[__NR_lseek] = ftrace_lseek;
  make_ro(syscall_table);
  return 0;
}

/*
 * restore original system calls' address in systemm call table
 */
void module_end(void)
{
  make_rw(syscall_table);
  syscall_table[__NR_open] = real_open;
  //syscall_table[__NR_close] = real_close;
  //syscall_table[__NR_read] = real_read;
  //syscall_table[__NR_write] = real_write;
  //syscall_table[__NR_lseek] = real_lseek;

  make_ro(syscall_table);
}

module_init(module_start);
module_exit(module_end);
MODULE_LICENSE("GPL");

