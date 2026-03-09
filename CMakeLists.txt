
cmake_minimum_required(VERSION 3.13)
include(libs/pico-sdk/pico_sdk_init.cmake)
project(hello_world_project)
pico_sdk_init()
add_executable(hello_world src/hello_world.c)
target_link_libraries(hello_world pico_stdlib)
pico_add_extra_outputs(hello_world)