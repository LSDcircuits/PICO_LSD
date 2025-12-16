cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(lsd_pump C CXX ASM)

pico_sdk_init()

add_executable(lsd_pump
    lsd_pump.c
    SSD1306.c
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pwm.pio)
pico_enable_stdio_usb(lsd_pump 1)
pico_enable_stdio_uart(lsd_pump 0)

target_link_libraries(lsd_pump
    pico_stdlib     
    hardware_pio
    hardware_i2c
    hardware_adc

)
pico_add_extra_outputs(lsd_pump)