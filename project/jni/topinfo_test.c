#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syscall.h>
#include <sys/wait.h>

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
      return buf;
  }
}

int main() {
  char *info = get_process_info();
  printf("%s\n", info);
  // int uusage = -1;
  // int susage = -1;
  // syscall(376, info, &uusage, &susage);

  // printf("User: %d\n", uusage);
  // printf("System: %d\n", susage);
  free(info);
}
