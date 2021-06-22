/**
 * Embedded System Software, 2021
 *
 * topinfo.c - new system call to parse system information (top)
 */
#include <linux/kernel.h>
#include <linux/uaccess.h>

struct sys_info_t {
  int user_usage;
  int sys_usage;
};

asmlinkage int sys_topinfo(struct sys_info_t *si, int *uusage, int *susage) {
  struct sys_info_t info;
  int user_usage;
  int sys_usage;
  int err;

  if ((err = copy_from_user(&user_usage, uusage, sizeof(int))) > 0) {
    printk(KERN_ALERT "failed copy_from_user: %d\n", err);
    return err;
  }
  if ((err = copy_from_user(&sys_usage, susage, sizeof(int))) > 0) {
    printk(KERN_ALERT "failed copy_from_user: %d\n", err);
    return err;
  }

  info.user_usage = user_usage/10;
  info.sys_usage = sys_usage/10;

  if ((err = copy_to_user(si, &info, sizeof(struct sys_info_t))) > 0) {
    printk(KERN_ALERT "failed copy_to_user: %d\n", err);
    return err;
  }

  return 0;
}
