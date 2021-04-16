#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "clock.h"
#include "counter.h"
#include "text_editor.h"
#include "draw_board.h"

const char *FND_DEVICE              = "/dev/fpga_fnd";
const char *TEXT_LCD_DEVICE         = "/dev/fpga_text_lcd";
const char *DOT_MATRIX_DEVICE       = "/dev/fpga_dot";
const char *SWITCH_DEVICE           = "/dev/fpga_push_switch";
const char *READKEY_DEVICE          = "/dev/input/event0";
const char *MEM_DEVICE              = "/dev/mem";
const unsigned int LED_BASE_ADDRESS = 0x08000000;
const unsigned int LED_OFFSET       = 0x16;
const unsigned int VOL_PLUS         = 0x73;
const unsigned int VOL_MINUS        = 0x72;
const unsigned int BACK             = 0x9E;
const unsigned int KEY_RELEASE      = 0x00;
const unsigned int KEY_PRESS        = 0x01;

int main() {
  int fnd_fd,
      text_lcd_fd,
      dot_matrix_fd,
      switch_fd,
      readkey_fd,
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
    close(fnd_fd);
    close(text_lcd_fd);
    return 1;
  }
  if ((dot_matrix_fd = open(DOT_MATRIX_DEVICE, O_RDWR)) < 0) {
    perror("open Dot Matrix failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    return 1;
  }
  if ((switch_fd = open(SWITCH_DEVICE, O_RDONLY)) < 0) {
    perror("open Switch failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    close(switch_fd);
    return 1;
  }
  if ((readkey_fd = open(READKEY_DEVICE, O_RDONLY | O_NONBLOCK)) < 0) {
    perror("open READ KEY failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    close(switch_fd);
    close(readkey_fd);
    return 1;
  }
  if ((led_fd = open(MEM_DEVICE, O_RDWR | O_SYNC)) < 0) {
    perror("open LED failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    close(switch_fd);
    close(readkey_fd);
    close(led_fd);
    return 1;
  }

  if ((fpga_addr = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, led_fd, LED_BASE_ADDRESS)) == MAP_FAILED) {
    perror("mmap failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    close(switch_fd);
    close(readkey_fd);
    close(led_fd);
    return 1;
  }

  led_addr = (unsigned long *)((void *)fpga_addr+LED_OFFSET);

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
      read(readkey_fd, status->readkey_val, sizeof(status->readkey_val));
      read(switch_fd, &status->switch_val, sizeof(status->switch_val));
      
      // set status
      
      // if BACK key was pressed, end program
      if (status->readkey_val[0].code == BACK && status->readkey_val[0].value == KEY_PRESS) break;

      // if Volume key was pressed, change mode
      if (status->readkey_val[0].code == VOL_PLUS && status->readkey_val[0].value == KEY_PRESS) {
        status->mode = (status->mode + 1) % 4;
      } else if (status->readkey_val[0].code == VOL_MINUS && status->readkey_val[0].value == KEY_PRESS) {
        status->mode = (status->mode + 3) % 4;
      }
      
      usleep(500000);
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
        // if BACK key was pressed, end program
        if (status->readkey_val[0].code == BACK) break;

        printf("current FND value: %d%d%d%d\n", status->fnd_val[0], status->fnd_val[1], status->fnd_val[2], status->fnd_val[3]);

        // write to device
        write(fnd_fd, &status->fnd_val, 4);
        write(text_lcd_fd, status->text_lcd_val, 32);
        write(dot_matrix_fd, status->dot_matrix_val, sizeof(status->dot_matrix_val));
        *led_addr = status->led_val;

        usleep(500000);
      }

      // detach from shared memory
      shmdt(status);

    } else {
      ///////////////////////////// Main process /////////////////////////////
      
      // attatch to shared memory and initialize status
      struct device_status *status = shmat(shm_id, NULL, 0);

      // the default mode of the device is Clock
      init_status(status, CLOCK_MODE);

      unsigned int current_mode = status->mode;

      for (;;) {
        // if BACK key was pressed, end program
        if (status->readkey_val[0].code == BACK) break;

        // if the mode has been changed, set device status to zero values
        if (current_mode != status->mode) {
          current_mode = status->mode;
          init_status(status, status->mode);
          printf("mode: %d\n", status->mode+1);
        }

        // set status
        switch (status->mode) {
        case 0:
          // clock(status);
          break;
        case 1:
          // counter(status);
          break;
        case 2:
          // text_editor(status);
          break;
        case 3:
          // draw_board(status);
          break;
        default:  // no such case
          break;
        }

        usleep(500000);
      }
      
      // detach from shared memory
      shmdt(status);
      
      // wait other processes
      wait(NULL);
      wait(NULL);

      // release shared memory
      shmctl(shm_id, IPC_RMID, NULL);

      // close device drivers
      close(readkey_fd);
      close(switch_fd);
      close(fnd_fd);
      close(text_lcd_fd);
      close(dot_matrix_fd);

      // close memory mapped device
      munmap(led_addr, 0x1000);
      close(led_fd);
    }
  }
}