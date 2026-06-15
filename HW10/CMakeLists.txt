cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(hw10_pot C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(hw10_pot
    HW10.c
)

target_link_libraries(hw10_pot
    pico_stdlib
    hardware_adc
)

# Route stdio over USB so the computer sees the serial output
pico_enable_stdio_usb(hw10_pot 1)
pico_enable_stdio_uart(hw10_pot 0)

pico_add_extra_outputs(hw10_pot)
