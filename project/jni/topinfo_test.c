#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syscall.h>
#include <wait.h>

#include "sysinfo.h"

#define FILENAME  "/data/local/tmp/output.txt"
#define BUFSIZE   16384

char *get_process_info() {
  if (fork() == 0) {
    execlp("sh", "sh", "-c", "top -n 1 > /data/local/tmp/output.txt");
  } else {
    wait(NULL);

    char *buf = malloc(BUFSIZE);

    int fd;
    while ((fd = open(FILENAME, O_RDONLY)) < 0);

    read(fd, buf, BUFSIZE);
    close(fd);

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

    syscall(376, &info, &user_usage, &sys_usage);

    printf("User: %d\n", info.user_usage);
    printf("System: %d\n", info.sys_usage);

    return buf;
  }
}

int main() {
  char *buf = get_process_info();
  free(buf);
}
