#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H
#include <stdio.h>
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "pwm.pio.h"


#define CLOCK_FREQ 125000000  // RP2040 clock frequency (125 MHz)
#define PWM_FREQ 50           // PWM frequency in Hz
#define ADC_MAX 4000          // ADC maximum value
#define NUM_SM 4              // Number of state machines (4)
#define NUM_CHANNELS 4        // Number of ADC channels (3)
#define PIO_NUMBER 2

// PWM Pin Assignments
extern const uint PWM_PINS_PIO0[NUM_SM];
extern const uint PWM_PINS_PIO1[NUM_SM];


// Function Prototypes
void calculate_constants(float *cv, uint16_t *l_const, uint16_t *period);
void pio_pwm_set_period(PIO pio, uint sm, uint32_t Period);
void set_pwm_program(PIO pio, uint sm, uint offset, uint gpio, uint16_t Period);
void run_pwm(uint16_t pwm0 , uint16_t pwm1, uint16_t pwm2, uint16_t pwm3, uint16_t pwm4, uint16_t pwm5, uint16_t pwm6, uint16_t pwm7);

#endif // PWM_CONTROL_H