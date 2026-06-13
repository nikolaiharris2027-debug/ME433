cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK (must be set before project())
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(HW3 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(HW3
    HW3.c
)

# Enable USB serial output (printf via USB); disable UART serial
pico_enable_stdio_usb(HW3 1)
pico_enable_stdio_uart(HW3 0)

# Link the I2C hardware library
target_link_libraries(HW3
    pico_stdlib
    hardware_i2c
)

# Create .uf2, .hex, .bin, etc.
pico_add_extra_outputs(HW3)