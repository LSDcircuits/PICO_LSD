cmake_minimum_required(VERSION 3.13)

# Import the Pico SDK
include(pico_sdk_import.cmake)

# Set project name
project(lsd_ultraS C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add your executable and source file
add_executable(lsd_ultraS
    lsd_ultraS.c
    
)

# Enable USB stdio (disable UART if you don’t need it)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pulse.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pulse_inverted.pio)
pico_enable_stdio_usb(lsd_ultraS 1)
pico_enable_stdio_uart(lsd_ultraS 0)

# Add the standard library + hardware support you need
target_link_libraries(lsd_ultraS
    pico_stdlib     # printf, sleep, etc.
    hardware_pio
)

# Create UF2, bin, and hex files
pico_add_extra_outputs(lsd_ultraS)