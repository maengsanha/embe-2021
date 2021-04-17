#ifndef _TEXT_EDITOR_H
#define _TEXT_EDITOR_H

#include <stdio.h>

#include "device_status.h"

/**
 * init_text_editor - initializes @status to the default status of mode 3(Text editor)
 *
 * @status: device status to initialize
 */
void init_text_editor(struct device_status *status);

/**
 * handle_text_editor - handles mode 3(Text editor)
 *
 * @status: current status of the device
 */
void handle_text_editor(struct device_status *status);

#endif