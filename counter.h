#ifndef _COUNTER_H
#define _COUNTER_H

#include <stdio.h>

#include "device_status.h"

/**
 * handle_counter - handles mode 2(Counter)
 *
 * @status: current status of the device
 */
void handle_counter(struct device_status *status);

#endif