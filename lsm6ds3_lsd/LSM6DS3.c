#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico_timer.h"
/* --- RP2040-Zero <-> LSM6DS3 (4-wire SPI) ---
   GP2 -> SCL/SPC (SCK)
   GP3 -> SDA/SDI (MOSI)
   GP0 -> SDO/SA0 (MISO)
   GP1 -> CS (active low)
*/
#define IMU_SPI   spi0
#define PIN_SCK   2
#define PIN_MOSI  3
#define PIN_MISO  0
#define PIN_CS    1

/* LSM6DS3 registers */
#define WHO_AM_I    0x0F
#define CTRL1_XL    0x10
#define CTRL2_G     0x11
#define CTRL3_C     0x12
#define STATUS_REG  0x1E
#define OUTX_L_G    0x22

/* Scale factors (datasheet) */
#define GYRO_2000DPS_SENS   0.07f        // dps/LSB at FS=2000 dps
#define ACC_16G_SENS_G      0.000488f    // g/LSB at FS=16 g
#define G_TO_MS2            9.80665f

// raw sensor data from register
typedef struct {
}


//SPI header: MSB = 1 for read, 0 for write; remaining 7 bits = address */
static inline void cs_low(void)  { gpio_put(PIN_CS, 0); }
static inline void cs_high(void) { gpio_put(PIN_CS, 1); }



static void reg_write(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };
    cs_low();  spi_write_blocking(IMU_SPI, tx, 2);  cs_high();
}
static void reg_read(uint8_t reg, uint8_t *dst, size_t n) {
    uint8_t hdr = (uint8_t)(reg | 0x80);
    cs_low();  spi_write_blocking(IMU_SPI, &hdr, 1);
    spi_read_blocking(IMU_SPI, 0x00, dst, n);  cs_high();
}
static void imu_init(void) {
    // SPI @10 MHz, SPI mode 3 (CPOL=1, CPHA=1), 8-bit, MSB first
    spi_init(IMU_SPI, 10 * 1000 * 1000);
    spi_set_format(IMU_SPI, 8, true, true, SPI_MSB_FIRST);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_init(PIN_CS); gpio_set_dir(PIN_CS, GPIO_OUT); cs_high();

    sleep_ms(5);

    // Basic interface setup: BDU=1, IF_INC=1, 4-wire (SIM=0)
    reg_write(CTRL3_C, 0b01000100);   // 0x44

    // Gyro: ODR=6.66 kHz (0b1010<<4), FS=±2000 dps (0b11<<2) => 0xAC
    reg_write(CTRL2_G, 0xAC);

    // Accel: ODR=6.66 kHz (0b1010<<4), FS=±16 g (0b01<<2) => 0xA4
    reg_write(CTRL1_XL, 0xA4);
}
