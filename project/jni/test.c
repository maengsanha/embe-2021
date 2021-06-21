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
  if (fork() == 0) {
    execlp("sh", "sh", "-c", "top -n 1 > /data/local/tmp/output.txt");
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

    return buf;
  }
}

int main() {
  char *buf = get_process_info();
  printf(buf);
  free(buf);
}
