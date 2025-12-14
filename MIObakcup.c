#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pwm.pio.h"
#include "SSD1306.h"
#define CLOCK_FREQ 125000000
#define PWM_FREQ   50

// pump code
void pwm_program_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_gpio_init(pio, pin); // gpio setup so this pin controlled by PIO 
    // (PIO0, sm0, PIN1, number of pins, true= out, false = in) 
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true); 
    // -> c is used as config memory
    pio_sm_config c = pwm_program_get_default_config(offset);
    // very important!, this assingns the pins to the pio wihtout this nothings happens
    // even when the program is running 
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_clkdiv(&c, 1000.0f);  // Run at 125kHz
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}
void pwm_set_period(PIO pio, uint sm, uint period) {
    pio_sm_set_enabled(pio, sm, false); // disable pio
    // pio_sm_put_blockng sends a value to TX fifo (32 but )
    // this is just memory and stays there until its pulled by PIO
    pio_sm_put_blocking(pio, sm, period);
    // pio_encode_pull(false, false) <-- NON-BLOCKING & ALWAYS TO OSR
    pio_sm_exec(pio, sm, pio_encode_pull(false, false));
    // pio_encode_out(pio_isr, 32) move 32 bytes to ISR from OSR
    pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
    pio_sm_set_enabled(pio, sm, true);
}
void pwm_set_duty(PIO pio, uint sm, uint level) {
    pio_sm_put_blocking(pio, sm, level);
}
void pwm_pulse(PIO pio, uint sm, uint pin, uint offset, uint duty, uint period) {
    pwm_program_init(pio, sm, offset, pin);
    pwm_set_period(pio, 0, period);
}
// main file has the order of in/out logic

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &pwm_program);
    uint sm = 0;
    uint pin = 5;
    uint period = CLOCK_FREQ / (PWM_FREQ * 20000);
    uint duty = period * 0.5;
    uint duration_ms = 1400;
    pwm_pulse(pio, sm, pin, offset, duty, period);
    while(1){
        const float conversion_factor = 3.3f / (1 << 12);
        float valu = 10;
        oled_print_value(valu);
        sleep_ms(10);
        uint16_t result = adc_read();
        uint sm = 0;
        uint pin = 5;
        uint period = CLOCK_FREQ / (PWM_FREQ * 20000);
        uint level = period * 0.5;
        pio_sm_put_blocking(pio, sm, level);
        printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
        sleep_ms(10000);
    }
}
