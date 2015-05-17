/****************************************************************************//**
  \file hardwareInit.c

  \brief Hardware initialization routine.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15.10.09 A. Taradov - Created
*******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <hardwareInit.h>
#if defined(EXT_MEMORY)
  #include <extMemReader.h>
#endif

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Startup initialization (frequency, io, usart)
******************************************************************************/
void lowLevelInit(void)
{ // mcu works from 2 MHz internal oscillator after reset
  // enable extended addressing if it is needded
  SET_EIND();

  // enable eeprom to ram memory mapping
  NVM.CTRLB |=  NVM_EEMAPEN_bm;

  wdt_reset();
  // Enable watch dog timer.  Make wdt interval 8s
  ENABLE_CHANGE_PROTECTION_REGISTER;
  WDT.CTRL = WDT_PER_8KCLK_gc | WDT_ENABLE_bm | WDT_CEN_bm;
  while (WDT.STATUS & WDT_SYNCBUSY_bm);

  // Enable high priority interrupts and replaced vectors into the boot section.
  ENABLE_CHANGE_PROTECTION_REGISTER;
  PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_IVSEL_bm;
  sei();

#if defined(EXT_MEMORY)
  // Point of enter to loader from external memory.
  extLoadDataFromExtMem();
#endif
}

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void lowLevelUnInit(void)
{ // mcu works from 2 MHz internal oscillator after reset
  // disable wdt
  ENABLE_CHANGE_PROTECTION_REGISTER;
  WDT.CTRL = WDT_CEN_bm;

  // disable eeprom to ram memory mapping
  NVM.CTRLB &= ~NVM_EEMAPEN_bm;

  // Disable all interrupts and replaced vectors into the application section.
  ENABLE_CHANGE_PROTECTION_REGISTER;
  PMIC.CTRL = 0;
  cli();
}

// eof hardwareInit.c
