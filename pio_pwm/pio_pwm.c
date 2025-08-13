#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pwm_control.h"

// Constants
#define CLOCK_FREQ 125000000  // RP2040 clock frequency (125 MHz)
#define PWM_FREQ 50           // PWM frequency in Hz
#define ADC_MAX 4000          // 12-bit ADC
#define NUM_SM 4              // Number of state machines per PIO
#define PIO_NUMBER 2          // We have pio0 and pio1

// Globals for constants
static float cv;
static uint16_t l_const, period;

int main() {
    stdio_init_all();

    // Init ADC pins
    adc_init();
    adc_gpio_init(26); // ADC0 -> GPIO26
    adc_gpio_init(27); // ADC1 -> GPIO27
    adc_gpio_init(28); // ADC2 -> GPIO28

    // Calculate PWM constants
    calculate_constants(&cv, &l_const, &period);
    // PIO setup
    PIO pios[2] = {pio0, pio1};
    int sms[NUM_SM] = {0, 1, 2, 3};

    uint offset0 = pio_add_program(pio0, &pwm_program);
    uint offset1 = pio_add_program(pio1, &pwm_program);

    for (int i = 0; i < NUM_SM; i++) {
        set_pwm_program(pio0, sms[i], offset0, PWM_PINS_PIO0[i], period);
        set_pwm_program(pio1, sms[i], offset1, PWM_PINS_PIO1[i], period);
    }

    // Main loop
    while (true) {
        uint16_t PWM_vals[8] = {0};

        // Read ADCs
        adc_select_input(0);
        PWM_vals[0] = adc_read();

        adc_select_input(1);
        PWM_vals[1] = adc_read();

        adc_select_input(2);
        PWM_vals[2] = adc_read();

        // Duplicate values for unused channels
        PWM_vals[3] = PWM_vals[0];
        PWM_vals[4] = PWM_vals[1];
        PWM_vals[5] = PWM_vals[2];
        PWM_vals[6] = PWM_vals[0];
        PWM_vals[7] = PWM_vals[1];

        // Update PWM duty levels (no reinit)
        for (int j = 0; j < NUM_SM; j++) {
            uint16_t level0 = (uint16_t)(l_const + cv * PWM_vals[j]);
            pio_sm_put_blocking(pio0, sms[j], level0);

            uint16_t level1 = (uint16_t)(l_const + cv * PWM_vals[j + 4]);
            pio_sm_put_blocking(pio1, sms[j], level1);
        }

        sleep_ms(20);
    }
}