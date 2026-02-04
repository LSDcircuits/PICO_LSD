cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(lsd_heater C CXX ASM)

pico_sdk_init()

add_executable(lsd_heater
    lsd_heater.c
)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pwm0.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pwm1.pio)
pico_enable_stdio_usb(lsd_heater 1)
pico_enable_stdio_uart(lsd_heater 0)

target_link_libraries(lsd_heater
    pico_stdlib     
    hardware_pio
    hardware_i2c
    hardware_adc
)

pico_add_extra_outputs(lsd_heater)