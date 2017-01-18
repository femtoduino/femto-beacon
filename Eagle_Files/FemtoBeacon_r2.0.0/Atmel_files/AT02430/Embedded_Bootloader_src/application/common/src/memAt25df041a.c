/****************************************************************************//**
  \file memAt25dd041a.c

  \brief Some commands for at25df041a implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    05/10/11 N. Fomin - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <extMemReader.h>
#include <spiMemInterface.h>

#if EXTERNAL_MEMORY == AT25DF041A

/******************************************************************************
                   Constants section
******************************************************************************/
const uint32_t imageStartAddress[POSITION_MAX] = {IMAGE1_START_ADDRESS, IMAGE2_START_ADDRESS};

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Check availability of the external flash. Reads vendor and chip ID
from the external flash.

\return
  true - correct memory, \n
  false - other
******************************************************************************/
bool memCheckMem(void)
{
  uint64_t manufacId = RDID;

  GPIO_EXT_MEM_CS_clr();
  spiMemTransac(SPI_TRANSACTION_TYPE_READ, (uint8_t *)&manufacId, sizeof(uint64_t)-3);
  GPIO_EXT_MEM_CS_set();

  if (MANUFACTURER_ID == (uint8_t)(manufacId >> 8))
    if ((DEVICE_ID_1 == (uint8_t)(manufacId >> 16)) &&
        (DEVICE_ID_2 == (uint8_t)(manufacId >> 24)) &&
        (EXT_STRING_LENGTH == (uint8_t)(manufacId >> 32)))
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
  uint8_t instruction = READ;

  offset = LITTLE_TO_BIG_ENDIAN(offset<<8);
  GPIO_EXT_MEM_CS_clr();
  spiMemTransac(SPI_TRANSACTION_TYPE_WRITE , &instruction, sizeof(uint8_t));
  spiMemTransac(SPI_TRANSACTION_TYPE_WRITE , (uint8_t *)&offset, sizeof(uint32_t)-1);
  spiMemTransac(SPI_TRANSACTION_TYPE_READ, data, size);
  GPIO_EXT_MEM_CS_set();  // release spi cs
}

#endif // EXTERNAL_MEMORY == AT25DF041A

// eof memAt25df041a.c
