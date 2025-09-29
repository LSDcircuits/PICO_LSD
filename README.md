# PICO_LSD
low level repository containing functions and libraries to be used on the RP2040

1. timer functions c


this file contains functions used for header files or funcs to use on the main script, its 
quite easy to use its based on the rp2040 64 bit timer defined in section (7.6) on datasheet.
this func calls the timer regiter values, can be used to find the time difference between measurements
or generl time based on the clock cycle (245mhz). 

3. PIO_PWM

In this file i make use of the PIO blocks from the PICO (check 1.3 RPI RP2040 datashet) for adding 8 additional PWM
ouput pins which take minimal ammount of CPU clock cylces to keep running parallel to Core. useful for:

- Time sensitive PWM signaling for real time operation such as for drone ESC's or controlling stepper drivers.
- To add extra PWM outputs, the RP2040 has 16 PWM out pins, adding these puts it to 24.

2. 7789_LSD c

his file contains libraries used to drive LCD screens which use the S7789 driver for graphics, its used to present
characters and can be useful to monior readings from a device, (working on a UI lib next).

3. lsm6s3_lsd c

this file contains the logic used to get raw data from this IMU. working on:

- making a library
- getting different power modes and sensitivity
- applying kallman filter & quaternions as additional fucntions


4. lsd_ultraS c

this file is used to make a ultrasonic sensor using the pico ADC,
pio is set:
make ISR define 8 pulses 
make the ISR loop count down and - each iteration till 0 then stop 
till isr is loaded again. 

