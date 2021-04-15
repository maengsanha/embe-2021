#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "device_status.h"

const char *FND_DEVICE              = "/dev/fpga_fnd";
const char *TEXT_LCD_DEVICE         = "/dev/fpga_text_lcd";
const char *DOT_MATRIX_DEVICE       = "/dev/fpga_dot";
const char *SWITCH_DEVICE           = "/dev/fpga_push_switch";
const char *MEM_DEVICE              = "/dev/mem";
const unsigned int LED_BASE_ADDRESS = 0x08000000;
const unsigned int LED_OFFSET       = 0x16;


int main() {
  int fnd_fd,
      text_lcd_fd,
      dot_matrix_fd,
      switch_fd,
      led_fd;
  unsigned long *fpga_addr,
                *led_addr;

  // open devices
  if ((fnd_fd = open(FND_DEVICE, O_RDWR)) < 0) {
    perror("open FND failed\n");
    close(fnd_fd);
    return 1;
  }
  if ((text_lcd_fd = open(TEXT_LCD_DEVICE, O_RDWR)) < 0) {
    perror("open Text LCD failed\n");
    close(text_lcd_fd);
    return 1;
  }
  if ((dot_matrix_fd = open(DOT_MATRIX_DEVICE, O_RDWR)) < 0) {
    perror("open Dot Matrix failed\n");
    close(dot_matrix_fd);
    return 1;
  }
  if ((switch_fd = open(SWITCH_DEVICE, O_RDONLY)) < 0) {
    perror("open Switch failed\n");
    close(switch_fd);
    return 1;
  }
  if ((led_fd = open(MEM_DEVICE, O_RDWR | O_SYNC)) < 0) {
    perror("open LED failed\n");
    close(led_fd);
    return 1;
  }

  if ((fpga_addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, led_fd, LED_BASE_ADDRESS)) == MAP_FAILED) {
    perror("mmap failed\n");
    close(led_fd);
    return 1;
  }

  led_addr = (unsigned char *)((void *)fpga_addr+LED_OFFSET);




  // get shared memory
  int shm_id;
  if ((shm_id = shmget(IPC_PRIVATE, sizeof(struct device_status), IPC_CREAT | 0644)) == -1) {
    perror("shmget failed\n");
    return 1;
  }

  pid_t input;
  if ((input = fork()) == -1) {
    perror("fork failed\n");
    return 1;
  } else if (input == 0) {
    ///////////////////////////// Input process /////////////////////////////
    
    // attatch to shared memory and intialize device status
    struct device_status *status = shmat(shm_id, NULL, 0);
    
    
    for (;;) {
      // read signals

      // check mode

      // set status

      
      usleep(400000);
    }

    // detach from shared memory
    shmdt(status);
  
  } else {

    pid_t output;
    if ((output = fork()) == -1) {
      perror("fork failed\n");
      return 1;
    } else if (output == 0) {
      ///////////////////////////// Output process /////////////////////////////
      
      // attatch to shared memory and initialize status
      struct device_status *status = shmat(shm_id, NULL, 0);
      
      for (;;) {
        // check mode


        // write to device
        write(fnd_fd, status->fnd_val, 4);
        write(text_lcd_fd, status->text_lcd_val, 32);
        write(dot_matrix_fd, status->dot_matrix_val, sizeof(status->dot_matrix_val));
        *(status->led_addr) = status->led_val;

        usleep(400000);
      }

      // detach from shared memory
      shmdt(status);

    } else {
      ///////////////////////////// Main process /////////////////////////////
      
      // attatch to shared memory and initialize status
      struct device_status *status = shmat(shm_id, NULL, 0);
      init_device(status, fnd_fd, text_lcd_fd, switch_fd, dot_matrix_fd, led_addr);
      
      for (;;) {
        // check mode

        // set status



        usleep(400000);
      }
      
      // detach from shared memory
      shmdt(status);
      
      // wait other processes
      wait(NULL);
      wait(NULL);

      // release shared memory
      shmctl(shm_id, IPC_RMID, NULL);

      // close device drivers
      close(fnd_fd);
      close(text_lcd_fd);
      close(dot_matrix_fd);
      close(switch_fd);

      // close memory mapped device
      munmap(led_addr, 4096);
      close(led_fd);
    }
  }
}