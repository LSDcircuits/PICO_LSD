#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pwm0.pio.h"
#include "pwm1.pio.h"

#define ON 26

#define pwm_heat 1
#define heat_period 5000

#define pwm_mtr 2
#define mtr_period 5000


// pio program start
void pwm_program_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = pwm_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_clkdiv(&c, 10.0f); 
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

// pio period set 
void pwm_set_period(PIO pio, uint sm, uint period) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_put_blocking(pio, sm, period);
    pio_sm_exec(pio, sm, pio_encode_pull(false, false));
    pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
    pio_sm_set_enabled(pio, sm, true);
}


// heat + mix 
void set_heat_mix() {

    stdio_init_all();
    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);
    gpio_init(ON);
    gpio_set_dir(ON, GPIO_IN);

    // PIO block setup
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &pwm_program);

    // Heat PWM setup
    int sm0 = 0;
    int pin0 = pwm_heat;
    uint32_t period0 = heat_period;
    pwm_program_init(pio, sm0, offset, pin0);
    pwm_set_period(pio, sm0, period0);

    // motor PWM setup
    int sm1 = 1;
    int pin1 = pwm_mtr;
    uint32_t period1 = mtr_period;
    pwm_program_init(pio, sm1, offset, pin1);
    pwm_set_period(pio, sm1, period1);
   
    bool active = 1;

    while (1) {
        

        // temporary just to setupn a variable pwm duty
        // implement memory management to add PID loop once circit completed
        uint16_t adc_val = adc_read();  
        uint16_t percent = adc_val / 40;


        if (percent > 100) percent = 100;

        if (gpio_get(ON)) {
            active = 1;
        } else {
            active = 0;
            pio_sm_put_blocking(pio, sm0, 0); 
            pio_sm_put_blocking(pio, sm1, 0); 
        }
        if (active){ 

            pio_sm_put_blocking(pio, sm1, percent * 50);
            pio_sm_set_enabled(pio, sm1, true);

            pio_sm_put_blocking(pio, sm0, percent * 50);
            pio_sm_set_enabled(pio, sm0, true);
        }
        sleep_ms(10);
    }
}

int main(){
    set_heat_mix();
}