#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <include/linux/kernel.h>

/**
 * parse_info - parse @res and save user usage to @user_usage, system usage to @sys_usage
 *
 * @res:        system information from top
 * @user_usage: variable to store user usage
 * @sys_usage:  variable to store system usage
 */
void parse_info(char res[], int *user_usage, int *sys_usage) {
  char *tokenizer = strtok(info, "%%");
  char ustr[strlen(tokenizer)];
  strcpy(ustr, tokenizer);
  tokenizer = strtok(NULL, "%%");
  char sstr[strlen(tokenizer)];
  strcpy(sstr, tokenizer);

  char *_tokenizer = strtok(ustr, " ");
  _tokenizer = strtok(NULL, " ");
  *user_usage = (int)simple_strtol(_tokenizer, NULL, 10);

  char *__tokenizer = strtok(sstr, " ");
  __tokenizer = strtok(NULL, " ");
  __tokenizer = strtok(NULL, " ");
  *sys_usage = (int)simple_strtol(__tokenizer, NULL, 10);
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
