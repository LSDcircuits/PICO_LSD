#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Framebuffer: one byte = 8 vertical pixels (page system)
unsigned char framebuffer[OLED_WIDTH * (OLED_HEIGHT / 8)];

// =========================================================
// Clear screen
void oled_clear() {
    memset(framebuffer, 0, sizeof(framebuffer));
}

// Set a single pixel ON/OFF
void oled_set_pixel(int x, int y, bool color) {
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT) return;

    int page = y / 8;     // which page (8 pixels tall row)
    int bit = y % 8;      // which bit inside that page

    if (color)
        framebuffer[page * OLED_WIDTH + x] |= (1 << bit);
    else
        framebuffer[page * OLED_WIDTH + x] &= ~(1 << bit);
}

// Debug: Print framebuffer as ASCII art (terminal simulation)
void oled_print_console() {
    for (int y = 0; y < OLED_HEIGHT; y++) {
        for (int x = 0; x < OLED_WIDTH; x++) {
            int page = y / 8;
            int bit = y % 8;
            if (framebuffer[page * OLED_WIDTH + x] & (1 << bit))
                printf("#");
            else
                printf(".");
        }
        printf("\n");
    }
}

// =========================================================
// Draw a function: y = f(x)
void oled_draw_function(double (*f)(double), double scale_x, double scale_y, int offset_y) {
    for (int x = 0; x < OLED_WIDTH; x++) {
        double y_val = f(x / scale_x);                 // calculate f(x)
        int y = (int)(OLED_HEIGHT/2 - (y_val * scale_y) + offset_y);  // map math -> screen coords
        oled_set_pixel(x, y, true);
    }
}

// Example math functions
double func_sin(double x) {
    return sin(x);   // sine wave
}

double func_parabola(double x) {
    return (x - 6) * (x - 6) / 20.0;   // parabola
}

// =========================================================
// MAIN
int main() {
    oled_clear();

    // Draw sine wave
    oled_draw_function(func_sin, 10.0, 20.0, 0);

    // Draw parabola (shifted down by 10)
    oled_draw_function(func_parabola, 3.0, 2.0, 10);

    // Print to console
    oled_print_console();

    return 0;
}
