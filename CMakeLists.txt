cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "~/pico/pico-sdk")
# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_ws2812_sound C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()
add_executable(pico_ws2812_sound)

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(pico_ws2812_sound ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_sources(pico_ws2812_sound PRIVATE main.c)
pico_enable_stdio_uart(pico_ws2812_sound 0)
pico_enable_stdio_usb(pico_ws2812_sound 1)

target_link_libraries(pico_ws2812_sound PRIVATE pico_stdlib hardware_pio hardware_adc)
pico_add_extra_outputs(pico_ws2812_sound)

# add url via pico_set_program_url
#example_auto_set_url(pico_ws2812_sound)

# Additionally generate python and hex pioasm outputs for inclusion in the RP2040 datasheet
add_custom_target(pico_ws2812_sound_datasheet DEPENDS ${CMAKE_CURRENT_LIST_DIR}/generated/ws2812.py)
add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/generated/ws2812.py
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
        COMMAND Pioasm -o python ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio ${CMAKE_CURRENT_LIST_DIR}/generated/ws2812.py
        )
add_dependencies(pico_ws2812_sound pico_ws2812_sound_datasheet)
