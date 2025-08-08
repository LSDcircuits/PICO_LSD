# PICO_LSD
low level repository containing functions and libraries to be used on the RP2040

1. timer functions c


this file contains functions used for header files or funcs to use on the main script, its 
quite easy to use its based on the rp2040 64 bit timer defined in section (7.6) on datasheet.
this func calls the timer regiter values, can be used to find the time difference between measurements
or generl time based on the clock cycle (245mhz). 

2. 7789_LSD c

his file contains libraries used to drive LCD screens which use the S7789 driver for graphics, its used to present
characters and can be useful to monior readings from a device, (working on a UI lib next).

3. PIO_PWM

s
