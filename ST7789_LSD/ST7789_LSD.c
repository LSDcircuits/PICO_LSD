#include "ST7789_LSD.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <string.h>
#include <stdio.h>

// I2C Configuration
#define I2C_PORT i2c0
#define SDA_PIN  4
#define SCL_PIN  5

static uint8_t framebuffer[OLED_WIDTH * OLED_HEIGHT / 8];

// ==== BASIC FONT (5x7) ====
static const uint8_t font5x7[][5] = {
    // only 32-90 ASCII subset to keep small
    // Each char = 5 bytes, each column (bit 0 = top pixel)
    {0,0,0,0,0}, // space
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x14,0x08,0x3E,0x08,0x14}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x50,0x30,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x32,0x49,0x79,0x41,0x3E}, // @
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    // ... (cut short for brevity, you’d fill more as needed)
};

// ==== I2C HELPERS ====
void oled_send_command(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
}

void oled_send_data(uint8_t *data, size_t len) {
    uint8_t buf[len+1];
    buf[0] = 0x40;
    memcpy(&buf[1], data, len);
    i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, len+1, false);
}

// ==== OLED CORE ====
void oled_init() {
    // init I2C
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    sleep_ms(100);

    oled_send_command(0xAE); // display off
    oled_send_command(0x20); oled_send_command(0x00); // horizontal addressing
    oled_send_command(0xB0); // page start
    oled_send_command(0xC8); // COM scan dir remap
    oled_send_command(0x00); // low column
    oled_send_command(0x10); // high column
    oled_send_command(0x40); // start line
    oled_send_command(0x81); oled_send_command(0x7F); // contrast
    oled_send_command(0xA1); // segment remap
    oled_send_command(0xA6); // normal display
    oled_send_command(0xA8); oled_send_command(0x3F); // multiplex
    oled_send_command(0xA4); // display resume
    oled_send_command(0xD3); oled_send_command(0x00); // offset
    oled_send_command(0xD5); oled_send_command(0x80); // clock
    oled_send_command(0xD9); oled_send_command(0xF1); // precharge
    oled_send_command(0xDA); oled_send_command(0x12); // compins
    oled_send_command(0xDB); oled_send_command(0x40); // vcom detect
    oled_send_command(0x8D); oled_send_command(0x14); // charge pump
    oled_send_command(0xAF); // display on
}

void oled_clear() {
    memset(framebuffer, 0, sizeof(framebuffer));
}

void oled_update() {
    for (int page = 0; page < 8; page++) {
        oled_send_command(0xB0 + page);
        oled_send_command(0x00);
        oled_send_command(0x10);
        oled_send_data(&framebuffer[OLED_WIDTH * page], OLED_WIDTH);
    }
}

// ==== DRAW PIXELS & TEXT ====
void oled_set_pixel(int x, int y, bool color) {
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT) return;
    int page = y / 8;
    int bit = y % 8;
    if (color)
        framebuffer[page * OLED_WIDTH + x] |= (1 << bit);
    else
        framebuffer[page * OLED_WIDTH + x] &= ~(1 << bit);
}

void oled_draw_char(int x, int y, char c) {
    if (c < 32 || c > 90) return;  // supported subset
    const uint8_t *glyph = font5x7[c-32];
    for (int col = 0; col < 5; col++) {
        uint8_t line = glyph[col];
        for (int row = 0; row < 7; row++) {
            if (line & (1 << row)) {
                oled_set_pixel(x+col, y+row, true);
            }
        }
    }
}

void oled_draw_string(int x, int y, const char *str) {
    while (*str) {
        oled_draw_char(x, y, *str++);
        x += 6; // 5px char + 1px space
    }
}

// ==== PRINT TEMPERATURE CENTERED ====
void oled_print_temperature(float value) {
    char buf[16];
    sprintf(buf, "%06.2fC", value);

    int text_width = strlen(buf) * 6;
    int x = (OLED_WIDTH - text_width) / 2;
    int y = (OLED_HEIGHT / 2) - 4;

    oled_clear();
    oled_draw_string(x, y, buf);
    oled_update();
}