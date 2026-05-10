#include <stdio.h>
#include "pico/stdlib.h"
#define A1 0
#define B1 1 
#define A2 2
#define B2 3
#define IN1 15
#define IN2 14
//motor2
#define A11 4
#define B11 5
#define A22 6
#define B22 7
#define IN11 27
#define IN22 26
#define time_s 1500 //600 for max rpm and 4800 for min
//VISUAL REPRESENATION FOR MOTOR STEPS BELOW
// | STEP  |A1 B1 A2 B2 |
// |  1    |1  1  0  0  |
// |  2    |0  1  1  0  |
// |  3    |0  0  1  1  |
// |  4    |0  0  0  1  |
int stepposition = 0; //set to zero
int stepposition2 = 0;
//confusing but clean
void setStep(int a1, int b1, int a2, int b2){
    gpio_put(A1, a1);
    gpio_put(B1, b1);
    gpio_put(A2, a2);
    gpio_put(B2, b2);
}
void setStep2(int a11, int b11, int a22, int b22){
    gpio_put(A11, a11);
    gpio_put(B11, b11);
    gpio_put(A22, a22);
    gpio_put(B22, b22);
}
//this void loop allows a linear drive from its prevoius pin state
void stepperL(){
    stepposition = (stepposition - 1 + 4) % 4; //move prev and wrap
    switch(stepposition){ 
        case 0: setStep(1, 0, 0, 1); break;
        case 1: setStep(0, 0, 1, 1); break;
        case 2: setStep(0, 1, 1, 0); break;
        case 3: setStep(1, 1, 0, 0); break;
    }

}
void stepperR(){
    stepposition = (stepposition + 1) % 4; //move prev and wrap
    switch(stepposition){ 
        case 0: setStep(1, 0, 0, 1); break;
        case 1: setStep(0, 0, 1, 1); break;
        case 2: setStep(0, 1, 1, 0); break;
        case 3: setStep(1, 1, 0, 0); break;
    }
}
//this void loop allows a linear drive from its prevoius pin state for motor2 yay..
void stepperL2(){
    stepposition2 = (stepposition2 - 1 + 4) % 4; //move prev and wrap
    switch(stepposition2){ 
        case 0: setStep2(1, 0, 0, 1); break;
        case 1: setStep2(0, 0, 1, 1); break;
        case 2: setStep2(0, 1, 1, 0); break;
        case 3: setStep2(1, 1, 0, 0); break;
    }

}
void stepperR2(){
    stepposition2 = (stepposition2 + 1) % 4; //move prev and wrap
    switch(stepposition2){ 
        case 0: setStep2(1, 0, 0, 1); break;
        case 1: setStep2(0, 0, 1, 1); break;
        case 2: setStep2(0, 1, 1, 0); break;
        case 3: setStep2(1, 1, 0, 0); break;
    }
}
int main() {
    // GPIO initialization and direction setting
    gpio_init(IN1);
    gpio_set_dir(IN1, GPIO_IN);
    gpio_init(IN2);
    gpio_set_dir(IN2, GPIO_IN);
    gpio_init(A1);
    gpio_set_dir(A1, GPIO_OUT);
    gpio_init(B1);
    gpio_set_dir(B1, GPIO_OUT);
    gpio_init(A2);
    gpio_set_dir(A2, GPIO_OUT);
    gpio_init(B2);
    gpio_set_dir(B2, GPIO_OUT);

    // Repeat for motor 2
    gpio_init(IN11);
    gpio_set_dir(IN11, GPIO_IN);
    gpio_init(IN22);
    gpio_set_dir(IN22, GPIO_IN);
    gpio_init(A11);
    gpio_set_dir(A11, GPIO_OUT);
    gpio_init(B11);
    gpio_set_dir(B11, GPIO_OUT);
    gpio_init(A22);
    gpio_set_dir(A22, GPIO_OUT);
    gpio_init(B22);
    gpio_set_dir(B22, GPIO_OUT);

    while(1){
        int motor1InputA = gpio_get(IN1);
        int motor1InputB = gpio_get(IN2);
        int motor2InputA = gpio_get(IN11);
        int motor2InputB = gpio_get(IN22);
        if (motor1InputA == 1 && motor1InputB == 1) {
            stepperR();
        } else if (motor1InputA == 0 && motor1InputB == 1) {
        stepperL(); 
        }
        if (motor2InputA == 1 && motor2InputB == 1) {
        stepperR2();
        } else if (motor2InputA == 0 && motor2InputB == 1) {
        stepperL2();
        }
    sleep_us(time_s);
    }
}