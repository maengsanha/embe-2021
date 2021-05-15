/*
 * Embedded System Software, 2021
 *
 * fpga_led.c - FPGA LED device utility functions implementation
 */
#include "fpga_led.h"

/**
 * led_init - initializes @led_addr to @init of @param
 *
 * @led_addr: the address of LED device
 * @param:    command line argument from user program
 */
inline void led_init(unsigned char *led_addr, struct args *param) {
  switch (get_init_val(param)) {
    case 1:
      *led_addr = 0x80;
      break;
    case 2:
      *led_addr = 0x40;
      break;
    case 3:
      *led_addr = 0x20;
      break;
    case 4:
      *led_addr = 0x10;
      break;
    case 5:
      *led_addr = 0x08;
      break;
    case 6:
      *led_addr = 0x04;
      break;
    case 7:
      *led_addr = 0x02;
      break;
    case 8:
      *led_addr = 0x01;
      break;
    default:
      // no such case
      break;
  }
}

/**
 * led_exit - initializes @led_addr to zero value
 *
 * @led_addr: the address of LED device
 */
inline void led_exit(unsigned char *led_addr) { *led_addr = 0x00; }