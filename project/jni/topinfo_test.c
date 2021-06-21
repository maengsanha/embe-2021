#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syscall.h>
#include <sys/wait.h>

#include "sysinfo.h"

#define FILENAME  "/data/local/tmp/output.txt"
#define BUFSIZE   32768

char *get_process_info() {
  switch (fork()) {
    case 0:
      execlp("/system/bin/sh", "/system/bin/sh", "-c", "top -n 1 > /data/local/tmp/output.txt");
      break;
    default:
      wait(NULL);
      char *buf = malloc(sizeof(char)*BUFSIZE);
      int fd;
      if ((fd = open(FILENAME, O_RDONLY)) < 0) {
        printf("open failed :%d\n", fd);
        exit(1);
      }
      read(fd, buf, BUFSIZE);
      close(fd);
      return buf;
  }
}

int main() {
  char *buf = get_process_info();
  struct sys_info_t info = {
    .user_usage = -1,
    .sys_usage = -1,
  };

  syscall(376, &info, buf);
  free(buf);

  printf("User: %d\n", info.user_usage);
  printf("System: %d\n", info.sys_usage);
}
