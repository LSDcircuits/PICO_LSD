#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "pulse.pio.h"
#include "pico_timer.h"
#define min_v 9

void pulse_setup(){
    // PIO setup
    const uint PIN_BASE = 28;
    const float CLKDIV = 625.f;  // ≈ 40kHz
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &pulse_program);
    pulse_program_init(pio, sm, offset, PIN_BASE, CLKDIV);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);
    pio_sm_put_blocking(pio, sm, 8);  // Send pulse pairs
    pio_sm_set_enabled(pio, sm, true);
    //printf("Pulses triggered...\n");
    uint32_t done = pio_sm_get_blocking(pio, sm);
    //printf("Pulse sequence completed. PIO response: %u\n", done);
}


uint16_t read_stable_adc(int samples) {
    uint32_t sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += adc_read();
    }
    return sum / samples;
}
int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(26); 
    adc_select_input(0);

    while (1) {
        uint64_t t1 = 0;
        uint8_t raw = 10;
        const int max_samples = 20000;
        pulse_setup();
        uint64_t t0 = read_timer_raw_macro();
        for (int i = 0; i < max_samples; i++){
            raw = read_stable_adc(2);
            //sleep_us(1);
            //printf("raw value = %u\n", raw);
            if(raw < min_v){
                t1 = read_timer_raw_macro();
                //printf("Threshold crossed: raw=%u at dt=%llu us\n", raw, (t1 - t0));
                float dt = (t1 - t0 - 200);
                float distance = (dt * 0.0344)/2;
                printf("distance = %.2f cm\n", distance);
                break;
            }
        }
        if (t1 == 0){
            //printf("No threshold crossing detected\n");
        }
        sleep_ms(10);
    }
}


// #include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pulse.pio.h"

// counter but i call it timer since i use it as timer ..// registers :)
#define TL (*(volatile uint32_t*)(0x40054024))
#define TH (*(volatile uint32_t*)(0x40054028))

// GPIO REGISTERS :) 
#define GPIO_IN      (*(volatile uint32_t *)(SIO_BASE + 0x04))

// its 64 bit and needs to be biffered into a uint64 
// dereferenced register is assinged to low and high
// (64bit)  (32 bit high << 32 bit low) 
uint64_t read_timer_raw_macro2() {
    uint32_t hi1, lo, hi2;
    do {
        hi1 = timer_hw->timerawh;
        lo = timer_hw->timerawl;
        hi2 = timer_hw->timerawh;
    } while (hi1 != hi2);
    return ((uint64_t)hi1 << 32) | lo;
}

// Reads pin level (returns 1 for HIGH, 0 for LOW)
uint8_t gpio_in_mmio(uint8_t pin) {
    return (GPIO_IN >> pin) & 1u;
}

// pio initiation
void pulse_program_init(PIO pio, uint sm, uint offset, uint pin_base, float clkdiv) {
    pio_gpio_init(pio, pin_base); // gpio setup so this pin controlled by PIO 
    pio_gpio_init(pio, pin_base + 1); // gpio pin 2, pin 1 + 1 = 2

    // (PIO0, sm0, PIN1, number of pins, true= out, false = in) 
    pio_sm_set_consecutive_pindirs(pio, sm, pin_base, 2, true);

    // -> c is used as config memory
    pio_sm_config c = pulse_program_get_default_config(offset);

    // very important!, this assingns the pins to the pio wihtout this nothings happens
    // even when the program is running 
    sm_config_set_sideset_pins(&c, pin_base);

    // clock devider
    sm_config_set_clkdiv(&c, clkdiv);

    // here the config is loaded
    pio_sm_init(pio, sm, offset, &c);
}

void pulse_setup(){
    // PIO setup
    const uint PIN_BASE = 28;
    const float CLKDIV = 625.f;  
    PIO pio = pio0;
    uint sm = 0;
    
    // adress instruction memory where pulse_program is loaded
    uint offset = pio_add_program(pio, &pulse_program);

    // here the vaues are sent to the init from offset --> &c 
    pulse_program_init(pio, sm, offset, PIN_BASE, CLKDIV);

    // clear any fifos (OSR,ISR)
    pio_sm_clear_fifos(pio, sm);

    // restart the PIO0 & statemachine
    pio_sm_restart(pio, sm);

    // pio_sm_put_blockng sends a value to TX fifo (32 but queue)
    // this is just memory and stays there until its pulled by PIO
    pio_sm_put_blocking(pio, sm, 8);  

    // this starts the PIO SM
    pio_sm_set_enabled(pio, sm, true);
    // note this is a interrupt not a IQR interrupt
    // assgings a uint32 in a fucntion so functions wont end till its assgined
    // so fucntion ends when PIO program finishes. 8 40khz pulses in this case 
    uint32_t done = pio_sm_get_blocking(pio, sm);
}

int main() {
    stdio_init_all();
    const uint8_t ECHO_PIN = 26;
    const double SOUND_SPEED_CM_PER_US = 0.0344;
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    while (1) {
        // Send the pulse
        pulse_setup();
        uint64_t t0 = read_timer_raw_macro2();  // Immediately after pulse sent
        // Wait for echo rising edge (GPIO goes HIGH)
        uint64_t timeout = t0 + 50000;  // 50 ms timeout
        uint64_t t1 = 0;
        while (gpio_in_mmio(ECHO_PIN) == 0) {
            if (read_timer_raw_macro2() > timeout) {
                printf("Timeout waiting for echo (rising edge)\n");
                goto done;
            }
        }
        t1 = read_timer_raw_macro2();  // Time when echo is detected (rising edge)
        // Calculate time difference and distance
        double dt_us = (double)(t1 - t0);
        double distance_cm = (dt_us * SOUND_SPEED_CM_PER_US) / 2.0;
        printf("Echo delay = %.0f us → distance = %.2f cm\n", dt_us, distance_cm);
    done:
        sleep_ms(100);  // avoid flooding
    }
}
 


