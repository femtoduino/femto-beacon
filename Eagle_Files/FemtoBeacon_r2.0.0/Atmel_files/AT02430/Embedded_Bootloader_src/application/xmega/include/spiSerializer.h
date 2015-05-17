/****************************************************************************//**
  \file spiSerializer.h

  \brief Declaration of spi(slave) serialize interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    27/05/09 A. Khromykh - Created
*******************************************************************************/

#ifndef _SPISERIALIZER_H
#define _SPISERIALIZER_H

/******************************************************************************
                   Define(s) section
******************************************************************************/
#ifndef INFINITE
  #define INFINITE true
#endif

#define SPI_SS_pin_bm   (1 << 4)
#define SPI_MOSI_pin_bm (1 << 5)
#define SPI_MISO_pin_bm (1 << 6)
#define SPI_SCK_pin_bm  (1 << 7)
// pin to generate external interrupt
#define EXT_IRQ_pin_bm  (1 << 3)

#define SS_PIN         SPI_SS_pin_bm
#define MOSI_PIN       SPI_MOSI_pin_bm
#define MISO_PIN       SPI_MISO_pin_bm
#define SCK_PIN        SPI_SCK_pin_bm
#define IRQ_PIN        EXT_IRQ_pin_bm

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Startup initialization (frequency, io, spi)
******************************************************************************/
void hwInitSpiE(void);

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitSpi(void);

/**************************************************************************//**
\brief Receive byte on spi.

\param[in]
  p - pointer to byte memory;
\param[in]
  flag - type of byte waiting. INFINITE - infinite time;

\return
    true - there is received byte; \n
    false - there is not received byte;
******************************************************************************/
bool getByteSpi(uint8_t *p);

/**************************************************************************//**
\brief Generate interrupt and transmit byte to spi.

\param[in]
  len - number of bytes to transmit;
\param[in]
  p - pointer to byte memory;
******************************************************************************/
void setByteSpi(uint16_t len, uint8_t *p);

#endif /* _SPISERIALIZER_H */
