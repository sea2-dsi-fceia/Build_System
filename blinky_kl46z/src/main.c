/*
 * Sistemas Embebidos Avanzados II - DSI - EIE - FCEIA - UNR
 * Blinky bare-metal para la FRDM-KL46Z (MKL46Z256VLL4)
 *
 * Sin SDK ni HAL: los registros se acceden por su dirección,
 * tomada del Reference Manual del KL46.
 *
 * Hace parpadear el LED rojo de la placa (PTE29, activo en bajo).
 */

#include <stdint.h>

/* ---------- Registros (KL46 Sub-Family Reference Manual) ---------- */

/* SIM: habilitación de relojes de los puertos */
#define SIM_SCGC5        (*(volatile uint32_t *) 0x40048038u)
#define SIM_SCGC5_PORTE  (1u << 13)

/* PORTE: Pin Control Register del pin 29 */
#define PORTE_PCR29      (*(volatile uint32_t *) (0x4004D000u + 4u * 29u))
#define PORT_PCR_MUX_GPIO (1u << 8)            /* MUX = 001: el pin funciona como GPIO */

/* GPIOE */
#define GPIOE_PSOR       (*(volatile uint32_t *) 0x400FF104u)   /* pone pines en 1      */
#define GPIOE_PTOR       (*(volatile uint32_t *) 0x400FF10Cu)   /* invierte pines       */
#define GPIOE_PDDR       (*(volatile uint32_t *) 0x400FF114u)   /* dirección: 1 = salida */

#define LED_ROJO         (1u << 29)

/* ---------- Variables globales ---------- */

uint32_t demora_ciclos = 300000;     /* con valor inicial -> sección .data */
uint32_t contador_parpadeos;         /* sin valor inicial -> sección .bss  */

/* ---------- Funciones ---------- */

static void demora(uint32_t ciclos)
{
    volatile uint32_t i;

    for (i = 0; i < ciclos; i++) {
        /* espera activa */
    }
}

/* Nadie la llama: sirve para ver qué hace --gc-sections */
void funcion_no_usada(void)
{
    contador_parpadeos = 0;
}

int main(void)
{
    SIM_SCGC5 = SIM_SCGC5 | SIM_SCGC5_PORTE;   /* 1. reloj del puerto E      */
    PORTE_PCR29 = PORT_PCR_MUX_GPIO;            /* 2. PTE29 como GPIO          */
    GPIOE_PSOR = LED_ROJO;                      /* 3. LED apagado (activo bajo) */
    GPIOE_PDDR = GPIOE_PDDR | LED_ROJO;         /* 4. PTE29 como salida        */

    while (1) {
        GPIOE_PTOR = LED_ROJO;                  /* invierte el LED */
        contador_parpadeos = contador_parpadeos + 1;
        demora(demora_ciclos);
    }
}
