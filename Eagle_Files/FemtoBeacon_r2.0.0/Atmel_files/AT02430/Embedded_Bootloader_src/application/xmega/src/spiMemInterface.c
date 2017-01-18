/****************************************************************************//**
  \file spiMemInterface.c

  \brief Implementation of external spi memory interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    28/07/10 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/\
#ifdef EXT_MEMORY
#include <spiMemInterface.h>

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Initializes flash spi.
******************************************************************************/
void spiMemInit(void)
{
  //1. Set MISO as input, SS, SCK, MOSI as output.
  ((PORT_t *)&SPI_PIN_PORT)->OUTSET = SS_PIN;
  ((PORT_t *)&SPI_PIN_PORT)->DIRSET = SS_PIN;
  ((PORT_t *)&SPI_PIN_PORT)->DIRSET = MOSI_PIN;
  ((PORT_t *)&SPI_PIN_PORT)->DIRSET = SCK_PIN;
  ((PORT_t *)&SPI_PIN_PORT)->DIRCLR = MISO_PIN;

  //2. Set the clock rate 1 MHz, data order (MSB first) and mode 0. Master mode.
  ((SPI_t *)SPI_CHANNEL)->CTRL = SPI_ENABLE_bm | SPI_CLK2X_bm | SPI_MASTER_bm;
}

/**************************************************************************//**
\brief Unintializies spi.
******************************************************************************/
void spiMemUnInit(void)
{
  // uninit spi pins
  ((PORT_t *)&SPI_PIN_PORT)->DIRCLR = SS_PIN;
  ((PORT_t *)&SPI_PIN_PORT)->DIRCLR = MOSI_PIN;
  ((PORT_t *)&SPI_PIN_PORT)->DIRCLR = SCK_PIN;

  // disable spi
  ((SPI_t *)SPI_CHANNEL)->CTRL = 0;
}

/**************************************************************************//**
\brief Writes or reads a length bytes from the external spi memory.

\param[in]
  type   -  transaction type;
\param[in]
  buffer -  pointer to the data buffer;
\param[in]
  length -  number bytes for transfer;
******************************************************************************/
void spiMemTransac(TransactionType_t type, uint8_t *buffer, uint16_t length)
{
  uint16_t i;

  wdt_reset();
  for (i = 0; i < length; i++)
  {
    ((SPI_t *)SPI_CHANNEL)->DATA = *(buffer + i); // Write data.
    while (!(((SPI_t *)SPI_CHANNEL)->STATUS & SPI_IF_bm));
    if (SPI_TRANSACTION_TYPE_WRITE  == type)
      ((SPI_t *)SPI_CHANNEL)->DATA;
    else
      *(buffer + i) = ((SPI_t *)SPI_CHANNEL)->DATA;
  }
}

#endif //EXT_MEMORY

// eof spiMemInterface.c

