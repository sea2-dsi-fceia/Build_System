/*
 * Sistemas Embebidos Avanzados II - DSI - EIE - FCEIA - UNR
 * Linker script mínimo para el MKL46Z256: 256 KB de FLASH, 32 KB de RAM.
 *
 * Describe el chip: qué memorias hay, dónde va cada sección y qué símbolos
 * necesita el startup (_estack, _sidata, _sdata, _edata, _sbss, _ebss).
 */

ENTRY(Reset_Handler)

MEMORY
{
  VECTORES (rx)  : ORIGIN = 0x00000000, LENGTH = 0x400
  FLASHCFG (rx)  : ORIGIN = 0x00000400, LENGTH = 0x10
  FLASH    (rx)  : ORIGIN = 0x00000410, LENGTH = 256K - 0x410
  RAM      (rwx) : ORIGIN = 0x1FFFE000, LENGTH = 32K
}

TAMANO_STACK = 0x400;                       /* 1 KB de stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);        /* 0x20006000: tope de la RAM */

SECTIONS
{
  /* Tabla de vectores en 0x0. KEEP: nadie la referencia desde el código,
     pero --gc-sections no la debe descartar */
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > VECTORES

  /* Configuración de la flash de Kinetis, exactamente en 0x400 */
  .flash_config :
  {
    KEEP(*(.flash_config))
  } > FLASHCFG

  /* Código y constantes */
  .text :
  {
    *(.text*)
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

  /* .data: vive en RAM, pero sus valores iniciales se guardan en FLASH */
  _sidata = LOADADDR(.data);
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > RAM AT> FLASH

  /* .bss: solo en RAM; el startup la pone en cero */
  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  /* Reserva para el stack: si no entra en la RAM, el linker da error */
  .stack (NOLOAD) :
  {
    . = ALIGN(8);
    . = . + TAMANO_STACK;
  } > RAM
}
