/**************************************************************************//**
\file  vectorTable.c

\brief Cortex-M3 vector table.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    06/10/10 D. Loskutnikov - Created
*******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <AT91SAM3S4.h>

/******************************************************************************
                   Types section
******************************************************************************/
//! Function prototype for exception table items (interrupt handler).
typedef void(* IntFunc_t)(void);

typedef union
{
  IntFunc_t __fun;
  void *__ptr;
} IntVector_t;

/*******************************************************************************
                   Prototypes section
*******************************************************************************/
extern void lowLevelInit(void);
extern void __iar_program_start(void);
extern void gccReset(void);
static void irqHandlerNotUsed(void);

/*******************************************************************************
                     External variables section
*******************************************************************************/
extern uint32_t __main_stack_top;

/******************************************************************************
                              Constants section
******************************************************************************/

// IAR really wants this variable to be named __vector_table
#ifdef  __IAR_SYSTEMS_ICC__
  #pragma segment="CSTACK"
  #define STACK_TOP  __sfe("CSTACK")
  #define RESET_ADDR  __iar_program_start

  #pragma language=extended
  #pragma section = ".vectors"
  #pragma location = ".vectors"
#elif __GNUC__
  #define STACK_TOP   &__main_stack_top
  #define RESET_ADDR  gccReset

  __attribute__ ((section(".vectors"), used))
#else
  #error "Unsupported compiler"
#endif
const IntVector_t __vector_table[] =
{
    {.__ptr = STACK_TOP},
    {RESET_ADDR},

    {irqHandlerNotUsed},
    {irqHandlerNotUsed},
    {irqHandlerNotUsed},
    {irqHandlerNotUsed},
    {irqHandlerNotUsed},
    {0}, {0}, {0}, {0},       /*  Reserved */
    {irqHandlerNotUsed},
    {irqHandlerNotUsed},
    {0},                      /*  Reserved */
    {irqHandlerNotUsed},
    {irqHandlerNotUsed},

    /*  Configurable interrupts */
    {irqHandlerNotUsed},     /*  0  SUPPLY CONTROLLER */
    {irqHandlerNotUsed},     /*  1  RESET CONTROLLER */
    {irqHandlerNotUsed},     /*  2  REAL TIME CLOCK */
    {irqHandlerNotUsed},     /*  3  REAL TIME TIMER */
    {irqHandlerNotUsed},     /*  4  WATCHDOG TIMER */
    {irqHandlerNotUsed},     /*  5  PMC */
    {irqHandlerNotUsed},     /*  6  EEFC */
    {irqHandlerNotUsed},     /*  7  Reserved */
    {irqHandlerNotUsed},     /*  8  UART0 */
    {irqHandlerNotUsed},     /*  9  UART1 */
    {irqHandlerNotUsed},     /*  10 SMC */
    {irqHandlerNotUsed},     /*  11 Parallel IO Controller A */
    {irqHandlerNotUsed},     /*  12 Parallel IO Controller B */
    {irqHandlerNotUsed},     /*  13 Parallel IO Controller C */
    {irqHandlerNotUsed},     /*  14 USART 0 */
    {irqHandlerNotUsed},     /*  15 USART 1 */
    {irqHandlerNotUsed},     /*  16 Reserved */
    {irqHandlerNotUsed},     /*  17 Reserved */
    {irqHandlerNotUsed},     /*  18 MCI */
    {irqHandlerNotUsed},     /*  19 TWI 0 */
    {irqHandlerNotUsed},     /*  20 TWI 1 */
    {irqHandlerNotUsed},     /*  21 SPI */
    {irqHandlerNotUsed},     /*  22 SSC */
    {irqHandlerNotUsed},     /*  23 Timer Counter 0 */
    {irqHandlerNotUsed},     /*  24 Timer Counter 1 */
    {irqHandlerNotUsed},     /*  25 Timer Counter 2 */
    {irqHandlerNotUsed},     /*  26 Timer Counter 3 */
    {irqHandlerNotUsed},     /*  27 Timer Counter 4 */
    {irqHandlerNotUsed},     /*  28 Timer Counter 5 */
    {irqHandlerNotUsed},     /*  29 ADC controller */
    {irqHandlerNotUsed},     /*  30 DAC controller */
    {irqHandlerNotUsed},     /*  31 PWM */
    {irqHandlerNotUsed},     /*  32 CRC Calculation Unit */
    {irqHandlerNotUsed},     /*  33 Analog Comparator */
    {irqHandlerNotUsed},     /*  34 USB Device Port */
    {irqHandlerNotUsed}      /*  35 not used */
};

/******************************************************************************
                              Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Default interrupt handler for not used irq.
******************************************************************************/
static void irqHandlerNotUsed(void)
{
  while(1);
}
// eof vectorTable.c
