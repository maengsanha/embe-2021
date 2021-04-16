#ifndef _TEXT_EDITOR_H
#define _TEXT_EDITOR_H

#include <stdio.h>
#include <unistd.h>

#include "device_status.h"

/**
 * handle_text_editor - handles mode 3(Text editor)
 *
 * @status: current status of the device
 */
void handle_text_editor(struct device_status *status);

#endif