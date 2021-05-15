/*
 * Embedded System Software, 2021
 *
 * main.c - timer module test
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "args.h"

#define DEV_DRIVER "/dev/dev_driver"
#define MAG_NUMBER 'T'
#define SET_OPTION _IOW(MAG_NUMBER, 0, int)
#define COMMAND    _IOW(MAG_NUMBER, 1, int)

int main(int argc, char **argv) {
  if (argc != 4) {
    printf("expected 3 arguments, got %d\n", argc-1);
    return 1;
  }

  const char *TIMER_INTERVAL = argv[1];
  const char *TIMER_CNT      = argv[2];
  const char *TIMER_INIT     = argv[3];

  struct args param = {
    .interval = atoi(TIMER_INTERVAL),
    .cnt      = atoi(TIMER_CNT),
    .init     = atoi(TIMER_INIT),
  };

  // open device
  int fd;
  if ((fd = open(DEV_DRIVER, O_WRONLY)) < 0) {
    printf("open %s failed\n", DEV_DRIVER);
    close(fd);
    return 1;
  }

  // deliver device driver option
  if (ioctl(fd, SET_OPTION, &param) < 0) {
    printf("ioctl 1 failed\n");
    close(fd);
    return 1;
  }

  // run timer device driver
  if (ioctl(fd, COMMAND) < 0) {
    printf("ioctl 2 failed\n");
    close(fd);
    return 1;
  }

  close(fd);
}