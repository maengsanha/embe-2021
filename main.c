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

#define FND_DEVICE        "/dev/fpga_fnd"
#define TEXT_LCD_DEVICE   "/dev/fpga_text_lcd"
#define DOT_MATRIX_DEVICE "/dev/fpga_dot"
#define SWITCH_DEVICE     "/dev/fpga_push_switch"
#define READKEY_DEVICE    "/dev/input/event0"
#define MEM_DEVICE        "/dev/mem"
#define LED_BASE_ADDRESS  0x08000000
#define LED_OFFSET        0x16

int main() {
  int fnd_fd,
      text_lcd_fd,
      dot_matrix_fd,
      switch_fd,
      readkey_fd,
      led_fd;
  unsigned long *fpga_addr;
  unsigned char *led_addr;

  // open devices
  if ((fnd_fd = open(FND_DEVICE, O_WRONLY)) < 0) {
    printf("open FND failed\n");
    close(fnd_fd);
    return 1;
  }
  if ((text_lcd_fd = open(TEXT_LCD_DEVICE, O_WRONLY)) < 0) {
    printf("open Text LCD failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    return 1;
  }
  if ((dot_matrix_fd = open(DOT_MATRIX_DEVICE, O_WRONLY)) < 0) {
    printf("open Dot Matrix failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    return 1;
  }
  if ((switch_fd = open(SWITCH_DEVICE, O_RDONLY)) < 0) {
    printf("open Switch failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    close(switch_fd);
    return 1;
  }
  if ((readkey_fd = open(READKEY_DEVICE, O_RDONLY)) < 0) {
    printf("open READ KEY failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    close(switch_fd);
    close(readkey_fd);
    return 1;
  }
  if ((led_fd = open(MEM_DEVICE, O_RDWR | O_SYNC)) < 0) {
    printf("open LED failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    close(switch_fd);
    close(readkey_fd);
    close(led_fd);
    return 1;
  }

  if ((fpga_addr = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, led_fd, LED_BASE_ADDRESS)) == MAP_FAILED) {
    printf("mmap failed\n");
    close(fnd_fd);
    close(text_lcd_fd);
    close(dot_matrix_fd);
    close(switch_fd);
    close(readkey_fd);
    close(led_fd);
    return 1;
  }

  led_addr = (unsigned char *)((void *)fpga_addr+LED_OFFSET);

  // get shared memory
  int shm_id;
  if ((shm_id = shmget(IPC_PRIVATE, sizeof(struct device_status), IPC_CREAT | 0644)) == -1) {
    printf("shmget failed\n");
    return 1;
  }

  pid_t input;
  if ((input = fork()) == -1) {
    printf("fork failed\n");
    return 1;
  } else if (input == 0) {
    ///////////////////////////// Input process /////////////////////////////
    
    // attatch to shared memory and intialize device status
    struct device_status *status = shmat(shm_id, NULL, 0);
  
    for (;;) {
      // read signals
      read(readkey_fd, status->readkey_val, sizeof(status->readkey_val));
      read(switch_fd, status->switch_val, sizeof(status->switch_val));
      
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
      printf("fork failed\n");
      return 1;
    } else if (output == 0) {
      ///////////////////////////// Output process /////////////////////////////
      
      // attatch to shared memory and initialize status
      struct device_status *status = shmat(shm_id, NULL, 0);
      
      for (;;) {
        // if BACK key was pressed, end program
        if (status->readkey_val[0].code == BACK) break;

        // write to device
        write(fnd_fd, status->fnd_val, 4);
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
      status->mode = CLOCK_MODE;
      init_status(status);

      unsigned int current_mode = status->mode;

      for (;;) {
        // if BACK key was pressed, end program
        if (status->readkey_val[0].code == BACK) break;

        // if the mode has been changed, set device status to zero values
        if (current_mode != status->mode) {
          current_mode = status->mode;
          switch (status->mode) {
          case CLOCK_MODE:
            init_clock(status);
            break;
          case COUNTER_MODE:
            init_counter(status);
            break;
          case TEXT_EDITOR_MODE:
            init_text_editor(status);
            break;
          case DRAW_BOARD_MODE:
            init_draw_board(status);
            break;
          }
        }

        // set status
        switch (status->mode) {
        case CLOCK_MODE:
          handle_clock(status);
          break;
        case COUNTER_MODE:
          handle_counter(status);
          break;
        case TEXT_EDITOR_MODE:
          handle_text_editor(status);
          break;
        case DRAW_BOARD_MODE:
          handle_draw_board(status);
          break;
        }

        usleep(500000);
      }

      // reinitialize device when program ends
      init_status(status);
      
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