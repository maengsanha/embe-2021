/**
 * Embedded System Software, 2021
 *
 * args.c - user-level struct args implementation
 */
#include "args.h"

/**
 * get_init_val - returns initial value of @param
 *
 * @param: command line argument from user program
 */
inline unsigned int get_init_val(struct args *param) {
  unsigned int init = param->init;
  if (0 < init/1000) return init/1000;
  if (0 < init/100)  return init/100;
  if (0 < init/10)   return init/10;
  return init;
}