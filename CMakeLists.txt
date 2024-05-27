cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

project(wirelessDMXTransmitter C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD "pico_w")

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

#set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

include(Pico-DMX/interfaceLibForPicoSDK.cmake)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

if (NOT TARGET pico_cyw43_arch)
message("Skipping Pico W examples as support is not available")
endif()

add_executable(wirelessDMXTransmitter
        wirelessDMXTransmitter.cpp
        dhcpserver/dhcpserver.c
        dnsserver/dnsserver.c
        )

target_include_directories(wirelessDMXTransmitter PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        ${CMAKE_CURRENT_LIST_DIR}/dnsserver
        )

target_link_libraries(wirelessDMXTransmitter
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        picodmx
        )

pico_add_extra_outputs(wirelessDMXTransmitter)

pico_enable_stdio_usb(wirelessDMXTransmitter 1)
pico_enable_stdio_uart(wirelessDMXTransmitter 0)
