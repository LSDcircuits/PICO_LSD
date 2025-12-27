#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
 
#define ADC_const 1
#define TEMP_ADC_CHANNEL 0
#define TEMP_ADC_PIN 26
#define DELAY 10
#define VAL_ADC_CHANNEL 1
#define VAL_ADC_PIN 27
#define SET_VAL 10

#define KP 1.0f
#define KI 1.0f
#define KD 1.0f 

typedef struct {
    uint64_t timerV_1;
    uint64_t timerV_2;
    uint16_t adc_val_1;
    uint16_t adc_val_2;
} raw;

typedef struct {
    float error;
    float prev_error;
    float integral;
    float derivative;
    float set_val;
    float output;
} pid_val;

uint64_t read_timer_raw_macro2() {
    uint32_t hi1, lo, hi2;
    do {
        hi1 = timer_hw->timerawh;
        lo = timer_hw->timerawl;
        hi2 = timer_hw->timerawh;
    } while (hi1 != hi2);
    return ((uint64_t)hi1 << 32) | lo;
}

void read_data(raw *R) {

    R->timerV_1 = read_timer_raw_macro2();
    
    adc_select_input(TEMP_ADC_CHANNEL);
    R->adc_val_1 = adc_read();

    adc_select_input(VAL_ADC_CHANNEL);
    R->adc_val_2 = adc_read();
    
    sleep_us(DELAY);

}

void pid_update(raw *R, pid_val *PID) {

    R->timerV_2 = read_timer_raw_macro2();
    float dt = (R->timerV_2 - R->timerV_1)/1000000.0f;;

    PID->error = (R->adc_val_2 - R->adc_val_1) * ADC_const;
    PID->integral += PID->error *dt;
    PID->derivative = (PID->error - PID->prev_error) / dt;

    PID->output = KP * PID->error + KI * PID->integral + KD * PID->derivative;
    PID->prev_error = PID->error;
}

int main(){

    stdio_init_all();
    adc_init();
    adc_gpio_init(TEMP_ADC_PIN);
    adc_gpio_init(VAL_ADC_PIN);

    raw sensor_data;
    pid_val PID = {
        .set_val = SET_VAL,
        .integral = 0,
        .prev_error = 0};

    while(1){
        read_data(&sensor_data);
        pid_update(&sensor_data, &PID);
        printf("adc_val_1: %d, adc_val_2: %d, timerV_1: %llu us\n",
                sensor_data.adc_val_1,
                sensor_data.adc_val_2,
                sensor_data.timerV_1);
        printf("error: %.2f, integral: %.2f, derivative: %.2f\n",
                PID.error,
                PID.integral,
                PID.derivative);
    }
}