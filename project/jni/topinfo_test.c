#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syscall.h>

#define FILENAME  "/data/local/tmp/output.txt"
#define COMMAND   "top -n 1 > /data/local/tmp/output.txt"
#define BUFSIZE   32768

char *get_process_info() {
  system(COMMAND);
  char *buf = malloc(sizeof(char)*BUFSIZE);
  int fd = open(FILENAME, O_RDONLY);
  read(fd, buf, BUFSIZE);
  return buf;
}

int main() {
  char *info = get_process_info();
  int uusage = 0;
  int susage = 0;
  syscall(376, info, &uusage, &susage);

  printf("User: %d\n", uusage);
  printf("System: %d\n", susage);
  free(info);
}
