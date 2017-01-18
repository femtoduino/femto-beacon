/****************************************************************************//**
  \file flashLoader.c

  \brief Implementation of internal flash loader.

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
#include <sp_driver.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define PAGE_NUMBER_SHIFT               9
#define PAGE_POSITION_OFFSET_MASK   0x1FF

/******************************************************************************
                   Types section
******************************************************************************/
typedef union
{
  uint8_t  byteData[2];
  uint16_t wordData;
} FlashWord_t;

/******************************************************************************
                   External global variables section
******************************************************************************/
extern BootBuffer_t dataBuffer;

/******************************************************************************
                   Global variables section
******************************************************************************/
uint16_t currentPage = 0;

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Function to load one word into page buffer.

\param[in]
  tableAddress - address in buffer to write the word.
\param[in]
  dataByte     - the byte of the word to load.
******************************************************************************/
void flashLoadPageBuffer(uint16_t tableAddress, uint8_t dataByte)
{
  static FlashWord_t  data;

  if (tableAddress & 0x01)
  {
    data.byteData[1] = dataByte;
    // Perform word load into page buffer and wait for end of operation.
    SP_LoadFlashWord(tableAddress-1, data.wordData);
  }
  else
  {
    data.byteData[0] = dataByte;
  }
}

/**************************************************************************//**
\brief Function to do an atomic erase-write on one page in the flash.

\note The maximum pageAddress must not be exceeded. The maximum number of
      pages can be found in the datasheet.

\param[in] pageAddress Page address to the page to erase/write.
******************************************************************************/
void flashEraseWritePage(uint16_t pageAddress)
{
  // Calculate actual start address of the page.
  uint32_t tableAddress = (uint32_t)pageAddress * (uint32_t)FLASH_PAGE_SIZE;

  // Perform page erase.
  SP_EraseWriteApplicationPage(tableAddress);
}

/**************************************************************************//**
\brief Load received date to the flash.
******************************************************************************/
void flashPacketToPage(void)
{
  uint16_t packetPage;
  uint16_t tableAddress;
  uint8_t *dataPointer;

  dataPointer = dataBuffer.data;
  // Data size is added to even value to the write word data to the temporary buffer.
  if (dataBuffer.dataSize & 0x01) // check data size is odd or not
  {
    dataBuffer.data[dataBuffer.dataSize] = 0xFF; // fake byte for odd srec record
    dataBuffer.dataSize++;
  }

  while (dataBuffer.dataSize)
  {
    packetPage = dataBuffer.address >> PAGE_NUMBER_SHIFT;

    if (currentPage != packetPage)
    {
      // Do a Erase-Write of the page.
      flashEraseWritePage(currentPage);
      currentPage = packetPage;
    }

    tableAddress = dataBuffer.address & PAGE_POSITION_OFFSET_MASK;
    flashLoadPageBuffer(tableAddress, *dataPointer++);
    dataBuffer.address++;
    dataBuffer.dataSize--;
  }
}

/**************************************************************************//**
\brief Flush unloaded data before exit from bootloader.
******************************************************************************/
void flashFlushPage(void)
{
  // Do a Erase-Write of the page.
  flashEraseWritePage(currentPage);
}

/******************************************************************************
Interrupt handler.
******************************************************************************/
ISR(NVM_SPM_vect)
{
  //disable flash interrupt
  NVM.INTCTRL = 0;
}

// eof flashLoader.c
