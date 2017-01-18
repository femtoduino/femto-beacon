/****************************************************************************//**
  \file spiSerializer.c

  \brief Implementation of spi(slave) serialize interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    27/05/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#if (USE_SPIE == 1)
#include <types.h>
#include <spiSerializer.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/**************************************************************************//**
 \brief I/O ports base addresses - used to initialize USART pins while
 USART initialization.
******************************************************************************/
#define PORTC_BASE_ADDR 0x0640
#define PORTD_BASE_ADDR 0x0660

#define SPI_CHANNEL_C 0x08C0
#define SPI_CHANNEL_D 0x09C0
#define SPI_CHANNEL_E 0x0AC0
#define SPI_CHANNEL_F 0x0BC0

/**************************************************************************//**
 \brief xmega I/O ports addresses displacement - used to initialize USART pins
 while USART initialization.
******************************************************************************/
#define XMEGA_PORTS_ADDR_DISPLACEMENT (PORTD_BASE_ADDR - PORTC_BASE_ADDR)

/******************************************************************************
                   Global variables section
******************************************************************************/
static uint16_t spiTty;
static uint16_t portTty;

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Startup initialization of the spi.
******************************************************************************/
static void hwInitSpi(void)
{ // mcu works from 2 MHz internal oscillator after reset
  uint8_t dif = (uint8_t)((spiTty >> 8) - (SPI_CHANNEL_C >> 8));

  portTty = PORTC_BASE_ADDR + XMEGA_PORTS_ADDR_DISPLACEMENT * dif;
  //1. Set SS, SCK, MOSI as input, MISO as output.
  ((PORT_t *)portTty)->DIRCLR = SS_PIN;
  ((PORT_t *)portTty)->DIRCLR = MOSI_PIN;
  ((PORT_t *)portTty)->DIRCLR = SCK_PIN;
  ((PORT_t *)portTty)->DIRSET = MISO_PIN;
  //2. Set the clock rate, data order (MSB first) and mode 0. Slave mode.
  ((SPI_t *)spiTty)->CTRL = SPI_ENABLE_bm;

  // configure port to generate external interrupt
  ((PORT_t *)portTty)->OUTSET = IRQ_PIN;
  ((PORT_t *)portTty)->DIRSET = IRQ_PIN;
}

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitSpi(void)
{ // mcu works from 2 MHz internal oscillator after reset
  // disable spi
  ((SPI_t *)spiTty)->CTRL = 0;
  // Set all pins as input.
  ((PORT_t *)portTty)->DIR = 0;
}

/**************************************************************************//**
\brief Receive byte on spi.

\param[out]
  p - pointer to byte memory;

\return
    true - there is received byte; \n
    false - there is not received byte;
******************************************************************************/
bool getByteSpi(uint8_t *p)
{
  if (((SPI_t *)spiTty)->STATUS & SPI_IF_bm)
  {
    *p = ((SPI_t *)spiTty)->DATA;
    return true;
  }

  return false;
}

/**************************************************************************//**
\brief Generate interrupt and transmit byte to spi.

\param[in]
  len - number of bytes to transmit;
\param[in]
  p - pointer to byte memory;
******************************************************************************/
void setByteSpi(uint16_t len, uint8_t *p)
{
  wdt_reset();
  ((SPI_t *)spiTty)->DATA = *p++; // prepare transmission
  ((PORT_t *)portTty)->OUTCLR = IRQ_PIN;
  while (!(((SPI_t *)spiTty)->STATUS & SPI_IF_bm));
  // set pin to high level to clear interrupt line
  ((PORT_t *)portTty)->OUTSET = IRQ_PIN;

  while (--len)
  {
    ((SPI_t *)spiTty)->DATA = *p++; // prepare transmission
    while (!(((SPI_t *)spiTty)->STATUS & SPI_IF_bm));
  }
}

/**************************************************************************//**
\brief Startup initialization of the spi E.
******************************************************************************/
void hwInitSpiE(void)
{
  spiTty = SPI_CHANNEL_E;
  hwInitSpi();
}

#endif // (USE_SPIE == 1)

// eof spiSerializer.c
