@echo off
rem ===================================================================
rem  Sistemas Embebidos Avanzados II - DSI - EIE - FCEIA - UNR
rem  Configura el PATH de ESTA ventana de cmd para compilar el blinky.
rem
rem  Uso: abrir cmd en la carpeta del proyecto y escribir   entorno
rem
rem  Ajustar las tres rutas a las de cada PC. Para encontrarlas:
rem    where /r C:\nxp arm-none-eabi-gcc.exe
rem    where /r C:\Espressif cmake.exe
rem    where /r C:\Espressif ninja.exe
rem ===================================================================

set MCUX=C:\nxp\MCUXpressoIDE_25.6.136\ide
set CMAKE_DIR=C:\Espressif\tools\cmake\4.0.3\bin
set NINJA_DIR=C:\Espressif\tools\ninja\1.12.1

set PATH=%MCUX%\tools\bin;%MCUX%\buildtools\bin;%CMAKE_DIR%;%NINJA_DIR%;%PATH%

echo.
echo Herramientas encontradas:
where arm-none-eabi-gcc
where make
where cmake
where ninja
echo.
echo Si alguna no aparece, corregir su ruta en entorno.bat
