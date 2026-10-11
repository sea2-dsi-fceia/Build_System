# Sistemas Embebidos Avanzados II - DSI - EIE - FCEIA - UNR
# CMakeLists del blinky bare-metal para la FRDM-KL46Z
#
# Configurar (una vez por carpeta de build):
#   cmake -B build-release -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Release
#   cmake -B build-debug   -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug
# Compilar (cada vez):
#   cmake --build build-release

cmake_minimum_required(VERSION 3.16)
project(blinky_kl46z C)

set(LDSCRIPT ${CMAKE_SOURCE_DIR}/linker/mkl46z256.ld)

# Opciones de cada tipo de build
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g")

add_executable(blinky.elf
    src/main.c
    startup/startup_mkl46z4.c
)

target_compile_options(blinky.elf PRIVATE
    -mcpu=cortex-m0plus -mthumb -Wall
    -ffunction-sections -fdata-sections
)

target_link_options(blinky.elf PRIVATE
    -mcpu=cortex-m0plus -mthumb
    -T ${LDSCRIPT}
    -nostdlib
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/blinky.map
)

# Si cambia el linker script, volver a linkear
set_target_properties(blinky.elf PROPERTIES LINK_DEPENDS ${LDSCRIPT})

# Después de linkear: generar el .bin y mostrar el uso de memoria
add_custom_command(TARGET blinky.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary blinky.elf blinky.bin
    COMMAND ${CMAKE_SIZE} blinky.elf
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
