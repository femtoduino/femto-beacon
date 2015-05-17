/****************************************************************************//**
  \file uartSerializer.c

  \brief Implementation of uart serialize interface.

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
#if (USE_USART0 == 1) || (USE_USART1 == 1) || (USE_USARTD0 == 1) || (USE_USARTF0 == 1)

#include <types.h>
#include <uartSerializer.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/**************************************************************************//**
 \brief Sets USART U2Xn bit value.
******************************************************************************/
#ifndef USART_DOUBLE_SPEED
  #define USART_DOUBLE_SPEED 1ul
#endif

/**************************************************************************//**
 \brief USART periferal modules base addresses (used as channel numbers).
******************************************************************************/
#define USART_CHANNEL_C0 0x08A0
#define USART_CHANNEL_C1 0x08B0
#define USART_CHANNEL_D0 0x09A0
#define USART_CHANNEL_D1 0x09B0
#define USART_CHANNEL_E0 0x0AA0
#define USART_CHANNEL_E1 0x0AB0
#define USART_CHANNEL_F0 0x0BA0

/**************************************************************************//**
 \brief I/O ports base addresses - used to initialize USART pins while
 USART initialization.
******************************************************************************/
#define PORTC_BASE_ADDR 0x0640
#define PORTD_BASE_ADDR 0x0660

/**************************************************************************//**
 \brief xmega USARTS channel '1' and channel '0' addresses displacement - used
 to initialize USART pins while USART initialization.
******************************************************************************/
#define XMEGA_USART_CHANNELS_ADDR_DISPLACEMENT (USART_CHANNEL_C1 - USART_CHANNEL_C0)

/**************************************************************************//**
 \brief xmega I/O ports addresses displacement - used to initialize USART pins
 while USART initialization.
******************************************************************************/
#define XMEGA_PORTS_ADDR_DISPLACEMENT (PORTD_BASE_ADDR - PORTC_BASE_ADDR)

#define USARTn0_RXD_pin_bm (1 << 2)
#define USARTn0_TXD_pin_bm (1 << 3)

#define USARTn1_RXD_pin_bm (1 << 6)
#define USARTn1_TXD_pin_bm (1 << 7)

#if (USE_USARTD0 == 1) || (USE_USARTF0 == 1)
/******************************************************************************
                   Global variables section
******************************************************************************/
static uint16_t usartTty;
static uint16_t portTty;

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void hwInitUsart(void);

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Startup initialization of the usart.
******************************************************************************/
static void hwInitUsart(void)
{
  uint8_t dif = (uint8_t)((usartTty >> 8) - (USART_CHANNEL_C0 >> 8));

  portTty = PORTC_BASE_ADDR + XMEGA_PORTS_ADDR_DISPLACEMENT * dif;
  if (usartTty & XMEGA_USART_CHANNELS_ADDR_DISPLACEMENT)
  { //USARTn channel 1
    //1. Set the TxD pin value high.
    ((PORT_t *)portTty)->OUTSET = USARTn1_TXD_pin_bm;
    //2. Set the TxD.
    ((PORT_t *)portTty)->DIRSET = USARTn1_TXD_pin_bm;
    ((PORT_t *)portTty)->DIRCLR = USARTn1_RXD_pin_bm;
  }
  else
  { //USARTn channel 0
    //1. Set the TxD pin value high.
    ((PORT_t *)portTty)->OUTSET = USARTn0_TXD_pin_bm;
    //2. Set the TxD.
    ((PORT_t *)portTty)->DIRSET = USARTn0_TXD_pin_bm;
    ((PORT_t *)portTty)->DIRCLR = USARTn0_RXD_pin_bm;
  }

  //3. Set the baud rate and frame format. 38400 baude.
  ((USART_t *)usartTty)->BAUDCTRLA = 0x0B;
  ((USART_t *)usartTty)->BAUDCTRLB = 0xF0;
  //4. Set mode of operation (enables the XCK pin output in synchronous mode).
  ((USART_t *)usartTty)->CTRLB = (uint8_t)USART_DOUBLE_SPEED << USART_CLK2X_bp; // Double the USART Transmition Speed
  ((USART_t *)usartTty)->CTRLC = USART_CHSIZE_8BIT_gc;
  //5. Enable the Transmitter and the Receiver.
  ((USART_t *)usartTty)->CTRLB |= USART_RXEN_bm | USART_TXEN_bm; // usart enable
}

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitUsart(void)
{ // mcu works from 2 MHz internal oscillator after reset
  // disable uart
  ((USART_t *)usartTty)->CTRLC = 0;
  ((USART_t *)usartTty)->CTRLB = 0;
  // Set all pins as input.
  ((PORT_t *)portTty)->DIR = 0;
}

/**************************************************************************//**
\brief Receive byte on uart.

\param[out]
  p - pointer to byte memory;

\return
    true - there is received byte; \n
    false - there is not received byte;
******************************************************************************/
bool getByteUsart(uint8_t *p)
{
  if (((USART_t *)usartTty)->STATUS & USART_RXCIF_bm)
  {
    *p = ((USART_t *)usartTty)->DATA;
    return true;
  }

  return false;
}

/**************************************************************************//**
\brief Transmit byte to uart.

\param[in]
  len - number of bytes to transmit;
\param[in]
  p - pointer to byte memory;
******************************************************************************/
void setByteUsart(uint16_t len, uint8_t *p)
{
  wdt_reset();
  while (len--)
  {
    ((USART_t *)usartTty)->DATA = *p++; // prepare transmission
    while (!(((USART_t *)usartTty)->STATUS & USART_TXCIF_bm));
    // wait for byte to send
    ((USART_t *)usartTty)->STATUS |= USART_TXCIF_bm; // delete TXCflag
  }
}

#if (USE_USARTD0 == 1)
/**************************************************************************//**
\brief Startup initialization of the usart D0.
******************************************************************************/
void hwInitUsartD0(void)
{
  usartTty = USART_CHANNEL_D0;
  hwInitUsart();
}
#endif // (USE_USARTD0 == 1)

#if (USE_USARTF0 == 1)
/**************************************************************************//**
\brief Startup initialization of the usart F0.
******************************************************************************/
void hwInitUsartF0(void)
{
  usartTty = USART_CHANNEL_F0;
  hwInitUsart();
}
#endif // (USE_USARTF0 == 1)

#endif // (USE_USARTD0 == 1) || (USE_USARTF0 == 1)

#endif // (USE_USART0 == 1) || (USE_USART1 == 1) || (USE_USARTD0 == 1) || (USE_USARTF0 == 1)

// eof uartSerializer.c
