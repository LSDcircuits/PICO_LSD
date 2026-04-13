#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/timer.h"    // For timer_hw
#include "hardware/sync.h"     // For critical sections if needed
#include "pwm.pio.h"           // Your PIO program (assumed single file)

// ADC Settings
#define ADC_SCALE 1.0f
#define TEMP_ADC_CHANNEL 0     // GPIO 26 - Temperature sensor
#define TEMP_ADC_PIN 26
#define SET_ADC_CHANNEL 1      // GPIO 27 - Setpoint potentiometer
#define SET_ADC_PIN 27

// PID Tuning (Adjust these for your thermal system!)
#define KP 2.0f                // Proportional gain
#define KI 0.5f                // Integral gain (reduced to prevent windup)
#define KD 0.1f                // Derivative gain (reduced for noise filtering)
#define INTEGRAL_MAX 500.0f    // Anti-windup limit
#define INTEGRAL_MIN -500.0f
#define OUTPUT_MAX 100.0f      // Max duty cycle %
#define OUTPUT_MIN 0.0f        // Min duty cycle % (0% for heater)

// PWM Settings (PIO-based)
#define HEATER_PWM_PIN 1       // GPIO 1 for heater control
#define MOTOR_PWM_PIN 2        // GPIO 2 for stirrer/mixer (optional)
#define HEATER_PERIOD 5000     // PWM period in PIO cycles
#define MOTOR_PERIOD 5000
#define PIO_CLKDIV 10.0f

// Control
#define ON_BUTTON_PIN 28       // GPIO 28 (changed from 26 to avoid ADC conflict!)
#define LOOP_DELAY_MS 10       // Main loop delay (~100Hz update rate)

typedef struct {
    uint64_t timerV_1;         // Loop start time
    uint64_t timerV_2;         // Loop end time (for dt calculation)
    uint16_t adc_temp;         // Raw temperature ADC value
    uint16_t adc_setpoint;     // Raw setpoint ADC value
} SensorData;

typedef struct {
    float error;
    float prev_error;
    float integral;
    float derivative;
    float output;
    float kp, ki, kd;
} PIDController;


uint64_t read_timer_raw_macro2() {
    uint32_t hi1, lo, hi2;
    do {
        hi1 = timer_hw->timerawh;
        lo = timer_hw->timerawl;
        hi2 = timer_hw->timerawh;
    } while (hi1 != hi2);
    return ((uint64_t)hi1 << 32) | lo;
}

void pwm_program_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = pwm_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_clkdiv(&c, PIO_CLKDIV); 
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


void read_data(SensorData *R) {
    R->timerV_1 = read_timer_raw_macro2();
    
    // Read temperature sensor
    adc_select_input(TEMP_ADC_CHANNEL);
    sleep_us(2);  // Small settling time
    R->adc_temp = adc_read();

    // Read setpoint potentiometer
    adc_select_input(SET_ADC_CHANNEL);
    sleep_us(2);
    R->adc_setpoint = adc_read();
}

void pid_update(SensorData *R, PIDController *pid) {
    // Calculate dt in seconds
    R->timerV_2 = read_timer_raw_macro2();
    float dt = (R->timerV_2 - R->timerV_1) / 1000000.0f;
    
    // Prevent division by zero on first run or overflow
    if (dt <= 0.0f || dt > 1.0f) dt = 0.001f;
    
    // Calculate error: Setpoint - Actual (12-bit ADC values: 0-4095)
    pid->error = ((float)R->adc_setpoint - (float)R->adc_temp) * ADC_SCALE;
    
    // Proportional term
    float P = pid->kp * pid->error;
    
    // Integral term with anti-windup (clamping)
    pid->integral += pid->error * dt;
    if (pid->integral > INTEGRAL_MAX) pid->integral = INTEGRAL_MAX;
    if (pid->integral < INTEGRAL_MIN) pid->integral = INTEGRAL_MIN;
    float I = pid->ki * pid->integral;
    
    // Derivative term (on error - consider changing to measurement for setpoint kick avoidance)
    pid->derivative = (pid->error - pid->prev_error) / dt;
    float D = pid->kd * pid->derivative;
    
    // Calculate total output
    pid->output = P + I + D;
    
    // Clamp output to 0-100% range for PWM
    if (pid->output > OUTPUT_MAX) pid->output = OUTPUT_MAX;
    if (pid->output < OUTPUT_MIN) pid->output = OUTPUT_MIN;
    
    pid->prev_error = pid->error;
}



int main() {
    stdio_init_all();
    
    // Initialize ADC
    adc_init();
    adc_gpio_init(TEMP_ADC_PIN);
    adc_gpio_init(SET_ADC_PIN);
    
    // Initialize ON button with pull-down (active high when pressed)
    gpio_init(ON_BUTTON_PIN);
    gpio_set_dir(ON_BUTTON_PIN, GPIO_IN);
    gpio_pull_down(ON_BUTTON_PIN);
    
    // Initialize PID controller
    PIDController pid = {
        .kp = KP,
        .ki = KI,
        .kd = KD,
        .error = 0,
        .prev_error = 0,
        .integral = 0,
        .derivative = 0,
        .output = 0
    };
    
    SensorData sensor_data = {0};
    
    // Initialize PIO PWM
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &pwm_program);
    
    // Setup Heater PWM (State Machine 0)
    uint sm_heater = 0;
    pwm_program_init(pio, sm_heater, offset, HEATER_PWM_PIN);
    pwm_set_period(pio, sm_heater, HEATER_PERIOD);
    
    // Setup Motor/Mixer PWM (State Machine 1) - Optional
    uint sm_motor = 1;
    pwm_program_init(pio, sm_motor, offset, MOTOR_PWM_PIN);
    pwm_set_period(pio, sm_motor, MOTOR_PERIOD);
    
    printf("\n=== PID Heater Controller Started ===\n");
    printf("KP: %.2f | KI: %.2f | KD: %.2f\n", KP, KI, KD);
    printf("Heater Pin: %d | Motor Pin: %d\n", HEATER_PWM_PIN, MOTOR_PWM_PIN);
    printf("ADC Temp: GPIO %d | ADC Set: GPIO %d\n", TEMP_ADC_PIN, SET_ADC_PIN);
    printf("=====================================\n");
    
    while(1) {
        // Read sensors and calculate PID
        read_data(&sensor_data);
        pid_update(&sensor_data, &pid);
        
        // Check if system is enabled
        bool system_active = gpio_get(ON_BUTTON_PIN);
        
        if (system_active) {
            // Convert PID output (0-100%) to PWM duty cycle
            // Period 5000 = 100%, so multiply by 50
            uint32_t heater_duty = (uint32_t)(pid.output * (HEATER_PERIOD / 100.0f));
            pio_sm_put_blocking(pio, sm_heater, heater_duty);
            
            // Optional: Control mixer motor - here set to fixed 25% or proportional
            // You can also map this to pid.output if motor speed should track heat
            uint32_t motor_duty = (uint32_t)(25.0f * (MOTOR_PERIOD / 100.0f));
            pio_sm_put_blocking(pio, sm_motor, motor_duty);
            
        } else {
            // System OFF - disable outputs and reset integral to prevent windup
            pio_sm_put_blocking(pio, sm_heater, 0);
            pio_sm_put_blocking(pio, sm_motor, 0);
            pid.integral = 0;
            pid.output = 0;
        }
        
        // Debug output (uncomment if not needed to save CPU)
        printf("T:%4d S:%4d E:%6.1f I:%6.1f D:%6.1f OUT:%5.1f%% %s\n",
               sensor_data.adc_temp,
               sensor_data.adc_setpoint,
               pid.error,
               pid.integral,
               pid.derivative,
               pid.output,
               system_active ? "[ON]" : "[OFF]");
        
        sleep_ms(LOOP_DELAY_MS);
    }
    
    return 0;
}
