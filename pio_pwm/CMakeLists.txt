cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pio_pwm)

pico_sdk_init()

add_executable(pio_pwm
    pio_pwm.c
    pwm_control.c

)

pico_generate_pio_header(pio_pwm ${CMAKE_CURRENT_LIST_DIR}/pwm.pio)
pico_enable_stdio_usb(pio_pwm 1)
pico_add_extra_outputs(pio_pwm)

target_link_libraries(pio_pwm pico_stdlib hardware_pio hardware_adc hardware_clocks hardware_timer)
