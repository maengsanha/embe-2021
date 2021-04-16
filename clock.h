#ifndef _CLOCK
#define _CLOCK

#include <stdbool.h>
#include <time.h>

#include "device_status.h"

const unsigned int CLOCK_MODE = 0;

/**
 * clock - handles mode 1(Clock)
 *
 * @status: current status of the device
 */
extern inline void clock(struct device_status *status) {
  struct tm *now  = localtime(time(NULL));
  int h           = now->tm_hour;
  int m           = now->tm_min;
  status->led_val = 0x80;
  bool start      = false;
  bool save       = false;

  do {
    // if 1st switch has been pressed, start or save
    if (status->switch_val[0] == 1) {
      status->led_val = 0x20;
      sleep(1);
      status->led_val = 0x10;
      sleep(1);
      if (!start) {
        start = true;
      } else {
        save = true;
      }
    }

    // if 2nd switch has been pressed, reset
    if (status->switch_val[1] == 1) {
      now = localtime(time(NULL));
      h = now->tm_hour;
      m = now->tm_min;
      status->fnd_val[0] = h/10;
      status->fnd_val[1] = h%10;
      status->fnd_val[2] = m/10;
      status->fnd_val[3] = m%10;
    }

    // if 3rd switch has been pressed, increase hour
    if (status->switch_val[2] == 1) {
      h++;
      h %= 24;
    }

    // if 4th switch has been pressed, increase minute
    if (status->switch_val[3] == 1) {
      m++;
      h = (h + m/60) % 24;
      m %= 60;
    }

    if (save) {
      status->fnd_val[0] = h/10;
      status->fnd_val[1] = h%10;
      status->fnd_val[2] = m/10;
      status->fnd_val[3] = m%10;
    }

  } while (status->mode == CLOCK_MODE);
}

#endif