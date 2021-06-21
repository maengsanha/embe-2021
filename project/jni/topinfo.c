/**
 * Embedded System Software, 2021
 *
 * topinfo.c - new system call to parse system information (top)
 */
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
void parse_info(char res[], int *user_usage, int *sys_usage) {
  char *cur = res;
  char *token = strsep(&cur, "%%");
  char ustr[strlen(token)];
  strcpy(ustr, token);

  token = strsep(&cur, "%%");
  char sstr[strlen(token)];
  strcpy(sstr, token);

  char *cur2 = ustr;
  char *_token = strsep(&cur2, " ");
  _token = strsep(&cur2, " ");
  *user_usage = (int)simple_strtol(_token, NULL, 10);

  char *cur3 = sstr;
  char *__token = strsep(&cur3, " ");
  __token = strsep(&cur3, " ");
  __token = strsep(&cur3, " ");
  *sys_usage = (int)simple_strtol(__token, NULL, 10);
}

asmlinkage int sys_topinfo(char *info, int *uusage, int *susage) {
  int user_usage = 0;
  int sys_usage = 0;
  char res[1 << 15];
  copy_from_user(res, info, sizeof(res));

  parse_info(res, &user_usage, &sys_usage);

  copy_to_user(uusage, &user_usage, sizeof(int));
  copy_to_user(susage, &sys_usage, sizeof(int));

  return 0;
}
