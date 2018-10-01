/*
 * 2018. 08. 25 OSLAB_2
 * Author: 2014722023 Soonwoo Hong
 */

#include "ftracehooking.h"

extern pid_t tracing_pid;
extern unsigned int read_bytes;
extern unsigned int write_bytes;
void **syscall_table = (void **)SYSCALL_TABLE;

// address of original system calls
asmlinkage long (* real_open)(const char *, int, int);
asmlinkage long (* real_close)(unsigned int);
asmlinkage long (* real_read)(unsigned int, const char *, size_t);
asmlinkage long (* real_write)(unsigned int, const char *, size_t);
asmlinkage long (* real_lseek)(unsigned int, off_t, unsigned int);
char * file_name;

// ftrace_open: trace 'open' system call
asmlinkage long ftrace_open(const char * filename, int flags, int mode)
{
  long fd = real_open(filename, flags, mode);

  if(current->pid != tracing_pid) return fd;
  else if(fd == -1) file_name = (char *)MESSAGE_FAIL;
  else file_name = get_file_name(current, fd);

  printk(KERN_INFO "[%d] /%s opened [%ld] %s\n", student_id, current->comm, fd, file_name);
  return fd;
 }

// ftrace_close: trace 'close' system call
asmlinkage long ftrace_close(unsigned int fd)
{
  long result;
  if(current->pid != tracing_pid) return real_close(fd);

  file_name = get_file_name(current, (long)fd);
  if(!file_name) file_name = (char *)MESSAGE_FAIL;

  result = real_close(fd);

  printk(KERN_INFO "[%d] /%s closed [%u] %s\n", student_id, current->comm, fd, file_name);
  return result;
}

// ftrace_read: trace 'read' system call
asmlinkage long ftrace_read(unsigned int fd, const char * buf, size_t count)
{
  long result = real_read(fd, buf, count);
  if(current->pid != tracing_pid) return result;

  file_name = get_file_name(current, fd);
  if(!file_name || result == -1) file_name = (char *)MESSAGE_FAIL;
  else read_bytes += (unsigned int)result;

  printk(KERN_INFO "[%d] /%s read [%d] %s - %u\n", student_id, current->comm, fd, file_name, (unsigned int)result);
  return result;
}

// ftrace_write: trace 'write' system call
asmlinkage long ftrace_write(unsigned int fd, const char * buf, size_t count)
{
  long result = real_write(fd, buf, count);
  if(current->pid != tracing_pid) return result;

  file_name = get_file_name(current, fd);
  if(!file_name || result == -1) file_name = (char *)MESSAGE_FAIL;
  else write_bytes += (unsigned int)result;

  printk(KERN_INFO "[%d] /%s written [%d] %s - %u\n", student_id, current->comm, fd, file_name, (unsigned int)result);
  return result;
}

// ftrace_lseek: trace 'lseek' system call
asmlinkage long ftrace_lseek(unsigned int fd, off_t offset, unsigned int origin)
{
  long result = real_lseek(fd, offset, origin);
  if(current->pid != tracing_pid)
    return result;

  file_name = get_file_name(current, fd);
  if(!file_name || result == -1) file_name = (char *)MESSAGE_FAIL;

  printk(KERN_INFO "[%d] /%s seek [%d] %s\n", student_id, current->comm, fd, file_name);
  return result;
}

/*
 * replaces real system calls to ftrace_ function
 */
int module_start(void)
{
  make_rw(syscall_table);

  real_open = syscall_table[__NR_open];
  real_close = syscall_table[__NR_close];
  real_read = syscall_table[__NR_read];
  real_write = syscall_table[__NR_write];
  real_lseek = syscall_table[__NR_lseek];

  syscall_table[__NR_open] = ftrace_open;
  syscall_table[__NR_close] = ftrace_close;
  syscall_table[__NR_read] = ftrace_read;
  syscall_table[__NR_write] = ftrace_write;
  syscall_table[__NR_lseek] = ftrace_lseek;

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
  syscall_table[__NR_close] = real_close;
  syscall_table[__NR_read] = real_read;
  syscall_table[__NR_write] = real_write;
  syscall_table[__NR_lseek] = real_lseek;

  make_ro(syscall_table);
}

module_init(module_start);
module_exit(module_end);
MODULE_LICENSE("GPL");
