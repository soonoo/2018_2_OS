#include <linux/module.h>
#include <linux/sched.h>

int display_task_init(void)
{
  struct task_struct *findtask = &init_task;

  do {
    printk("%s[%d] -> ", findtask->comm, findtask->pid);
    findtask = next_task(findtask);
  } while((findtask->pid != init_task.pid));

  printk("%s[%d]\n", findtask->comm, findtask->pid);

  return 0;
}

void display_task_exit(void)
{

}

module_init(display_task_init);
module_exit(display_task_exit);
MODULE_LICENSE("GPL");

