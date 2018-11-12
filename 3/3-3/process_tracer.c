//
// 2018. 11. 12
// 2014722023 홍순우
// 3-3 process_tracer.c
//

#include <linux/types.h>
#include <linux/module.h>
#include <linux/highmem.h>
#include <asm/unistd.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/fs.h> 
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <linux/pid.h>

#define __NR_ftrace 351

char* get_full_path(struct task_struct *task, char *pathname);
void **syscall_table = (void **)SYSCALL_TABLE;
asmlinkage pid_t (* real_ftrace)(pid_t pid);

asmlinkage pid_t process_tracer(pid_t trace_task)
{
  pid_t pid_temp;
  struct task_struct *task_temp;
  // get task_struct from pid_t
  struct pid *pid_struct = find_get_pid(trace_task);
  struct task_struct *task = pid_task(pid_struct, PIDTYPE_PID);
  int count = 0;
  struct task_struct *task_c;
  struct list_head *list;
  char *pathname, *p;
  pathname = kmalloc(PATH_MAX, GFP_ATOMIC);

  p = get_full_path(task, pathname);
  if(!p) p = task->comm;
  printk(KERN_INFO "[2014722023 OSLab] #### TASK INFORMATION of '[%d] %s' ####", (int)trace_task, p);

  // print task state
  if(task->state == TASK_RUNNING)
    printk(KERN_INFO "[2014722023 OSLab]  - task state: Running or ready\n");
  else if(task->state == TASK_UNINTERRUPTIBLE)
    printk(KERN_INFO "[2014722023 OSLab]  - task state: Wait with ignoring all signals\n");
  else if(task->state == TASK_INTERRUPTIBLE)
    printk(KERN_INFO "[2014722023 OSLab]  - task state: Wait\n");
  else if(task->state == TASK_STOPPED)
    printk(KERN_INFO "[2014722023 OSLab]  - task state: Stopped\n");
  else if(task->state == EXIT_ZOMBIE)
    printk(KERN_INFO "[2014722023 OSLab]  - task state: Zombie process\n");
  else if(task->state == TASK_DEAD)
    printk(KERN_INFO "[2014722023 OSLab]  - task state: Dead\n");
  else
    printk(KERN_INFO "[2014722023 OSLab]  - task state: Etc.\n");

  // print process group information
  pid_temp = pid_vnr(task_pgrp(task));
  task_temp = pid_task(find_vpid(pid_temp), PIDTYPE_PID);
  p = get_full_path(task, pathname);
  if(!p) p = task_temp->comm;
  printk(KERN_INFO "[2014722023 OSLab]  - Process Group Leader: [%d] %s\n", pid_temp, p);

  // print session information
  pid_temp = pid_vnr(task_session(task));
  task_temp = pid_task(find_vpid(pid_temp), PIDTYPE_PID);
  p = get_full_path(task, pathname);
  if(!p) p = task_temp->comm;
  printk(KERN_INFO "[2014722023 OSLab]  - Session Leader: [%d] %s\n", pid_temp, p);

  // print # of context switches
  printk(KERN_INFO "[2014722023 OSLab]  - # of context switch(es): %d\n", task->context_switch_count);

  // print parent process information
  p = get_full_path(task->parent, pathname);
  if(!p) p = task->parent->comm;
  printk(KERN_INFO "[2014722023 OSLab]  - it's parent process: [%d] %s\n", task->parent->pid, p);

  // print sibling processes information
  count = 0;
  printk(KERN_INFO "[2014722023 OSLab]  - it's sibling process(es): ");
  list_for_each(list, &task->parent->children) {
    task_c = list_entry(list, struct task_struct, sibling);
    if(strcmp(task_c->comm, task->comm) == 0) continue;
    p = get_full_path(task_c, pathname);
    if(!p) p = task_c->comm;
    printk(KERN_INFO "[2014722023 OSLab]    > [%d] %s \n", task_c->pid, p);
    count++;
  }
  printk(KERN_INFO "[2014722023 OSLab]    > this process has %d sibling(s)\n", count);

  // print children processes information
  count = 0;
  printk(KERN_INFO "[2014722023 OSLab]  - it's child process(es): ");
  list_for_each(list, &task->children) {
    task_c = list_entry(list, struct task_struct, sibling);
    p = get_full_path(task_c, pathname);
    if(!p) p = task_c->comm;
    printk(KERN_INFO "[2014722023 OSLab]    > [%d] %s \n", task_c->pid, p);
    count++;
  }
  printk(KERN_INFO "[2014722023 OSLab]    > this process has %d child(ren)\n", count);

  printk(KERN_INFO "[2014722023 OSLab] #### END OF INFORMATION ####\n");

  kfree(pathname);
  return trace_task;
}

char *get_full_path(struct task_struct *task, char *pathname)
{
  struct mm_struct *mm;
  char *p = NULL;

  mm = task->mm;
  if (mm) {
    down_read(&mm->mmap_sem);
    if (mm->exe_file) {
      if (pathname) {
        p = d_path(&mm->exe_file->f_path, pathname, PATH_MAX);
      }
    }
    up_read(&mm->mmap_sem);
  }
  return p;
}

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

/*
 * replaces real system calls to ftrace_ function
 */
int module_start(void)
{
  make_rw(syscall_table);

  real_ftrace = syscall_table[__NR_ftrace];
  syscall_table[__NR_ftrace] = process_tracer;

  make_ro(syscall_table);
  return 0;
}

/*
 * restore original system calls' address in systemm call table
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

