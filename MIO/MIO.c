// lib made to work with gpio i/o

#include <stdio.h>
#include "pico/stdlib.h"

#define SIO_BASE        0xD0000000
#define GPIO_OUT        (*(volatile uint32_t *)(SIO_BASE + 0x10))
#define GPIO_OUT_SET    (*(volatile uint32_t *)(SIO_BASE + 0x14))
#define GPIO_OUT_CLR    (*(volatile uint32_t *)(SIO_BASE + 0x18))
#define GPIO_OUT_XOR    (*(volatile uint32_t *)(SIO_BASE + 0x1C))
#define GPIO_OE_SET     (*(volatile uint32_t *)(SIO_BASE + 0x24))

void gpio_init_mmio(uint8_t pin) {
    GPIO_OE_SET = (1u << pin);
}

void gpio_set_mmio(uint8_t pin, bool value) {
    if (value)
        GPIO_OUT_SET = (1u << pin);
    else
        GPIO_OUT_CLR = (1u << pin);
}

int main() {
    const uint8_t LED_PIN = 25; // onboard LED
    stdio_init_all();
    gpio_init_mmio(LED_PIN);

    while (true) {
        gpio_set_mmio(LED_PIN, true);
        sleep_ms(250);
        gpio_set_mmio(LED_PIN, false);
        sleep_ms(250);
    }
}
