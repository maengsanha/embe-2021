/**
 * Embedded System Software, 2021
 *
 * topinfo.c - new system call to parse system information (top)
 */
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>

#include "sysinfo.h"

/**
 * parse_info - parse @res and save user usage to @user_usage, system usage to @sys_usage
 *
 * @res:        system information from top
 * @user_usage: variable to store user usage
 * @sys_usage:  variable to store system usage
 */
void parse_info(struct sys_info_t *info, char *buf) {
  char *cur = buf;
  char *token = strsep(&cur, "%%");
  char *ustr = kmalloc(strlen(token), GFP_KERNEL);
  strcpy(ustr, token);

  token = strsep(&cur, "%%");
  char *sstr = kmalloc(strlen(token), GFP_KERNEL);
  strcpy(sstr, token);

  char *_token = strsep(&ustr, " ");
  _token = strsep(&ustr, " ");
  info->user_usage = (int)simple_strtol(_token, NULL, 10);

  char *__token = strsep(&sstr, " ");
  __token = strsep(&sstr, " ");
  __token = strsep(&sstr, " ");
  info->sys_usage = (int)simple_strtol(__token, NULL, 10);

  kfree(ustr);
  kfree(sstr);
}

asmlinkage int sys_topinfo(struct sys_info_t *si, char *str) {
  struct sys_info_t info;
  char *buf = kmalloc(1 << 10, GFP_KERNEL);
  int err;

  if ((err = copy_from_user(buf, str, sizeof(buf))) > 0) {
    printk(KERN_ALERT "failed copy_from_user: %d\n", err);
    return err;
  }

  parse_info(&info, buf);
  kfree(buf);

  if ((err = copy_to_user(si, &info, sizeof(struct sys_info_t))) > 0) {
    printk(KERN_ALERT "failed copy_to_user: %d\n", err);
    return err;
  }

  return 0;
}
