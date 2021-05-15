/*
 * Embedded System Software, 2021
 *
 * fpga_fnd.h - FPGA FND device utility functions definition
 */
#include "args.h"

/**
 * fnd_init - initializes @fnd_addr to @init of @param
 *
 * @fnd_addr: the address of FND device
 * @param:    command line argument from user program
 */
inline void fnd_init(unsigned char *fnd_addr, struct args *param);

/**
 * fnd_exit - initializes @fnd_addr to zero value
 *
 * @fnd_addr: the address of FND device
 */
inline void fnd_exit(unsigned char *fnd_addr);