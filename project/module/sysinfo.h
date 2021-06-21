/*
 * Embedded System Software, 2021
 *
 * sysinfo.c - system information definition
 */
#ifndef _SYSINFO_H
#define _SYSINFO_H

/**
 * struct sys_info_t - resource usage information
 *
 * @user_usage: user usage
 * @sys_usage:  system usage
 */
struct sys_info_t {
  int user_usage;
  int sys_usage;
};

#endif
