#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pwm.pio.h"
#include "SSD1306.h"

#define ON 27

#define LED_0 8
#define LED_1 7
#define LED_2 6
#define LED_3 5
#define LED_4 4
#define LED_5 3

void pwm_program_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = pwm_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_clkdiv(&c, 10.0f); 
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}
void pwm_set_period(PIO pio, uint sm, uint period) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_put_blocking(pio, sm, period);
    pio_sm_exec(pio, sm, pio_encode_pull(false, false));
    pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
    pio_sm_set_enabled(pio, sm, true);
}

void led(int l0, int l1, int l2, int l3, int l4, int l5){
    gpio_put(LED_0, l0);
    gpio_put(LED_1, l1);
    gpio_put(LED_2, l2);
    gpio_put(LED_3, l3);
    gpio_put(LED_4, l4);
    gpio_put(LED_5, l5);
}

void led_set(uint8_t led_pos){
    uint8_t led_ind = led_pos/ (100/6);
    if (led_ind > 6) led_ind = 6; 
    switch(led_ind){
        case 0: led(0,0,0,0,0,0); break;
        case 1: led(1,0,0,0,0,0); break;
        case 2: led(1,1,0,0,0,0); break;
        case 3: led(1,1,1,0,0,0); break;
        case 4: led(1,1,1,1,0,0); break;
        case 5: led(1,1,1,1,1,0); break;
        case 6: led(1,1,1,1,1,1); break;
    }
}

int main() {

    stdio_init_all();
    oled_init();

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    gpio_init(ON);
    gpio_set_dir(ON, GPIO_IN);

    gpio_init(LED_0);
    gpio_set_dir(LED_0, GPIO_OUT);
    gpio_init(LED_1);
    gpio_set_dir(LED_1, GPIO_OUT);
    gpio_init(LED_2);
    gpio_set_dir(LED_2, GPIO_OUT);
    gpio_init(LED_3);
    gpio_set_dir(LED_3, GPIO_OUT);
    gpio_init(LED_4);
    gpio_set_dir(LED_4, GPIO_OUT);
    gpio_init(LED_5);
    gpio_set_dir(LED_5, GPIO_OUT);
    
    int sm = 0;
    int pin = 2;
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &pwm_program);
    uint32_t period = 5000;//25ms
    pwm_program_init(pio, sm, offset, pin);
    pwm_set_period(pio, sm, period);
    bool active = 0;
  
    while (1) {
        uint16_t adc_val = adc_read();       
        uint16_t percent = adc_val / 40;
        if (percent > 100) percent = 100;

            oled_print_value_scaled(percent);

        if (gpio_get(ON)) {
            active = 1;
        } else {
            active = 0;
            led_set(0);
            pio_sm_put_blocking(pio, sm, 0); 
        }
        if (active){ 
            pio_sm_put_blocking(pio, sm, percent * 50);  
            pio_sm_set_enabled(pio0, sm, true);
            led_set(percent);
        }
        sleep_ms(10);
    }
}

