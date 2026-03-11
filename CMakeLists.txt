
cmake_minimum_required(VERSION 3.13)
include(libs/pico-sdk/pico_sdk_init.cmake)
project(hello_world_project)
pico_sdk_init()
add_executable(hello_world src/hello_world.c)
target_link_libraries(hello_world pico_stdlib)
target_compile_definitions(hello_world PRIVATE
	PICO_DEFAULT_UART=0
	PICO_DEFAULT_UART_TX_PIN=16
	PICO_DEFAULT_UART_RX_PIN=17
)
pico_enable_stdio_usb(hello_world 0)
pico_enable_stdio_uart(hello_world 1)
pico_add_extra_outputs(hello_world)
#pico_enable_stdio_semihosting(hello_world 1)