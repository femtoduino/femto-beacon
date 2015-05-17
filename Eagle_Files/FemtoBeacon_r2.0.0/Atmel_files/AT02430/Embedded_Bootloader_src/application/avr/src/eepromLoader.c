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
#include <sp_driver.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define EEPROM_ADDRESS_MASK             0x0000FFFF

/******************************************************************************
                   External global variables section
******************************************************************************/
extern BootBuffer_t dataBuffer;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Writes a page to the EEPROM.

\param[in]
  address - address of data byte.
\param[in]
  data - data for saving.
******************************************************************************/
static inline void eepromWriteData(uint16_t address, uint8_t data)
{
  EEAR = address;
  EEDR = data;
  // master programming enable
  EECR = (1 << EEMPE);
  // programming enable
  EECR |= (1 << EEPE);
  // Wait for completion of previous write.
  while (EECR & (1 << EEPE));
}

/**************************************************************************\\**
\brief Write eeprom page and counting address and length for next write
******************************************************************************/
void eepromPacketToPage(void)
{
  uint8_t *dataPointer;

  dataPointer = dataBuffer.data;
  dataBuffer.address &= EEPROM_ADDRESS_MASK;

  while (dataBuffer.dataSize)
  {
    // Do a Erase-Write of the page.
    eepromWriteData(dataBuffer.address++, *dataPointer++);
    dataBuffer.dataSize--;
  }
}

/**************************************************************************//**
\brief Flush unloaded data before exit from bootloader.
******************************************************************************/
void eepromFlushPage(void)
{
}

// eof eepromLoader.c
