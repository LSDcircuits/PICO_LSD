PICO_LSD
Repository containing functions and libraries to be used on the RP2040

1. timer functions c

this file contains functions used for header files or funcs to use on the main script, its 
quite easy to use its based on the rp2040 64 bit timer defined in section (7.6) on datasheet.
this func calls the timer regiter values, can be used to find the time difference between measurements
or general time based on the clock cycle (125mhz). 
same as timer function, the lib is a good example of the usage of memory i/o.

3. PIO_PWM (in maintenance) 

In this file i make use of the PIO blocks from the PICO (check 1.3 RPI RP2040 datashet) for adding 8 additional PWM
ouput pins which take minimal ammount of CPU clock cylces to keep running parallel to Core. useful for:

- Time sensitive PWM signaling for real time operation such as for drone ESC's or controlling stepper drivers.
- To add extra PWM outputs, the RP2040 has 16 PWM out pins, adding these puts it to 24.

2. SSD1306_LSD

This file contains libraries used to drive LCD screens which use the SSD1306 driver for graphics, its used to present
characters and can be useful to monior readings from a device. Still alot can be dont to this library before its comple
neveheless its still useful Already. 

works for: 
- printing chars
- printing numbers

3. lsm6s3_lsd c

This file contains the logic used to get raw data from this IMU. working on: (im fustrated with my self i havent finished it :( )

- using structures for data. 
- making a library
- getting different power modes and sensitivity
- applying quaternions & other rotation methods
- making expandable for other usage

4. lsd_pid (im fustrated with my self i havent finished it :( ) same here bb 

this file contains PID loops, from a simple pid loop with only variable input to PID with expandable struct data. this can be used for the temperature controll of a hotplate or to balance a device. (in progress with the PWM lib).




