# Sistemas Embebidos Avanzados II - DSI - EIE - FCEIA - UNR
# Toolchain file: le dice a CMake que compile para ARM y no para la PC.

set(CMAKE_SYSTEM_NAME Generic)          # sin sistema operativo
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
set(CMAKE_SIZE         arm-none-eabi-size)

# CMake prueba el compilador armando un ejecutable. Sin linker script fallaría:
# así prueba armando una biblioteca estática.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
