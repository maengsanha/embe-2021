#ifndef _CLOCK_H
#define _CLOCK_H

#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include "device_status.h"

/**
 * handle_clock - handles mode 1(Clock)
 *
 * @status: current status of the device
 */
void handle_clock(struct device_status *status);

#endif