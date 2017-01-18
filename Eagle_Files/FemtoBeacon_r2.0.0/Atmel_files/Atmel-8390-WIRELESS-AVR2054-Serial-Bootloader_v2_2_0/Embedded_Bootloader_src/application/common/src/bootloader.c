/****************************************************************************//**
  \file bootloader.c

  \brief Implementation of bootloader state machine.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
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
  srecHandshake();
  srecProtocol();
  return 1;
}

// eof bootloader.c
