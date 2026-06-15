cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(hw8_sram_dac C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(hw8_sram_dac
    HW8..c
)

target_link_libraries(hw8_sram_dac
    pico_stdlib
    hardware_spi
    m
)

pico_enable_stdio_usb(hw8_sram_dac 1)
pico_enable_stdio_uart(hw8_sram_dac 0)

pico_add_extra_outputs(hw8_sram_dac)
