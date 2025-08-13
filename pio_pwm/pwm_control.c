#include "pwm_control.h"

const uint PWM_PINS_PIO0[NUM_SM] = {2, 3, 4, 5};
const uint PWM_PINS_PIO1[NUM_SM] = {6, 7, 8, 9};



//calculate constants from herts and duty cycles
void calculate_constants(float *cv, uint16_t *l_const, uint16_t *period) {
    *period = (1.0 / PWM_FREQ) * CLOCK_FREQ / (1000 * 3);
    *l_const = (1.0 / PWM_FREQ) * 0.05 * CLOCK_FREQ / (1000 * 3);
    *cv = (float)(*l_const)/ADC_MAX ;
}

void pio_pwm_set_period(PIO pio, uint sm, uint32_t Period) {
    pio_sm_set_enabled(pio, sm, false);               // Disable the state machine
    pio_sm_put_blocking(pio, sm, Period);             // Set the PWM period
    pio_sm_exec(pio, sm, pio_encode_pull(false, false)); // Pull period into OSR
    pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));   // Load OSR into ISR
    pio_sm_set_enabled(pio, sm, true);                // Re-enable the state machine
}

void set_pwm_program(PIO pio, uint sm, uint offset, uint gpio, uint16_t Period) {
    pwm_program_init(pio, sm, offset, gpio);
    float clock_divider = 1000.0f;  // Set clock divider
    pio_sm_set_clkdiv(pio, sm, clock_divider);
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_put_blocking(pio, sm, Period); // Set PWM period
    pio_sm_exec(pio, sm, pio_encode_pull(false, false));
    pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
    pio_sm_set_enabled(pio, sm, true);
}



void run_pwm(uint16_t pwm0 , uint16_t pwm1, uint16_t pwm2, uint16_t pwm3, uint16_t pwm4, uint16_t pwm5, uint16_t pwm6, uint16_t pwm7) {
    // Initialize the standard IO for printf Initialize ADC

    uint16_t pwm_PIO0[4] = {pwm0, pwm1, pwm2, pwm3};
    uint16_t pwm_PIO1[4] = {pwm4, pwm5, pwm6, pwm7};
    float cv;  // constant v 
    uint16_t l_const, period; // l const and period declaration 
    calculate_constants(&cv, &l_const, &period); //here theire defined 
    int sm[NUM_SM] = {0,1,2,3}; // define state machine
    PIO pio[2] = {pio0, pio1}; // define PIO


    // load the program and pio 
    uint offset0 = pio_add_program(pio[0], &pwm_program);
    uint offset1 = pio_add_program(pio[1], &pwm_program);
    printf("Loaded program at %d\n", offset0);
    printf("Loaded program at %d\n", offset1);


    // Call the function correctly
    // Initialize multiple PIO state machines using a loop
    // Initialize state machines for both PIOs
    for (int i = 0; i < NUM_SM; i++) {
        set_pwm_program(pio[0], sm[i], offset0, PWM_PINS_PIO0[i], period);
        set_pwm_program(pio[1], sm[i], offset1, PWM_PINS_PIO1[i], period);
    }

    while (true) {
        // Set PWM level for each state machine
        for (int i = 0; i < PIO_NUMBER; i++) { // there is 2 pio 
            if (i == 0) {
                PIO selected_pio = pio[i];
                for (int j = 0; j < NUM_SM; j++) {
                    uint16_t level = pwm_PIO0[j] * cv;
                    pio_sm_put_blocking(selected_pio, sm[j], level + l_const);
                }
            }
            if (i == 1) {
                PIO selected_pio = pio[i];
                for (int k = 0; k < NUM_SM; k++) {
                uint16_t level = pwm_PIO1[k] * cv;
                pio_sm_put_blocking(selected_pio, sm[k], level + l_const);
                }  
            }
        }
        sleep_ms(20);
    }
}