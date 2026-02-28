cmake_minimum_required(VERSION 3.20)

set(CROSS_PREFIX "arm-none-eabi" CACHE STRING "Cross-compiler prefix")

if(NOT DEFINED CMAKE_SYSTEM_NAME)
    set(CMAKE_SYSTEM_NAME Generic)
endif()

if(NOT DEFINED CMAKE_TRY_COMPILE_TARGET_TYPE)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "${CROSS_PREFIX}-gcc")
endif()

if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "${CROSS_PREFIX}-g++")
endif()

project(cortexm_hello LANGUAGES C CXX ASM)

set(ARM_NEWLIB_INCLUDE_DIR "/usr/include/newlib" CACHE PATH "Path to newlib headers")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(LIBCXX_ARM_ROOT "/opt/libcxx-arm-none-eabi" CACHE PATH "Path to ARM libc++ bundle")
    set(ARM_GCC_VERSION "12.2.1" CACHE STRING "ARM GCC version for libgcc path")
    set(ARM_MULTILIB_DIR "thumb/v7-m/nofp" CACHE STRING "ARM multilib directory")
endif()

find_program(CMAKE_OBJCOPY NAMES "${CROSS_PREFIX}-objcopy" REQUIRED)
find_program(CMAKE_SIZE NAMES "${CROSS_PREFIX}-size" REQUIRED)
find_program(QEMU_SYSTEM_ARM NAMES qemu-system-arm)

add_executable(hello.elf
    src/startup.cpp
    src/main.cpp
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_include_directories(hello.elf PRIVATE
        ${ARM_NEWLIB_INCLUDE_DIR}
        ${LIBCXX_ARM_ROOT}/include/c++/v1
    )

    target_compile_options(hello.elf PRIVATE
        -stdlib=libc++
    )

    target_link_directories(hello.elf PRIVATE
        ${LIBCXX_ARM_ROOT}/lib
        /usr/lib/gcc/arm-none-eabi/${ARM_GCC_VERSION}/${ARM_MULTILIB_DIR}
        /usr/lib/arm-none-eabi/newlib/${ARM_MULTILIB_DIR}
    )

    target_link_options(hello.elf PRIVATE
        -stdlib=libc++
        -Wl,--start-group
        -lrdimon
        -lc
        -lm
        -lgcc
        -lc++
        -lc++abi
        -lunwind
        -Wl,--end-group
    )
endif()

target_compile_options(hello.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -std=c++20
    -O0
    -g3
    -ffreestanding
    -fdata-sections
    -ffunction-sections
    -fno-exceptions
    -fno-rtti
    -Wall
    -Wextra
)

target_link_options(hello.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -T${CMAKE_SOURCE_DIR}/linker.ld
    -specs=rdimon.specs
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/hello.map
    -Wl,--entry=Reset_Handler
    -nostartfiles
)

set_target_properties(hello.elf PROPERTIES
    OUTPUT_NAME hello
    SUFFIX .elf
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/hello.bin
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:hello.elf> ${CMAKE_BINARY_DIR}/hello.bin
    DEPENDS hello.elf
    COMMENT "Generating hello.bin"
    VERBATIM
)

add_custom_target(bin ALL DEPENDS ${CMAKE_BINARY_DIR}/hello.bin)

add_custom_target(size
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:hello.elf>
    DEPENDS hello.elf
    COMMENT "Printing ELF size"
)

if(QEMU_SYSTEM_ARM)
    add_custom_target(run
        COMMAND ${QEMU_SYSTEM_ARM}
                -machine lm3s6965evb
                -cpu cortex-m3
                -nographic
                -semihosting-config enable=on,target=native
                -kernel $<TARGET_FILE:hello.elf>
        DEPENDS hello.elf
        COMMENT "Running hello.elf in QEMU"
    )
else()
    message(STATUS "qemu-system-arm not found: 'run' target will not be available")
endif()