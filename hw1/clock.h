/*
 * Embedded System Software, 2021
 *
 * clock.h - clock function defition
 */
#ifndef _CLOCK_H
#define _CLOCK_H

#include <stdbool.h>
#include <time.h>

#include "device_status.h"

/**
 * init_clock - initializes @status to the default value of mode 1(Clock)
 *
 * @status: device status to initialize
 */
void init_clock(struct device_status *status);

/**
 * handle_clock - handles mode 1(Clock)
 *
 * @status: current status of the device
 */
void handle_clock(struct device_status *status);

#endif
