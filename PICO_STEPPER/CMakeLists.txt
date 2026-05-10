cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(stepper)

pico_sdk_init()

add_executable(stepper
stepper.c
)

pico_add_extra_outputs(stepper)

target_link_libraries(stepper pico_stdlib)