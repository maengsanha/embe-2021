/*
 * Embedded System Software, 2021
 *
 * procinfo.c - system information JNI function
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#include "procinfo.h"

#define FILENAME      "/data/local/tmp/output.txt"
#define DEVICE_DRIVER "/dev/monitor"
#define BUFSIZE       32768

extern char *get_process_info();

char *get_process_info() {
  pid_t pid;

  if ((pid = fork()) == -1) {
    printf("fork failed\n");
    exit(1);
  } else if (pid == 0) {
    execlp("sh", "sh", "-c", "top -n 1 > /data/local/tmp/output.txt", NULL);
  } else {
    wait(NULL);

    char *buf = malloc(BUFSIZE);

    int fd;
    if ((fd = open(FILENAME, O_RDONLY)) < 0) {
      printf("open failed: %d\n", fd);
      free(buf);
      exit(1);
    }

    read(fd, buf, BUFSIZE);
    close(fd);

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

    struct sys_info_t info = {
      .user_usage = -1,
      .sys_usage = -1,
    };

    syscall(376, &info, &user_usage, &sys_usage);

    // write to device driver
    if ((fd = open(DEVICE_DRIVER, O_WRONLY)) < 0) {
      printf("device open failed: %d\n", fd);
      free(buf);
      exit(1);
    }
    write(fd, &info, sizeof(struct sys_info_t));
    close(fd);

    return buf;
  }

  return NULL;
}
