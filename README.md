#### Embedded system software Homework #1



- Before run the application, you should activate `/root/.bashrc` and insert modules.

  ```bash
  $ sudo -s
  $ source /root/.bashrc
  $ sh fpga_insmod.sh
  ```

  

- To run the application, do

  ```bash
  # This command generates an execution file named 'app'
  $ make
  $ adb push app /data/local/tmp
  
  # Go to /data/local/tmp of your device
  $ ./app
  ```

  

- To clean the outputs, do

  ```bash
  $ make clean
  ```