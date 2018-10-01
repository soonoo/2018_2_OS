/*
 * 2018. 08. 25 OSLAB_2
 * Author: 2014722023 Soonwoo Hong
*/

#include <linux/types.h>
#include <linux/module.h>
#include <linux/highmem.h>
#include <asm/unistd.h>

#include <linux/init.h>
#include<linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/fs.h> 
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/slab.h>

asmlinkage long ftrace_open(const char*, int, int);
asmlinkage long ftrace_close(unsigned int);
asmlinkage long ftrace_write(unsigned int, const char*, size_t);
asmlinkage long ftrace_read(unsigned int, const char*, size_t);
asmlinkage long ftrace_lseek(unsigned int, off_t, unsigned int);

int student_id = 2014722023;
const char *MESSAGE_FAIL = "ERROR OCCURED";

// make the page writable
void make_rw(void *address)
{
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long)address, &level);
  if(pte->pte &~ _PAGE_RW) pte->pte |= _PAGE_RW;
}

// make the page write only
void make_ro(void *address)
{
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long)address, &level);
  pte->pte = pte->pte &~ _PAGE_RW;
}

// get_file_name: get file name from file descriptor
// return value: file name if fd is valid and NULL if fd is invalid
char *get_file_name(struct task_struct * current_task, long fd)
{
  struct fdtable *files_table = current_task->files->fdt;
  struct file *files_path = files_table->fd[fd];

  if(files_path) return (char *)files_path->f_path.dentry->d_name.name;
  else return NULL;
}
