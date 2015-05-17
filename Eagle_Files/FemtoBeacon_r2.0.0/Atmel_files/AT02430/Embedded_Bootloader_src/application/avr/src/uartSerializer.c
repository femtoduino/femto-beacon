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
#include <types.h>
#include <uartSerializer.h>

#if (USE_USART0 == 1) || (USE_USART1 == 1) || (USE_USARTD0 == 1) || (USE_USARTF0 == 1)

/******************************************************************************
                   Define(s) section
******************************************************************************/
/**************************************************************************//**
 \brief Sets USART U2Xn bit value.
******************************************************************************/
#ifndef USART_DOUBLE_SPEED
  #define USART_DOUBLE_SPEED 1ul
#endif

#define MMIO_BYTE(mem_addr) (*(volatile uint8_t *)(mem_addr))
#define MMIO_WORD(mem_addr) (*(volatile uint16_t *)(mem_addr))

#define USART_CHANNEL_0  0xC0 // USART0 start address
#define USART_CHANNEL_1  0xC8 // USART1 start address

#define UCSRnA(tty) MMIO_BYTE(tty + 0)
#define UCSRnB(tty) MMIO_BYTE(tty + 1)
#define UCSRnC(tty) MMIO_BYTE(tty + 2)
#define UBRRnL(tty) MMIO_BYTE(tty + 4)
#define UBRRnH(tty) MMIO_BYTE(tty + 5)
#define UBRRn(tty)  MMIO_WORD(tty + 4)
#define UDRn(tty)   MMIO_BYTE(tty + 6)

#if (USE_USART0 == 1) || (USE_USART1 == 1)

/******************************************************************************
                   Global variables section
******************************************************************************/
static uint8_t usartTty;

/******************************************************************************
                   Prototypes section
******************************************************************************/
static inline void hwInit(void);
static inline void hwUnInit(void);

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Startup initialization of the usart.
******************************************************************************/
static inline void hwInit(void)
{
  // Set the baud rate. 38400 bauds.
  UBRRnL(usartTty) = 0x02; // 38400 bauds
  // Double the USART Transmition Speed
  UCSRnA(usartTty) = 1 << U2X0;
  // Set frame format. 8 bit.
  UCSRnC(usartTty) = (1 << UCSZ10) | (1 << UCSZ11);
  //Enable the Transmitter and the Receiver.
  UCSRnB(usartTty) = (1 << RXEN0) | (1 << TXEN0);
}

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
static inline void hwUnInit(void)
{ // mcu works from 921600 Hz internal oscillator after reset
  // disable uart
  UBRRnL(usartTty) = 0;
  UCSRnA(usartTty) = 0;
  UCSRnC(usartTty) = 0;
  UCSRnB(usartTty) = 0;
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
  if (UCSRnA(usartTty) & (1 << RXC1))
  {
    *p = UDRn(usartTty);
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
    UDRn(usartTty) = *p++; // prepare transmission
    while (!(UCSRnA(usartTty) & (1 << TXC1)));
    // wait for byte to send
    UCSRnA(usartTty) |= (1 << TXC1); // delete TXCflag
  }
}

#if (USE_USART0 == 1)
/**************************************************************************//**
\brief Startup initialization of the usart0.
******************************************************************************/
void hwInitUsart0(void)
{
  usartTty = USART_CHANNEL_0;

#if (KEY_REMOTE_RS232_CONTROLLER == YES)
  #if defined(ATMEGA1281)
    DDRG = (1 << 1);  // LEDP_SEL
    PORTG = 0;
    DDRA = (1 << 6);  // EN_232
    PORTA = 0;
    PORTG = (1 << 1); // latch low level on EN_232
    PORTG = 0;        // latch low level on EN_232
  #elif defined(ATMEGA128RFA1)
    DDRE = (1 << 5);  // LEDP_SEL
    PORTE = 0;
    DDRB = (1 << 6);  // EN_232
    PORTB = 0;
    PORTE = (1 << 5); // latch low level on EN_232
    PORTE = 0;        // latch low level on EN_232
  #endif
#endif // _RCB_BB_RS232_CONTROLLER_

  hwInit();
}

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitUsart0(void)
{
#if (KEY_REMOTE_RS232_CONTROLLER == YES)
  #if defined(ATMEGA1281)
    DDRG = 0;
    DDRA = 0;
  #elif defined(ATMEGA128RFA1)
    DDRE = 0;
    DDRB = 0;
  #endif
#endif // _RCB_BB_RS232_CONTROLLER_

  hwUnInit();
}
#endif // (USE_USART0 == 1)

#if (USE_USART1 == 1)
/**************************************************************************//**
\brief Startup initialization of the usart1.
******************************************************************************/
void hwInitUsart1(void)
{
  usartTty = USART_CHANNEL_1;

  // Enable RS232 controller on RCB_BB board.
#if (RCB_BB_RS232_CONTROLLER == YES)
  #if defined(ATMEGA1281)
    DDRC = (1 << 4) | (1 << 6) | (1 << 7);
    PORTC = (1 << 6) | (1 << 7);
  #elif defined(ATMEGA128RFA1)
    DDRD = (1 << 4) | (1 << 6) | (1 << 7);
    PORTD = (1 << 6) | (1 << 7);
  #endif
#endif // RCB_BB_RS232_CONTROLLER

  hwInit();
}

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitUsart1(void)
{
#if (RCB_BB_RS232_CONTROLLER == YES)
  #if defined(ATMEGA1281)
    DDRC = 0;
  #elif defined(ATMEGA128RFA1)
    DDRD = 0;
  #endif
#endif // RCB_BB_RS232_CONTROLLER

  hwUnInit();
}
#endif // (USE_USART1 == 1)

#endif // (USE_USART0 == 1) || (USE_USART1 == 1)

#endif // (USE_USART0 == 1) || (USE_USART1 == 1) || (USE_USARTD0 == 1) || (USE_USARTF0 == 1)

// eof uartSerializer.c
