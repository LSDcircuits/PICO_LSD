PICO_LSD
Repository containing functions and libraries to be used on the RP2040 // note for self: seperate projecta from libs, then ue the libs in projects. 

1. timer functions c

- Call 64bit counter for timing
- 125MHZ 

3. PIO_PWM

Ouptut_PWM
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

4. lsd_pid  

this file contains PID loops, from a simple pid loop with only variable input to PID with expandable struct data. this can be used for the temperature controll of a hotplate or to balance a device. (in progress with the PWM lib using PIO_ASM for direct use on pwm).

to be implemented:
- tuning using root locus
- PIO_ASM based for low freq PD controller / figuring out boolean operations for the limited memory space. 


5. PICO stepper

- used for controlling stepper motors for motion.
- includes schematic example for the LD293D


