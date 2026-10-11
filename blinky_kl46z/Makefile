# Sistemas Embebidos Avanzados II - DSI - EIE - FCEIA - UNR
# Makefile del blinky bare-metal para la FRDM-KL46Z
#
# Uso:
#   make            compila en modo Release (-Os)
#   make DEBUG=1    compila en modo Debug (-O0 -g3)   ¡hacer "make clean" antes!
#   make size       muestra el uso de FLASH y RAM
#   make clean      borra la carpeta build
#
# Para crear y borrar carpetas se usa "cmake -E", que funciona igual en
# Windows (cmd o PowerShell) y en cualquier otro sistema.

# ---------- Toolchain ----------
PREFIJO  = arm-none-eabi-
CC       = $(PREFIJO)gcc
OBJCOPY  = $(PREFIJO)objcopy
SIZE     = $(PREFIJO)size

# ---------- Archivos ----------
BUILD    = build
FUENTES  = src/main.c startup/startup_mkl46z4.c
LDSCRIPT = linker/mkl46z256.ld
OBJETOS  = $(addprefix $(BUILD)/,$(FUENTES:.c=.o))

# ---------- Flags ----------
CPU      = -mcpu=cortex-m0plus -mthumb

ifeq ($(DEBUG),1)
OPT      = -O0 -g3
else
OPT      = -Os -g
endif

CFLAGS   = $(CPU) $(OPT) -Wall -ffunction-sections -fdata-sections -MMD -MP
LDFLAGS  = $(CPU) -T $(LDSCRIPT) -nostdlib -Wl,--gc-sections -Wl,-Map=$(BUILD)/blinky.map

# ---------- Reglas ----------
all: $(BUILD)/blinky.bin

$(BUILD)/blinky.bin: $(BUILD)/blinky.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/blinky.elf: $(OBJETOS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(OBJETOS) -o $@

$(BUILD)/%.o: %.c Makefile
	@cmake -E make_directory $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

size: $(BUILD)/blinky.elf
	$(SIZE) $<

clean:
	cmake -E rm -rf $(BUILD)

.PHONY: all size clean

# Dependencias de headers generadas por gcc (-MMD -MP)
-include $(OBJETOS:.o=.d)
