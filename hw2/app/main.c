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

int main(int argc, char **argv) {
  if (argc != 4) {
    printf("expected 3 arguments, got %d\n", argc-1);
    return 1;
  }

  const char *DEVICE_DRIVER  = "/dev/dev_driver";
  const char *TIMER_INTERVAL = argv[1];
  const char *TIMER_CNT      = argv[2];
  const char *TIMER_INIT     = argv[3];
  const int  interval        = atoi(TIMER_INTERVAL);
  const int  cnt             = atoi(TIMER_CNT);
  const int  init            = atoi(TIMER_INIT);

  int fd;
  if ((fd = open(DEVICE_DRIVER, O_RDWR)) < 0) {
    printf("open %s failed\n", DEVICE_DRIVER);
    close(fd);
    return 1;
  }

  return 0;
}