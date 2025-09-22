#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico_timer.h"
// RP2040-Zero <-> LSM6DS3 (4-wire SPI) ---
   GP2 -> SCL/SPC (SCK)
   GP3 -> SDA/SDI (MOSI)
   GP0 -> SDO/SA0 (MISO)
   GP1 -> CS (active low)

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
// gyro_data_raw 
    int16_t gx, gy, gz;
    int16_t ax, ay, az;
} imu_raw;

typedef struct {
// time taken per measurment (microsecond)
    uint64_t gx_t, gy_t, gz_t;
    uint64_t ax_t, ay_t, az_t;
} raw_time;


static inline void cs_low(void)  { gpio_put(PIN_CS, 0); }
static inline void cs_high(void) { gpio_put(PIN_CS, 1); }

//SPI header: MSB = 1 for read, 0 for write; remaining 7 bits = address */
// this is used to wirte a byte so that the lsm6Ds3 knows its time to write or receive. 
static void reg_write(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };
    cs_low();  spi_write_blocking(IMU_SPI, tx, 2);  cs_high();
}
// this is used to read 
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
// this needs to be made into a void loop to be used to point to the stuct file.
static void imu_set(void) {
    stdio_init_all();
    sleep_ms(200);
    imu_init();

    // Quick check (optional) this is just a value to send and make sure
    // that you are communicated IK ill forget :/
    
    uint8_t who=0; reg_read(WHO_AM_I, &who, 1);
    printf("WHO_AM_I=0x%02X (expect 0x69)\n", who);

    uint8_t status;
    uint8_t raw[12];
    int16_t gx, gy, gz, ax, ay, az;

    // ca
    int16_t Gx, Gy, Gz, Ax, Ay, Az
    static float angle_x = 0, angle_y = 0, angle_z = 0;
    float   gx_dps, gy_dps, gz_dps, ax_ms2, ay_ms2, az_ms2;
    float   gx_deg, gy_deg, gz_deg;

    while (true) {
        // Wait until both accel and gyro have new data (XLDA|GDA)
        reg_read(STATUS_REG, &status, 1);
        if ((status & 0x03) == 0x03) {
            // One 12-byte burst: Gx..Gz then Ax..Az
            uint64_t Gyro_t0 = read_timer_raw_macro();
            reg_read(OUTX_L_G, raw, sizeof raw);
            gx = (int16_t)((raw[1] << 8) | raw[0]);
            gy = (int16_t)((raw[3] << 8) | raw[2]);
            gz = (int16_t)((raw[5] << 8) | raw[4]);
            ax = (int16_t)((raw[7] << 8) | raw[6]);
            ay = (int16_t)((raw[9] << 8) | raw[8]);
            az = (int16_t)((raw[11] << 8) | raw[10]);
            uint64_t Gyro_t1 = read_timer_raw_macro();
            uint64_t dt = Gyro_t1 - Gyro_t0;
            float dt_s = dt / 1000.0f;
            printf("Elapsed time: %llu microseconds\n", dt);
        }
    }
}
