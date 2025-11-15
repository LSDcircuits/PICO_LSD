cmake_minimum_required(VERSION 3.13)

# Import the Pico SDK
include(pico_sdk_import.cmake)

# Set project name
project(SSD1306_LSD C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add your executable and source file
add_executable(SSD1306_LSD
    SSD1306_LSD.c
    SSD1306.c
    
)

pico_enable_stdio_usb(SSD1306_LSD 1)
pico_enable_stdio_uart(SSD1306_LSD 0)

# Add the standard library + hardware support you need
target_link_libraries(SSD1306_LSD
    pico_stdlib     # printf, sleep, etc.
    hardware_i2c
)

# Create UF2, bin, and hex files
pico_add_extra_outputs(SSD1306_LSD)