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
    14/08/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#ifdef EXT_MEMORY
#include <spiMemInterface.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define UCPHA0   1
#define UCPOL0   0

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Initializes usart as spi.
******************************************************************************/
void spiMemInit(void)
{
  // Set MSPI mode
  UCSRCSPI = (1 << UMSEL01) | (1 << UMSEL00);
  // Set spi mode 3 and MSB order
  UCSRCSPI |= (1 << UCPOL0) | (1 << UCPHA0);
  // Set sck pin as output
  GPIO_XCK_make_out();
  // Enable receiver and transmitter
  UCSRBSPI = (1 << RXEN0) | (1 << TXEN0);
  // Set maximum baud rate
  UBRRSPI = 0;
  // Set cs pin as output
  GPIO_EXT_MEM_CS_make_out();
  GPIO_EXT_MEM_CS_set();
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
  uint8_t temp;

  wdt_reset();
  for (i = 0; i < length; i++)
  {
    // Wait for empty transmit buffer
    while (!(UCSRASPI & (1 << UDRE0)));
    // Send data
    UDRSPI = *(buffer + i);
    // Wait for data to be received
    while (!(UCSRASPI & (1 << RXC0)));
    // receives data to clear received usart buffer
    if (SPI_TRANSACTION_TYPE_WRITE  == type)
      temp = UDRSPI;
    else
      *(buffer + i) = UDRSPI;
  }
  (void)temp;
}

#endif //EXT_MEMORY

// eof spiMemInterface.c

