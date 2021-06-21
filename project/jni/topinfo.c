/**
 * Embedded System Software, 2021
 *
 * topinfo.c - new system call to parse system information (top)
 */
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

/**
 * parse_info - parse @res and save user usage to @user_usage, system usage to @sys_usage
 *
 * @res:        system information from top
 * @user_usage: variable to store user usage
 * @sys_usage:  variable to store system usage
 */
void parse_info(char *buf, int *user_usage, int *sys_usage) {
  char *token = strsep(&buf, "%%");
  char ustr[strlen(token)];
  strcpy(ustr, token);

  token = strsep(&buf, "%%");
  char sstr[strlen(token)];
  strcpy(sstr, token);

  char *_ustr = ustr;
  char *_token = strsep(&_ustr, " ");
  _token = strsep(&_ustr, " ");
  *user_usage = (int)simple_strtol(_token, NULL, 10);

  char *_sstr = sstr;
  char *__token = strsep(&_sstr, " ");
  __token = strsep(&_sstr, " ");
  __token = strsep(&_sstr, " ");
  *sys_usage = (int)simple_strtol(__token, NULL, 10);
}

asmlinkage int sys_topinfo(char *info, int *uusage, int *susage) {
  char *buf = kmalloc(1 << 10, GFP_KERNEL);
  int user_usage = 0;
  int sys_usage = 0;
  int err;
  if ((err = copy_from_user(buf, info, sizeof(buf))) > 0) {
    return err;
  }

  parse_info(buf, &user_usage, &sys_usage);
  kfree(buf);

  if ((err = copy_to_user(uusage, &user_usage, sizeof(int))) > 0) {
    return err;
  }
  if ((err = copy_to_user(susage, &sys_usage, sizeof(int))) > 0) {
    return err;
  }

  return 0;
}
