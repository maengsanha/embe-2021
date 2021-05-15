/**
 * Embedded System Software, 2021
 *
 * args.h - user-level struct args definition
 */

/**
 * struct args - same as user-level struct args
 *
 * See app/args.h
 */
struct args {
  int interval;
  int cnt;
  int init;
};

/**
 * get_init_val - returns initial value of @param
 *
 * @param: command line argument from user program
 */
inline unsigned int get_init_val(struct args *param);