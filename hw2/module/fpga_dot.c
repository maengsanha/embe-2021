/*
 * Embedded System Software, 2021
 *
 * fpga_dot.c - FPGA Dot Matrix device utility functions implementation
 */
#include "fpga_dot.h"

/**
 * dot_matrix_init - initializes @dot_matrix_addr to @init of @param
 *
 * @dot_matrix_addr: the address of Dot Matrix device
 * @param:           command line argument from user program
 */
inline void dot_matrix_init(unsigned char *dot_matrix_addr, struct args *param) {
  char number[10] = fpga_number[get_init_val(param)];
  unsigned int i;
  for (i=9; 0 <= i; --i) dot_matrix_addr[i] = number[i];
}

/**
 * dot_matrix_exit - initializes @dot_matrix_addr to zero value
 *
 * @dot_matrix_addr: the address of Dot Matrix device
 */
inline void dot_matrix_exit(unsigned char *dot_matrix_addr) {
  unsigned int i;
  for (i=9; 0 <= i; --i) dot_matrix_addr[i] = 0x00;
}