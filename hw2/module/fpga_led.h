/*
 * Embedded System Software, 2021
 *
 * fpga_led.h - FPGA LED device utility functions definition
 */
#include "args.h"

/**
 * led_init - initializes @led_addr to @init of @param
 *
 * @led_addr: the address of LED device
 * @param:    command line argument from user program
 */
inline void led_init(unsigned char *led_addr, struct args *param);

/**
 * led_exit - initializes @led_addr to zero value
 *
 * @led_addr: the address of LED device
 */
inline void led_exit(unsigned char *led_addr);