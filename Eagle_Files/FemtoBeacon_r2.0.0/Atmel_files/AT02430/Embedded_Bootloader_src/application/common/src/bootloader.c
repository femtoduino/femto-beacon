/****************************************************************************//**
  \file bootloader.c

  \brief Implementation of bootloader state machine.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2/04/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <bootStructure.h>
#include <abstractSerializer.h>
#include <hardwareInit.h>

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief State machine in the main
******************************************************************************/
int main(void)
{
  lowLevelInit();
  /* Protocol for TWI Interface is as per AVR1624 */
#if (USE_TWI == 1) || (USE_TWIC == 1) || (USE_TWID == 1) || (USE_TWIE == 1) || (USE_TWIF ==1)  
  twiBootHandshake();
  twiBootCheck();
#else
  srecHandshake();
  srecProtocol();
#endif  
  return 1;
}
// eof bootloader.c
