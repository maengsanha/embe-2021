/**
 * Embedded system software, 2021
 *
 * counter.h - counter function defition
 */
#ifndef _COUNTER_H
#define _COUNTER_H

#include <stdio.h>

#include "device_status.h"

/**
 * init_counter - initializes @status to the default value of mode 2(Counter)
 *
 * @status: device status to initialize
 */
void init_counter(struct device_status *status);

/**
 * handle_counter - handles mode 2(Counter)
 *
 * @status: current status of the device
 */
void handle_counter(struct device_status *status);

#endif