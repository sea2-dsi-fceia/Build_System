# Build_System

Material de la clase de **build systems en sistemas embebidos**: qué pasa cuando se aprieta **Build**, compilando un mismo programa **a mano**, con **Make** y con **CMake + Ninja** sobre la **FRDM-KL46Z**.

| | |
|---|---|
| **Asignatura** | Sistemas Embebidos Avanzados II |
| **Departamento** | Departamento de Sistemas e Informática — EIE — FCEIA — UNR |
| **Docente** | Prof. Ing. Daniel Márquez |
| **Contacto** | [dmarquez@fceia.unr.edu.ar](mailto:dmarquez@fceia.unr.edu.ar) |

*EIE: Escuela de Ingeniería Electrónica — FCEIA: Facultad de Ciencias Exactas, Ingeniería y Agrimensura — UNR: Universidad Nacional de Rosario.*

---

## Contenido del repositorio

```
Build_System/
├── README.md
├── Guía Práctica Build System/
│   └── SEAII_Guia_practica_Build_System.pdf        Guía práctica: conceptos, entorno y prácticas paso a paso
├── Presentación (Slides)/
│   └── Build Systems y FreeRTOS en el ESP32-S3.pdf  Presentación de la clase
└── blinky_kl46z/                                    Proyecto de ejemplo (sin SDK ni HAL)
    ├── src/main.c                                   LED rojo que parpadea (PTE29)
    ├── startup/startup_mkl46z4.c                    Tabla de vectores, flash config y Reset_Handler
    ├── linker/mkl46z256.ld                          Linker script
    ├── Makefile                                     Build con Make
    ├── CMakeLists.txt                               Build con CMake
    ├── cmake/arm-none-eabi.cmake                    Toolchain file de CMake
    ├── entorno.bat                                  Configura el PATH en Windows
    └── README.md                                    Instrucciones detalladas del ejemplo
```

- 📗 [**Guía práctica**](Gu%C3%ADa%20Pr%C3%A1ctica%20Build%20System/SEAII_Guia_practica_Build_System.pdf): explica los conceptos, prepara el entorno en Windows y recorre las prácticas con cada comando explicado y la salida esperada. **Es el material de estudio principal.**
- 📙 [**Presentación**](Presentaci%C3%B3n%20(Slides)/Build%20Systems%20y%20FreeRTOS%20en%20el%20ESP32-S3.pdf): las slides de la clase.
- 📁 [**blinky_kl46z**](blinky_kl46z/): el proyecto con el que se hacen todas las prácticas.

---

## ¿Para qué estudiar el build?

El IDE compila por nosotros. Entender cómo lo hace permite:

- **Diagnosticar** errores de link (`undefined reference`), de memoria (`region RAM overflowed`) y comportamientos raros al depurar.
- **Controlar la memoria**: saber qué ocupa FLASH y qué ocupa RAM (por ejemplo, el heap y los stacks de FreeRTOS).
- **Ubicar el código** donde se necesita: bootloader y aplicación, actualizaciones OTA, datos en direcciones fijas.
- **Entender la optimización**: Debug y Release no generan el mismo código.
- **No depender de un IDE ni de un fabricante**: MCUXpresso, STM32CubeIDE, ESP-IDF y Zephyr usan el mismo `gcc`, el mismo linker y Make o CMake.
- **Trabajar como en la industria**: builds reproducibles, integración continua y trazabilidad del binario.

## Conceptos clave

| Pieza | Qué es | En la FRDM-KL46Z |
|---|---|---|
| **IDE** | Editor y botones. **No compila:** llama a las demás piezas | MCUXpresso IDE |
| **Build system** | Decide qué compilar, en qué orden y con qué opciones | `make`, `cmake` + `ninja` |
| **Toolchain** | El compilador: traduce y linkea | `arm-none-eabi-gcc` |

**Build** es el proceso (preprocesar → compilar → ensamblar → linkear → convertir); **build system** es la herramienta que lo automatiza.

```
main.c ──► main.i ──► main.s ──► main.o ──► blinky.elf ──► blinky.bin
        preprocesar  compilar   ensamblar    linkear       convertir
```

---

## Requisitos (Windows)

No hace falta instalar nada nuevo si ya están el **MCUXpresso IDE** y **ESP-IDF** (instalado con el EIM):

| Herramienta | Dónde está (las versiones pueden variar) |
|---|---|
| `arm-none-eabi-gcc` | `C:\nxp\MCUXpressoIDE_<versión>\ide\tools\bin` |
| `make` | `C:\nxp\MCUXpressoIDE_<versión>\ide\buildtools\bin` |
| `cmake` | `C:\Espressif\tools\cmake\<versión>\bin` |
| `ninja` | `C:\Espressif\tools\ninja\<versión>` |

Para encontrar las rutas exactas: `where /r C:\nxp arm-none-eabi-gcc.exe` (y lo mismo con `make.exe`, `cmake.exe` y `ninja.exe`).

## Puesta en marcha

1. **Descargar** el repositorio (**Code → Download ZIP**) y descomprimirlo en una ruta **sin espacios ni acentos**, por ejemplo `C:\sea2\`.
   > Al descomprimir, Windows suele crear una carpeta dentro de otra: la carpeta del proyecto es la que contiene el `Makefile`.
2. **Editar `blinky_kl46z\entorno.bat`** con el Bloc de notas y ajustar las tres rutas a las de la PC.
3. **Abrir el cmd** en la carpeta `blinky_kl46z`: en el Explorador, escribir `cmd` en la barra de direcciones y presionar Enter.
   > Usar el **Símbolo del sistema (cmd)**, no PowerShell.
4. **Configurar el PATH** y verificar:
   ```bat
   entorno
   arm-none-eabi-gcc --version
   make --version
   cmake --version
   ninja --version
   ```
   > El PATH vale **solo para esa ventana**: al abrir otra, volver a ejecutar `entorno`.
5. **Compilar:**
   ```bat
   make
   make size
   ```
   Debe mostrar `text 380 · data 4 · bss 1028`.

---

## Prácticas

El detalle de cada paso está en la [guía práctica](Gu%C3%ADa%20Pr%C3%A1ctica%20Build%20System/SEAII_Guia_practica_Build_System.pdf) y en el [README del ejemplo](blinky_kl46z/README.md).

### 1. El build a mano — *nosotros hacemos de build system*

```bat
mkdir tmp
set CPU=-mcpu=cortex-m0plus -mthumb
arm-none-eabi-gcc %CPU% -E src/main.c -o tmp/main.i
arm-none-eabi-gcc %CPU% -Os -S src/main.c -o tmp/main.s
arm-none-eabi-gcc %CPU% -Os -c src/main.c -o tmp/main.o
arm-none-eabi-gcc %CPU% -Os -c startup/startup_mkl46z4.c -o tmp/startup.o
arm-none-eabi-gcc %CPU% -T linker/mkl46z256.ld -nostdlib tmp/main.o tmp/startup.o -o tmp/blinky.elf
arm-none-eabi-objcopy -O binary tmp/blinky.elf tmp/blinky.bin
```

Qué observar: el `.i` sin `#define`, el assembler en el `.s`, los símbolos **sin dirección** en el `.o` (`arm-none-eabi-nm tmp/main.o`) y **con dirección** en el `.elf` (`arm-none-eabi-nm -n tmp/blinky.elf`). Sin `-nostdlib`, el link falla con `undefined reference to __bss_start__`.

### 2. Explorar el binario

`make size`, `arm-none-eabi-objdump -h build/blinky.elf` y el archivo `build/blinky.map`: dónde quedó cada sección en FLASH y en RAM. Experimento: achicar la RAM a `1K` en el linker script → `region 'RAM' overflowed by 8 bytes`.

### 3. Make — *ejecuta los mismos comandos, solo los necesarios*

```bat
make                      :: compila todo
make                      :: Nothing to be done: todo está al día
copy /b src\main.c +,,    :: actualiza la fecha de main.c (equivalente de touch)
make                      :: recompila solo main.c y linkea
make DEBUG=1              :: ¡la trampa! no recompila: Make compara fechas, no opciones
make clean                :: hay que limpiar para cambiar de modo
make DEBUG=1              :: ahora sí: Debug (-O0 -g3), text 472
```

### 4. CMake + Ninja — *cmake genera las instrucciones, ninja las ejecuta*

```bat
cmake -B build-release -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Release
cmake -B build-debug   -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build build-release
cmake --build build-debug
```

Una carpeta por modo: Release (380 bytes) y Debug (472 bytes) conviven sin pisarse.

---

## Valores de referencia

| Qué | Valor esperado |
|---|---|
| `make size` (Release) | text 380 · data 4 · bss 1028 |
| Debug (`-O0 -g3`) | text 472 |
| Práctica 1, sin `--gc-sections` | text 392 (`funcion_no_usada` sigue en el binario) |
| `main` en la práctica 1 / con `make` | `0x41C` / `0x410` |
| `demora_ciclos` (.data) / `contador_parpadeos` (.bss) | `0x1FFFE000` / `0x1FFFE004` |
| Tope del stack (`_estack`) | `0x20006000` |

*Medidos con GCC 13.2 y verificados con el GCC 14.2 que trae el MCUXpresso. Con otra versión del compilador pueden variar unos bytes.*

## Grabar en la placa

Desde el **MCUXpresso IDE**: seleccionar cualquier proyecto de la KL46Z, abrir **GUI Flash Tool**, elegir la sonda y en **Program** cargar `build\blinky.elf`. Al terminar, presionar **RESET**: el LED rojo parpadea.

> ⚠️ Si la placa se conecta con **RESET presionado** aparece la unidad **BOOTLOADER**: es para actualizar el firmware del depurador, no para el programa. No copiar el blinky ahí.

## Problemas frecuentes

| Síntoma | Solución |
|---|---|
| *"no se reconoce como un comando interno o externo"* | La herramienta no está en el PATH: ejecutar `entorno` (y revisar sus rutas). |
| `No targets specified and no makefile found` | El cmd no está en la carpeta del proyecto: con `dir` tiene que aparecer el `Makefile`. |
| Los comandos con `%CPU%` fallan | Se está usando PowerShell: abrir el Símbolo del sistema (cmd). |
| `make` falla al crear carpetas | `cmake` no está en el PATH: el `Makefile` usa `cmake -E`. |
| CMake: *unrecognized command-line option '-mthumb'* | Faltó `-DCMAKE_TOOLCHAIN_FILE=...`: borrar la carpeta de build y configurar de nuevo. |
| Errores con rutas extrañas | Mover el proyecto a una ruta sin espacios ni acentos. |

---

## Material relacionado

- [**FreeRTOS_ESP32-S3**](https://github.com/sea2-dsi-fceia/FreeRTOS_ESP32-S3): FreeRTOS sobre ESP32-S3 con ESP-IDF, que usa CMake + Ninja y Kconfig. La segunda parte de la presentación se apoya en ese repositorio.
- [**FreeRTOS_Workspace**](https://github.com/sea2-dsi-fceia/FreeRTOS_Workspace): ejemplos de FreeRTOS sobre la FRDM-KL46Z con MCUXpresso. En la carpeta `Debug` de cualquiera de sus proyectos se ven los makefiles, el linker script y el `.map` que genera el IDE.

## Referencias

- [GCC — opciones de compilación y optimización](https://gcc.gnu.org/onlinedocs/)
- [GNU Make Manual](https://www.gnu.org/software/make/manual/)
- [GNU ld — linker scripts](https://sourceware.org/binutils/docs/ld/)
- [CMake — toolchains](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)
- [Ninja](https://ninja-build.org/manual.html)
- NXP — *KL46 Sub-Family Reference Manual* y *FRDM-KL46Z User's Guide*

---

Sistemas Embebidos Avanzados II — Departamento de Sistemas e Informática — EIE — FCEIA — UNR
Prof. Ing. Daniel Márquez — [dmarquez@fceia.unr.edu.ar](mailto:dmarquez@fceia.unr.edu.ar)
