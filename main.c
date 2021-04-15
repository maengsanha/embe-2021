#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#include "device_status.h"

int main() {
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
    
    // attatch to shared memory
    struct device_status *status = shmat(shm_id, NULL, 0);
    
    
    for (;;) {
      // send input signal to main process
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
      
      // attatch to shared memory
      struct device_status *status = shmat(shm_id, NULL, 0);
      
      for (;;) {
        // send out result to devices
      }

      // detach from shared memory
      shmdt(status);

    } else {
      ///////////////////////////// Main process /////////////////////////////
      
      // attatch to shared memory
      struct device_status *status = shmat(shm_id, NULL, 0);
      
      for (;;) {
        // do something
      }
      
      // detach from shared memory
      shmdt(status);
      
      // wait other processes
      wait(NULL);
      wait(NULL);

      // release shared memory
      shmctl(shm_id, IPC_RMID, NULL);

    }
  }
}