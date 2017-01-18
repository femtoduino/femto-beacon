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
                   Define(s) section
******************************************************************************/
/** \brief the eeprom base address in ram space after remaping */
#define EEPROM_BASE_ADDRESS             0x1000u
/** \brief eeprom page size */
#define EEPROM_PAGE_SIZE_BYTES          32
/** \brief number of pages */
#define EEPROM_PAGE_NUMBER              64
#define POWER_OF_TWO_EEPROM_PAGE_SIZE   5
#define EEPROM_ADDRESS_MASK             0x0000FFFF

/******************************************************************************
                   External global variables section
******************************************************************************/
extern BootBuffer_t dataBuffer;

/******************************************************************************
                   Global variables section
******************************************************************************/
uint8_t eepCurrentPage = 0;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Writes a page to the EEPROM.

\param[in]
  pageNumber - page number for saving.
******************************************************************************/
void eepromEraseWritePage(uint8_t pageNumber)
{
  uint16_t address;

  // Calculate page address
  address = (uint16_t)pageNumber * EEPROM_PAGE_SIZE_BYTES;
  // Set address.
  NVM.ADDR0 = address & 0xFF;
  NVM.ADDR1 = (address >> 8) & 0x1F;
  NVM.ADDR2 = 0x00;
  // set up IDLE sleep mode and enable sleep
  SLEEP.CTRL = SLEEP_SMODE_IDLE_gc | SLEEP_SEN_bm;
  // EEPROM Erase and Write command.
  NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
  // Enable change of protected IO register
  CCP = 0xD8;
  // start command
  NVM.CTRLA = NVM_CMDEX_bm;
  // enable eeprom ready interrupt as high level interrupt
  NVM.INTCTRL = NVM_EELVL_HI_gc;
  // go to sleep
  __SLEEP;
  // clear sleep parameters
  SLEEP.CTRL = 0;
}

/**************************************************************************\\**
\brief Write eeprom page and counting address and length for next write
******************************************************************************/
void eepromPacketToPage(void)
{
  uint8_t packetPage;
  uint8_t *dataPointer;

  dataPointer = dataBuffer.data;
  dataBuffer.address &= EEPROM_ADDRESS_MASK;

  while (dataBuffer.dataSize)
  {
    packetPage = dataBuffer.address >> POWER_OF_TWO_EEPROM_PAGE_SIZE;

    if (eepCurrentPage != packetPage)
    {
      // Do a Erase-Write of the page.
      eepromEraseWritePage(eepCurrentPage);
      eepCurrentPage = packetPage;
    }

    *((uint8_t *)EEPROM_BASE_ADDRESS + dataBuffer.address) = *dataPointer++;
    dataBuffer.address++;
    dataBuffer.dataSize--;
  }
}

/**************************************************************************//**
\brief Flush unloaded data before exit from bootloader.
******************************************************************************/
void eepromFlushPage(void)
{
  // Do a Erase-Write of the page.
  eepromEraseWritePage(eepCurrentPage);
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
  return *((uint8_t *)(EEPROM_BASE_ADDRESS + address));
}

/******************************************************************************
Interrupt handler.
******************************************************************************/
ISR(NVM_EE_vect)
{
  //disable eeprom interrupt
  NVM.INTCTRL = 0;
}

// eof eepromLoader.c
