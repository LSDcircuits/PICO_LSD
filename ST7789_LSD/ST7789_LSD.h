#ifndef ST7789_LSD_H
#define ST7789_LSD_H

#include <stdbool.h>

// Display configuration
#define OLED_WIDTH   128
#define OLED_HEIGHT  64
#define OLED_ADDR    0x3C  // I2C address

// Function prototypes
void oled_init(void);
void oled_clear(void);
void oled_update(void);
void oled_set_pixel(int x, int y, bool color);
void oled_draw_char(int x, int y, char c);
void oled_draw_string(int x, int y, const char *str);
void oled_print_temperature(float value);

#endif // ST7789_LCD_H
