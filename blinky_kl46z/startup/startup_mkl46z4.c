/*
 * Sistemas Embebidos Avanzados II - DSI - EIE - FCEIA - UNR
 * Startup mínimo para el MKL46Z256 (Cortex-M0+), escrito en C para que se lea.
 *
 * Contiene:
 *   1. La tabla de vectores (sección .isr_vector, en 0x00000000).
 *   2. La configuración de la flash de Kinetis (sección .flash_config, en 0x400).
 *   3. Reset_Handler: apaga el watchdog, inicializa .data y .bss y llama a main().
 */

#include <stdint.h>

/* Símbolos definidos en el linker script (linker/mkl46z256.ld) */
extern uint32_t _estack;   /* tope de la RAM: valor inicial del SP          */
extern uint32_t _sidata;   /* dirección en FLASH de los valores de .data    */
extern uint32_t _sdata;    /* inicio de .data en RAM                        */
extern uint32_t _edata;    /* fin de .data en RAM                           */
extern uint32_t _sbss;     /* inicio de .bss en RAM                         */
extern uint32_t _ebss;     /* fin de .bss en RAM                            */

extern int main(void);

/* Registro del watchdog COP: viene HABILITADO al salir de reset */
#define SIM_COPC  (*(volatile uint32_t *) 0x40048100u)

void Reset_Handler(void);
void Default_Handler(void);

/* Los handlers del sistema son "weak": el programa puede redefinirlos */
void NMI_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)   __attribute__((weak, alias("Default_Handler")));

/* ---------- 1. Tabla de vectores: 16 del sistema + 32 interrupciones = 48 ---------- */
__attribute__((used, section(".isr_vector")))
void (* const tabla_vectores[48])(void) = {
    (void (*)(void)) &_estack,  /*  0: valor inicial del SP          */
    Reset_Handler,              /*  1: Reset                         */
    NMI_Handler,                /*  2: NMI                           */
    HardFault_Handler,          /*  3: HardFault                     */
    0, 0, 0, 0, 0, 0, 0,        /*  4-10: reservados en Cortex-M0+   */
    SVC_Handler,                /* 11: SVCall                        */
    0, 0,                       /* 12-13: reservados                 */
    PendSV_Handler,             /* 14: PendSV                        */
    SysTick_Handler,            /* 15: SysTick                       */
    /* 16-47: las 32 interrupciones de periféricos (IRQ0..IRQ31) */
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler
};

/* ---------- 2. Flash configuration field (0x400 a 0x40F), propio de Kinetis ----------
 * Bytes 0-7: backdoor key | 8-11: FPROT | 12: FSEC | 13: FOPT | 14: FEPROT | 15: FDPROT
 * FSEC = 0xFE deja el chip SIN proteger. ¡Un valor equivocado puede bloquearlo!
 */
__attribute__((used, section(".flash_config")))
const uint32_t configuracion_flash[4] = {
    0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFEu
};

/* ---------- 3. Reset_Handler: lo primero que se ejecuta ---------- */
void Reset_Handler(void)
{
    /* volatile: sin esto, con -Os el compilador reemplaza los lazos por llamadas
       a memcpy() y memset(), que no existen porque linkeamos con -nostdlib */
    volatile uint32_t *origen;
    volatile uint32_t *destino;

    /* a) Apagar el watchdog COP; si no, el micro se resetea solo cada ~1 s */
    SIM_COPC = 0;

    /* b) Copiar los valores iniciales de .data desde la FLASH a la RAM */
    origen = &_sidata;
    destino = &_sdata;
    while (destino < &_edata) {
        *destino = *origen;
        destino = destino + 1;
        origen = origen + 1;
    }

    /* c) Poner .bss en cero */
    destino = &_sbss;
    while (destino < &_ebss) {
        *destino = 0;
        destino = destino + 1;
    }

    /* d) Recién ahora, main() */
    main();

    /* main() no debería retornar */
    while (1) {
    }
}

void Default_Handler(void)
{
    while (1) {
    }
}
