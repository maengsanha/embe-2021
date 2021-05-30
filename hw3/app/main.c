/*
 * Embedded System Software, 2021
 *
 * main.c - stop watch module test application
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define DEV_DRIVER "/dev/stopwatch"

int main() {
  int fd;
  if ((fd = open(DEV_DRIVER, O_RDWR)) < 0) {
    printf("open %s failed\n", DEV_DRIVER);
    return 1;
  }

  write(fd, NULL, 0);

  close(fd);
}
