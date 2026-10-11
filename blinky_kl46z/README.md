# Blinky bare-metal para la FRDM-KL46Z

**Sistemas Embebidos Avanzados II** — Departamento de Sistemas e Informática — EIE — FCEIA — UNR
Prof. Ing. Daniel Márquez — dmarquez@fceia.unr.edu.ar

Ejemplo de la clase de **build systems**: un LED que parpadea, **sin SDK ni HAL**. Todo lo que termina en el binario está en estos archivos, así que se puede compilar a mano, con **Make** y con **CMake + Ninja**, y ver qué hace cada herramienta.

```
blinky_kl46z/
├── src/main.c                    Programa: hace parpadear el LED rojo (PTE29)
├── startup/startup_mkl46z4.c     Tabla de vectores, flash config y Reset_Handler
├── linker/mkl46z256.ld           Linker script: memorias y ubicación de cada sección
├── Makefile                      Build con Make
├── CMakeLists.txt                Build con CMake
└── cmake/arm-none-eabi.cmake     Toolchain file de CMake
```

---

## Herramientas (Windows)

Hacen falta cuatro programas en el `PATH`. Verificar cada uno con `--version`.

| Herramienta | De dónde sacarla |
|---|---|
| `arm-none-eabi-gcc` | La trae el **MCUXpresso IDE**, en `C:\nxp\MCUXpressoIDE_<versión>\ide\tools\bin`. También se puede instalar el *Arm GNU Toolchain* desde developer.arm.com, marcando *Add path to environment variable*. |
| `make` | El MCUXpresso IDE trae uno en `C:\nxp\MCUXpressoIDE_<versión>\ide\buildtools\bin`. Si no está: `winget install ezwinports.make` |
| `cmake` | `winget install Kitware.CMake` |
| `ninja` | `winget install Ninja-build.Ninja` |

Si ESP-IDF está instalado con el EIM, `cmake` y `ninja` ya están en la PC, en `C:\Espressif\tools\cmake\<versión>\bin` y `C:\Espressif\tools\ninja\<versión>`.

**Configurar el `PATH` con `entorno.bat`:**

1. Abrir `entorno.bat` con el Bloc de notas y ajustar las tres rutas a las de la PC. Para encontrarlas:
   ```bat
   where /r C:\nxp arm-none-eabi-gcc.exe
   where /r C:\Espressif cmake.exe
   where /r C:\Espressif ninja.exe
   ```
2. Abrir **cmd** en la carpeta del proyecto (en el Explorador, escribir `cmd` en la barra de direcciones y presionar Enter) y ejecutar:
   ```bat
   entorno
   ```
   Muestra dónde encontró cada herramienta. El `PATH` vale **solo para esa ventana**: al abrir otra, hay que volver a ejecutar `entorno`.

> Usar **cmd** y no PowerShell: los comandos de la demo 1 usan la sintaxis de variables de cmd (`set` y `%CPU%`).

> El proyecto tiene que estar en una ruta **sin espacios ni acentos**, por ejemplo `C:\sea2\blinky_kl46z`.

---

## Demo 1 — El build a mano

Desde la carpeta del proyecto:

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

| Archivo | Qué mirar |
|---|---|
| `tmp/main.i` | Ya no hay `#include` ni `#define`: el preprocesador los reemplazó (unas 100 líneas). |
| `tmp/main.s` | El assembler Thumb del Cortex-M0+. Buscar `main:`. |
| `tmp/main.o` | `arm-none-eabi-nm tmp/main.o`: los símbolos todavía no tienen dirección (todos en `00000000`). |
| `tmp/blinky.elf` | `arm-none-eabi-nm -n tmp/blinky.elf`: ahora sí, `main` en `0x00000410`, `demora_ciclos` en `0x1FFFE000`. |

**Probar:** sacar `-nostdlib` del comando de link. Falla con `undefined reference to '__bss_start__'`: entra el startup de newlib (`crt0`), que espera símbolos que nuestro linker script no define.

---

## Demo 2 — Make

```bat
make              :: compila todo y genera build/blinky.bin
make              :: "Nothing to be done": todo está al día
make size         :: uso de FLASH y RAM
make clean        :: borra build/
make DEBUG=1      :: compila con -O0 -g3
```

Para ver el **build incremental**, actualizar la fecha de `src/main.c` sin cambiar su contenido (es el equivalente de `touch` en el cmd) y volver a compilar:

```bat
copy /b src\main.c +,,
make
```

Solo recompila `main.c` y vuelve a linkear.

**Trampa:** `make DEBUG=1` justo después de `make`, sin `make clean`, no recompila nada. Los `.o` son más nuevos que los `.c`, y Make no ve que cambiaron los flags.

---

## Demo 3 — CMake + Ninja

```bat
cmake -B build-release -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Release
cmake -B build-debug   -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build build-release
cmake --build build-debug
```

Cada tipo de build tiene su propia carpeta: Debug y Release conviven sin pisarse. Al final de cada build se muestra el tamaño.

**Probar:** configurar sin `-DCMAKE_TOOLCHAIN_FILE=...`. CMake usa el compilador de la PC, que no entiende `-mthumb`, y la compilación falla.

---

## Tamaños medidos (`arm-none-eabi-size`, GCC 13.2)

| Variante | text | data | bss |
|---|---:|---:|---:|
| Release `-Os` + `--gc-sections` | 380 | 4 | 1028 |
| Release sin `--gc-sections` | 392 | 4 | 1028 |
| Debug `-O0` + `--gc-sections` | 472 | 4 | 1028 |

- Los **12 bytes** de diferencia sin `--gc-sections` son `funcion_no_usada()`, que nadie llama.
- **FLASH** = text + data; **RAM** = data + bss. Los 1028 bytes de bss incluyen el stack de 1 KB.
- Con otra versión de GCC los números pueden cambiar levemente.

## Mapa de memoria (Release)

| Sección | Dirección | Tamaño | Contenido |
|---|---|---:|---|
| `.isr_vector` | `0x00000000` | 192 B | 48 vectores (16 del sistema + 32 IRQ) |
| `.flash_config` | `0x00000400` | 16 B | Configuración de la flash (FSEC = 0xFE: sin proteger) |
| `.text` | `0x00000410` | 172 B | Código |
| `.data` (copia en FLASH) | `0x000004BC` | 4 B | Valor inicial de `demora_ciclos` |
| `.data` | `0x1FFFE000` | 4 B | `demora_ciclos` |
| `.bss` | `0x1FFFE004` | 4 B | `contador_parpadeos` |
| `.stack` | `0x1FFFE008` | 1 KB | Stack (crece hacia abajo desde `_estack = 0x20006000`) |

Para verlo: `arm-none-eabi-objdump -h build/blinky.elf` y el archivo `build/blinky.map`.

---

## Grabar en la placa

**Con el MCUXpresso IDE** (la forma habitual en la materia):

1. Abrir el MCUXpresso y seleccionar cualquier proyecto de la KL46Z en el *Project Explorer* (le indica al IDE el microcontrolador).
2. Hacer clic en **GUI Flash Tool** y elegir la sonda detectada.
3. En **Program**, *File to program*: elegir `build\blinky.elf` (con el `.elf` no hace falta indicar la dirección).
4. **Run** y, al terminar, presionar **RESET** en la placa. El LED **rojo** debe parpadear.

**Arrastrando el archivo:** solo si la placa tiene una aplicación OpenSDA que lo permita (MSD, mbed o DAPLink) y aparece como unidad USB al conectarla normalmente. En ese caso se copia `build/blinky.bin` a esa unidad.

> ⚠️ Si la placa se conecta con RESET presionado aparece la unidad **BOOTLOADER**: es para actualizar el firmware del **depurador**, no para el programa. No copiar el blinky ahí.

---

## Ejercicios

1. Separar el manejo del LED en `led.c` / `led.h` y agregarlo al `Makefile` y al `CMakeLists.txt`.
2. Achicar la RAM a `1K` en el linker script y explicar el error del linker (`region 'RAM' overflowed by 8 bytes`).
3. Compilar sin `-nostdlib`. ¿De dónde sale `__bss_start__`?
4. En el desensamblado de `Reset_Handler` (`arm-none-eabi-objdump -d build/blinky.elf`), encontrar los lazos que copian `.data` y borran `.bss`.
5. Sacar el `volatile` de los punteros de `Reset_Handler` y compilar. ¿Por qué aparecen `memcpy` y `memset`?
