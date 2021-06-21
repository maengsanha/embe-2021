#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syscall.h>
#include <sys/wait.h>

#include "sysinfo.h"

#define FILENAME  "/data/local/tmp/output.txt"
#define BUFSIZE   16384

char *get_process_info() {
  pid_t pid;
  if ((pid = fork()) == -1) {
    printf("fork failed\n");
    exit(1);
  } else if (pid == 0) {
    execlp("/system/bin/sh", "/system/bin/sh", "-c", "top -n 1 > /data/local/tmp/output.txt");
  } else {
    wait(NULL);
    char *buf = malloc(sizeof(char)*BUFSIZE);
    int fd;
    if ((fd = open(FILENAME, O_RDONLY)) < 0) {
      printf("open failed: %d\n", fd);
      free(buf);
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

  char *token = strtok(buf, "%%");
  char ustr[strlen(token)];
  strcpy(ustr, token);

  token = strtok(NULL, "%%");
  char sstr[strlen(token)];
  strcpy(sstr, token);

  char *_token = strtok(ustr, " ");
  _token = strtok(NULL, " ");
  int user_usage = atoi(_token);

  char *__token = strtok(sstr, " ");
  __token = strtok(sstr, " ");
  __token = strtok(sstr, " ");
  int sys_usage = atoi(__token);

  // syscall(376, &info, &user_usage, &sys_usage);
  info.user_usage = user_usage;
  info.sys_usage = sys_usage;
  free(buf);

  printf("User: %d\n", info.user_usage);
  printf("System: %d\n", info.sys_usage);
}
