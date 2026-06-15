cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(hw7_dac C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(hw7_dac
    HW7.c
)

# Pull in SPI and math libraries
target_link_libraries(hw7_dac
    pico_stdlib
    hardware_spi
    m
)

# Enable USB serial output (optional, useful for debugging)
pico_enable_stdio_usb(hw7_dac 1)
pico_enable_stdio_uart(hw7_dac 0)

pico_add_extra_outputs(hw7_dac)
