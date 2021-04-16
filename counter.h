#ifndef _COUNTER_H
#define _COUNTER_H

#include "device_status.h"

#define COUNTER_MODE 1

/**
 * counter - handles mode 2(Counter)
 *
 * @status: current status of the device
 */
extern inline void counter(struct device_status *status);

#endif