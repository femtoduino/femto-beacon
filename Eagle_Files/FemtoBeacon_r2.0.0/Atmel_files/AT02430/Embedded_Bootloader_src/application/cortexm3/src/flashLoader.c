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
    07.09.11 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <bootStructure.h>
#include <AT91SAM3S4.h>
#include <core_cm3.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define FLASH_PAGE_SIZE               256
#define PAGE_NUMBER_SHIFT               8
#define PAGE_POSITION_OFFSET_MASK   0x000000FF

#define FLASH_CMD_EWP              0x03 // Erase page and write page
#define FLASH_CMD_EWPL             0x04 // Erase page and write page then lock
#define FLASH_CMD_GLB              0x0A // Get Lock Bit
#define FLASH_CMD_CLB              0x09 // Clear Lock Bit
#define FLASH_CMD_SLB              0x08 // Set Lock Bit

#define FLASH_WRITE_ENABLE_MAGIC   (0x5A << 24)

#define FWS_FOR_WRITE      6 // see SAM3S series errata
#define MMIO(addr)         *((volatile uint32_t *)(addr))
#define IAP_ADDRESS        0x00800008
#define FIRST_PAGE             0
#define LAST_PAGE          17407

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
static uint32_t (*IAP_Function)(uint32_t, uint32_t);
static uint16_t currentPage = 0;
static uint8_t temporaryPageBuffer[FLASH_PAGE_SIZE];
uint32_t appEntryPoint;

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
  \brief Writes page to flash optionally locking Flash after write
  \param[in] page - page number to write to
  \param[in] lock - true if locking is required after write
******************************************************************************/
void flashWritePage(uint16_t page, bool lock)
{
  uint32_t cmd;

  if (lock)
    cmd = FLASH_WRITE_ENABLE_MAGIC | FLASH_CMD_EWPL | ((uint32_t)page << 8);
  else
    cmd = FLASH_WRITE_ENABLE_MAGIC | FLASH_CMD_EWP | ((uint32_t)page << 8);

  IAP_Function = (uint32_t (*)(uint32_t, uint32_t)) *((uint32_t *)IAP_ADDRESS);
  IAP_Function(0, cmd);
}

/**************************************************************************//**
  \brief Unlocks page in Flash
  \param[in] page - page number to unlock
******************************************************************************/
void flashUnlockPage(uint16_t page)
{
  uint32_t cmd;

  cmd = FLASH_WRITE_ENABLE_MAGIC | FLASH_CMD_CLB | ((uint32_t)page << 8);

  IAP_Function = (uint32_t (*)(uint32_t, uint32_t)) *((uint32_t *)IAP_ADDRESS);
  IAP_Function(0, cmd);
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
  uint16_t itr;
  uint32_t *ptr = (uint32_t *)temporaryPageBuffer;

  for (itr = 0; itr < FLASH_PAGE_SIZE; itr += sizeof(uint32_t), ptr++)
  {
    if ((FIRST_PAGE == pageAddress) && (sizeof(uint32_t) == (tableAddress + itr)))
    {
      appEntryPoint = *ptr;
      MMIO(tableAddress + itr) = MMIO(tableAddress + itr);
    }  
    else  
    {  
      MMIO(tableAddress + itr) = *ptr;
    }  
  }  
  // Perform page erase.
  flashWritePage(pageAddress, false);
}

/**************************************************************************//**
\brief Load received date to the flash.
******************************************************************************/
void flashPacketToPage(void)
{
  uint16_t packetPage;
  uint16_t tableAddress;
  uint8_t *dataPointer;
  uint32_t fmrBackup;

  dataPointer = dataBuffer.data;
  while (dataBuffer.dataSize)
  {
    packetPage = (dataBuffer.address & ~FLASH_START_ADDRESS) >> PAGE_NUMBER_SHIFT;

    if (currentPage != packetPage)
    {
      fmrBackup = EFC->EEFC_FMR;
      EFC->EEFC_FMR = (FWS_FOR_WRITE << 8);
      // Do a Unlock page
      flashUnlockPage(currentPage);
      // Do a Erase-Write of the page.
      flashEraseWritePage(currentPage);
      currentPage = packetPage;
      EFC->EEFC_FMR = fmrBackup;
    }

    tableAddress = dataBuffer.address & PAGE_POSITION_OFFSET_MASK;
    temporaryPageBuffer[tableAddress] = *dataPointer++;
    dataBuffer.address++;
    dataBuffer.dataSize--;
  }
}

/**************************************************************************//**
\brief Flush unloaded data before exit from bootloader.
******************************************************************************/
void flashFlushPage(void)
{
  uint32_t fmrBackup;

  fmrBackup = EFC->EEFC_FMR;
  EFC->EEFC_FMR = (FWS_FOR_WRITE << 8);
  // Do a Unlock page
  flashUnlockPage(currentPage);
  // Do a Erase-Write of the page.
  flashEraseWritePage(currentPage);
  EFC->EEFC_FMR = fmrBackup;
}

/**************************************************************************//**
\brief Performs exchanging of entry points between application and bootloader.
******************************************************************************/
void flashExchangeEntryPoints(void)
{
  uint32_t appLabel;
  uint32_t tempAddr = FLASH_START_ADDRESS;
  uint32_t itr;
  uint32_t fmrBackup;

  tempAddr = FLASH_END_ADDRESS + BOOT_AREA_SIZE - FLASH_PAGE_SIZE + 1;
  // change application entry point to boot entry point
  for (itr = 0; itr < FLASH_PAGE_SIZE; itr += sizeof(uint32_t))
  {
    if ((FLASH_END_ADDRESS + BOOT_AREA_SIZE - sizeof(uint32_t) + 1) == (tempAddr + itr))
    {  // last flash byte for storing application entry point
      MMIO(tempAddr + itr) = appEntryPoint;
      continue;
    }

    if ((FLASH_END_ADDRESS + BOOT_AREA_SIZE - 2 * sizeof(uint32_t) + 1) == (tempAddr + itr))
    {
      appLabel = MMIO(tempAddr + itr);
      appLabel |= 0xFF000000;
      MMIO(tempAddr + itr) = appLabel;
      continue;
    }

    MMIO(tempAddr + itr) = MMIO(tempAddr + itr); // copy flash page to shadow buffer
  }
  fmrBackup = EFC->EEFC_FMR;
  EFC->EEFC_FMR = (FWS_FOR_WRITE << 8);
  // Perform page erase and write.
  flashWritePage(LAST_PAGE, false);
  EFC->EEFC_FMR = fmrBackup;
}

// eof flashLoader.c
