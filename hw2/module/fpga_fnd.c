/*
 * Embedded System Software, 2021
 *
 * fpga_fnd.c - FPGA FND device utility functions implementation
 */
#include "fpga_fnd.h"

/**
 * fnd_init - initializes @fnd_addr to @init of @param
 *
 * @fnd_addr: the address of FND device
 * @param:    command line argument from user program
 */
inline void fnd_init(unsigned char *fnd_addr, struct args *param) {
  unsigned char val = param->init;
  fnd_addr[0]       = val/1000;
  val               %= 1000;
  fnd_addr[1]       = val/100;
  val               %= 100;
  fnd_addr[2]       = val/10;
  val               %= 10;
  fnd_addr[3]       = val;
}

/**
 * fnd_exit - initializes @fnd_addr to zero value
 *
 * @fnd_addr: the address of FND device
 */
inline void fnd_exit(unsigned char *fnd_addr) {
  fnd_addr[0] = 0x00;
  fnd_addr[1] = 0x00;
  fnd_addr[2] = 0x00;
  fnd_addr[3] = 0x00;
}