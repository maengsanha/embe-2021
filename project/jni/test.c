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

int main() {
  execlp("sh", "sh", "-c", "top -n 1 > /data/local/tmp/output.txt");
  sleep(5);
  
  char *buf = malloc(sizeof(char)*BUFSIZE);

  int fd;
  if ((fd = open(FILENAME, O_RDONLY)) < 0) {
    printf("open failed: %d\n", fd);
    free(buf);
    exit(1);
  }

  read(fd, buf, BUFSIZE);
  close(fd);

  printf(buf);
  free(buf);
}
