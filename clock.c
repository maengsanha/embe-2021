#include "clock.h"

/**
 * handle_clock - handles mode 1(Clock)
 *
 * @status: current status of the device
 */
void handle_clock(struct device_status *status) {
  time_t rawtime;
  time(&rawtime);
  struct tm *now  = localtime(&rawtime);
  int h           = now->tm_hour;
  int m           = now->tm_min;
  bool start      = false;
  bool led_3_off  = false;

  // initialize FND to local time, LED to 1
  status->fnd_val[0]  = h/10;
  status->fnd_val[1]  = h%10;
  status->fnd_val[2]  = m/10;
  status->fnd_val[3]  = m%10;
  status->led_val     = 0x80;
  
  do {
    if (status->readkey_val[0].code == BACK) break;
    
    time(&rawtime);
    now = localtime(&rawtime);

    if (status->switch_val[0] == KEY_PRESS) {
      if (!start) {
        start = true;
      } else {
        status->fnd_val[0]  = h/10;
        status->fnd_val[1]  = h%10;
        status->fnd_val[2]  = m/10;
        status->fnd_val[3]  = m%10;
        status->led_val     = 0x80;
        start               = false;
      }
    }

    if (status->switch_val[1] == KEY_PRESS) {
      h                   = now->tm_hour;
      m                   = now->tm_min;
      status->fnd_val[0]  = h/10;
      status->fnd_val[1]  = h%10;
      status->fnd_val[2]  = m/10;
      status->fnd_val[3]  = m%10;
      status->led_val     = 0x80;
      start               = false;
    }

    if (start) {
      status->led_val = led_3_off ? 0x20 : 0x10;
      if (status->switch_val[2] == KEY_PRESS) {
        h = (h + 1) % 24;
      }
      if (status->switch_val[3] == KEY_PRESS) {
        m++;
        h = (h + m/60) % 24;
        m %= 60;
      }
    }

    sleep(1);
  } while (status->mode == CLOCK_MODE);
}