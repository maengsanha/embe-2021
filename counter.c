#include "counter.h"

/**
 * handle_counter - handles mode 2(Counter)
 *
 * @status: current status of the device
 */
void handle_counter(struct device_status *status) {
  do {
    if (status->readkey_val[0].code == BACK) break;
    printf("counter...\n");
    sleep(1);
  } while (status->readkey_val[0].code == COUNTER_MODE);
}