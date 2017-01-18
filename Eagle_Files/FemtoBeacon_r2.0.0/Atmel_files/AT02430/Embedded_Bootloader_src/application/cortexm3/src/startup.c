/**************************************************************************//**
\file  startup.c

\brief Implementation of the start up code.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    29/08/11 A. Khromykh - Created
*******************************************************************************/

/*******************************************************************************
                   Includes section
*******************************************************************************/
#include <AT91SAM3S4.h>
#include <core_cm3.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define START_ADDRESS_MASK    0x2FFFFFF8UL

/*******************************************************************************
                     External variables section
*******************************************************************************/
#ifdef  __IAR_SYSTEMS_ICC__
  extern uint32_t __ICFEDIT_vector_start__;
#elif __GNUC__
  extern uint32_t __data;
  extern uint32_t __data_end;
  extern uint32_t __data_load;
  extern uint32_t __bss;
  extern uint32_t __bss_end;
  extern uint32_t __vectors;
#endif

/*******************************************************************************
                   Extern functions prototypes section
*******************************************************************************/
#ifdef  __IAR_SYSTEMS_ICC__
#elif __GNUC__
  extern int main(void) __attribute__((noreturn));
#endif

/******************************************************************************
                              Implementations section
******************************************************************************/
#ifdef  __IAR_SYSTEMS_ICC__
/**************************************************************************//**
\brief Specify function for low level initialization. It needs for IAR.
The function must return 1 to initialize variable section

\return
  1 - always.
******************************************************************************/
int __low_level_init(void)
{
  // Remap vector table to real location
  SCB->VTOR = ((uint32_t)&__ICFEDIT_vector_start__) & START_ADDRESS_MASK;

  return 1; /*  if return 0, the data sections will not be initialized. */
}
#elif __GNUC__
/**************************************************************************//**
\brief Setup environment, do low-level initialization and proceed to main
******************************************************************************/
void gccReset(void)
{
  // Remap vector table to real location
  SCB->VTOR = ((uint32_t)&__vectors) & START_ADDRESS_MASK;

  // Relocate .data section
  for (uint32_t *from = &__data_load, *to = &__data; to < &__data_end; from++, to++)
  {
    *to = *from;
  }

  // Clear .bss
  for (uint32_t *to = &__bss; to < &__bss_end; to++)
  {
    *to = 0;
  }

  main();
}
#endif

// eof startup.c
