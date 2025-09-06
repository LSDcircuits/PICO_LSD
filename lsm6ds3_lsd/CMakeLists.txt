cmake_minimum_required(VERSION 3.13)

# Import the Pico SDK
include(pico_sdk_import.cmake)

# Set project name
project(kalman C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add your executable and source file
add_executable(kalman
    kalman.c
    pico_timer.c
)

# Enable USB stdio (disable UART if you don’t need it)
pico_enable_stdio_usb(kalman 1)
pico_enable_stdio_uart(kalman 0)

# Add the standard library + hardware support you need
target_link_libraries(kalman
    pico_stdlib     # printf, sleep, etc.
    hardware_i2c    # I2C API
    hardware_spi    # SPI API
    pico_time       # time-related functions
    m               # math library (for <math.h>)
    hardware_timer
)

# Create UF2, bin, and hex files
pico_add_extra_outputs(kalman)