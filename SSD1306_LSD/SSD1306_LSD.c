#include <stdio.h>
#include "SSD1306.h"
#include "pico/stdlib.h"

int main(){
    while(1){
        float valu = 10;
        oled_print_value(valu);
        sleep_ms(10);
    }
}
