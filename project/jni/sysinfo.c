/*
 * Embedded System Software, 2021
 *
 * sysinfo.c - system information JNI function
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syscall.h>

#include "sysinfo.h"

#define FILENAME      "/data/local/tmp/output.txt"
#define COMMAND       "top -n 1 > /data/local/tmp/output.txt"
#define BUFSIZE       32768
#define DEVICE_DRIVER "/dev/monitor"

/**
 * get_process_info - returns process informations
 */
char *get_process_info() {
  system(COMMAND);
  char *buf = malloc(sizeof(char)*BUFSIZE);
  int fd = open(FILENAME, O_RDONLY);
  read(fd, buf, BUFSIZE);
  return buf;
}

char *getProcessInfo() {
  char *buf = get_process_info();
  struct sys_info_t info = {
    .user_usage = 0,
    .sys_usage = 0,
  };

  // syscall -> parse and set usage
  int fd = open(DEVICE_DRIVER, O_WRONLY);
  write(fd, &info, sizeof(struct sys_info_t));
  close(fd);
  // char * -> jstring
  // free(buf);
  return buf;
}

int main() {
  char *info = getProcessInfo();
  int user_usage = 0;
  int sys_usage = 0;
  syscall(376, info, &user_usage, &sys_usage);
  printf("%s\n", info);
  free(info);
}
