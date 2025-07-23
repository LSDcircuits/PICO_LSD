#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#define delay   10 //print dalay

typedef struct {
    uint16_t ch1;
    uint16_t ch2;
    uint16_t ch3;
    uint16_t ch4;
} channels_t;

void setup_ADC() {
    adc_init();
}

void Read_print(){
    channels_t RC;
    const float conversion_factor = 3.3f / (1 << 12);

    adc_gpio_init(26);
    adc_select_input(0);
    RC.ch1 = adc_read();
    printf("INA1: ADC Value: %u, Voltage: %.4f V\n", RC.ch1, RC.ch1 * (conversion_factor));
    sleep_ms(delay);
    
    adc_gpio_init(27);
    adc_select_input(1);
    RC.ch2 = adc_read();
    printf("INA2: ADC Value: %u, Voltage: %.4f V\n", RC.ch2, RC.ch2 * (conversion_factor));
    sleep_ms(delay);

    adc_gpio_init(28);
    adc_select_input(2);
    RC.ch3 = adc_read();
    printf("INA3: ADC Value: %u, Voltage: %.4f V\n", RC.ch3, RC.ch3 * (conversion_factor));
    sleep_ms(delay);

    adc_gpio_init(29);
    adc_select_input(3);
    RC.ch4 = adc_read();
    printf("INA4: ADC Value: %u, Voltage: %.4f V\n", RC.ch4, RC.ch4 * (conversion_factor));
    sleep_ms(delay);
}

int main() {
    stdio_init_all();
    setup_ADC();
    while (1) {
        Read_print();
        sleep_ms(10); // Wait before the next reading
    }
    return 0;
}