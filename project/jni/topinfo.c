/**
 * Embedded System Software, 2021
 *
 * topinfo.c - new system call to parse system information (top)
 */
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>

/**
 * parse_info - parse @res and save user usage to @user_usage, system usage to @sys_usage
 *
 * @res:        system information from top
 * @user_usage: variable to store user usage
 * @sys_usage:  variable to store system usage
 */
void parse_info(struct sys_info_t *info, char *buf) {
  char *token = strsep(&buf, "%%");
  char ustr[strlen(token)];
  strcpy(ustr, token);

  token = strsep(&buf, "%%");
  char sstr[strlen(token)];
  strcpy(sstr, token);

  char *_ustr = ustr;
  char *_token = strsep(&_ustr, " ");
  _token = strsep(&_ustr, " ");
  info->user_usage = (int)simple_strtol(_token, NULL, 10);

  char *_sstr = sstr;
  char *__token = strsep(&_sstr, " ");
  __token = strsep(&_sstr, " ");
  __token = strsep(&_sstr, " ");
  info->sys_usage = (int)simple_strtol(__token, NULL, 10);
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

  if ((err = copy_to_user(si, &info, sizeof(int))) > 0) {
    printk(KERN_ALERT "failed copy_to_user: %d\n", err);
    return err;
  }

  return 0;
}
