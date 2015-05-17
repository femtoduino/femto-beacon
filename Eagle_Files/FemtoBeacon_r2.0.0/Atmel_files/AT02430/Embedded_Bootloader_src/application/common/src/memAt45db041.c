/****************************************************************************//**
  \file memAt45db041.c

  \brief Some commands for at45db041 implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    26/07/10 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <extMemReader.h>
#include <spiMemInterface.h>

#if EXTERNAL_MEMORY == AT45DB041

/******************************************************************************
                   Constants section
******************************************************************************/
const uint32_t imageStartAddress[POSITION_MAX] = {IMAGE1_START_ADDRESS, IMAGE2_START_ADDRESS};

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Check availability of the external flash. Reads status register and
checks unchangable bits.

\return
  true - correct memory, \n
  false - other
******************************************************************************/
bool memCheckMem(void)
{
  uint16_t regStatus = RDSR;

  GPIO_EXT_MEM_CS_clr();
  spiMemTransac(SPI_TRANSACTION_TYPE_READ, (uint8_t *)&regStatus, sizeof(uint16_t));
  GPIO_EXT_MEM_CS_set();

  if (STATUS_UNCHANGED_BIT_MASK == ((uint8_t)(regStatus >> 8) & STATUS_UNCHANGED_BIT_MASK))
    return true;

  return false;
}

/**************************************************************************//**
\brief Reads data from memory.

\param[in]
  offset -  internal flash address;
\param[in]
  buffer -  pointer to the data buffer;
\param[in]
  length -  number bytes for reading;
******************************************************************************/
void memReadData(uint32_t offset, uint8_t *data, uint16_t size)
{
  uint32_t pageAddr;
  uint32_t byteAddr;

  pageAddr = offset / PAGE_SIZE;
  byteAddr = offset % PAGE_SIZE;
  offset = byteAddr | (pageAddr << 9) | ((uint32_t)READ << 24);
  offset = LITTLE_TO_BIG_ENDIAN(offset);
  GPIO_EXT_MEM_CS_clr();
  spiMemTransac(SPI_TRANSACTION_TYPE_WRITE , (uint8_t *)&offset, sizeof(uint32_t));
  // load 32 don't care bits
  spiMemTransac(SPI_TRANSACTION_TYPE_WRITE , (uint8_t *)&offset, sizeof(uint32_t));
  spiMemTransac(SPI_TRANSACTION_TYPE_READ, data, size);
  GPIO_EXT_MEM_CS_set();  // release spi cs
}

#endif  // EXTERNAL_MEMORY == AT45DB041

// eof memAt45db041.c
