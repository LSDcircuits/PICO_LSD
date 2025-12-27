cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(lsd_pid C CXX ASM)

pico_sdk_init()

add_executable(lsd_pid
    lsd_pid.c
)

pico_enable_stdio_usb(lsd_pid 1)
pico_enable_stdio_uart(lsd_pid 0)

target_link_libraries(lsd_pid
    pico_stdlib     
    hardware_adc
)
pico_add_extra_outputs(lsd_pid)