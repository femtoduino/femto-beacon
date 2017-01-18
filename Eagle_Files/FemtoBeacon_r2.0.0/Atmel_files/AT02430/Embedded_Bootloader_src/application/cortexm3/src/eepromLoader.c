/****************************************************************************//**
  \file eepromLoader.c

  \brief Implementation of internal eeprom loader.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    3/04/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <bootStructure.h>

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************\\**
\brief Write eeprom page and counting address and length for next write
******************************************************************************/
void eepromPacketToPage(void)
{
}

/**************************************************************************//**
\brief Flush unloaded data before exit from bootloader.
******************************************************************************/
void eepromFlushPage(void)
{
}

/**************************************************************************//**
\brief Reads data from the EEPROM.

\param[in]
  address - address of data byte.
\return
  read byte
******************************************************************************/
uint8_t eepromReadData(uint16_t address)
{
  (void)address;
  return 0;
}

// eof eepromLoader.c
