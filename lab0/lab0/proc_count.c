#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

/*
Credits:
- https://sysprog21.github.io/lkmpg/ Linux Kernel Module Programming Guide
- Disc 1A and Disc 1C slides
*/

static struct proc_dir_entry *my_proc_file;

static int proc_count(struct seq_file *s, void *v) {
  struct task_struct *p;
  unsigned long long int my_process_count = 0;
  for_each_process(p) {
    my_process_count++;
  }
  seq_printf(s, "%lld\n", my_process_count);
  return 0;
}
static int __init proc_count_init(void)
{
  my_proc_file = proc_create_single("count", 0644, NULL, &proc_count);
  if (NULL == my_proc_file) {
    pr_alert("error: could not initialize /proc/count\n");
    return -ENOMEM;
  }
  pr_info("proc_count: /proc/count created\n");
  return 0;
}

static void __exit proc_count_exit(void)
{
  proc_remove(my_proc_file);
  pr_info("proc_count: /proc/count destroyed\n");
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Arteen Abrishami");
MODULE_DESCRIPTION(
"Display number of running processes \
on machine at /proc/count");
MODULE_LICENSE("GPL");
