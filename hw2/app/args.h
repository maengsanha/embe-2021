/*
 * Embedded System Software, 2021
 *
 * args.h - user argument definition
 */

/**
 * struct args - user argument
 *
 * @interval: the timer interval (HZ)
 * @cnt:      the number to run timer
 * @init:     the initialization option
 */
struct args {
  int interval;
  int cnt;
  int init;
};