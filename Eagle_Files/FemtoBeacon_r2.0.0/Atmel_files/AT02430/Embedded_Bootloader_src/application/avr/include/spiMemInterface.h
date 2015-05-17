/****************************************************************************//**
  \file spiMemInterface.h

  \brief Declaration of external spi memory interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    14/08/09 A. Khromykh - Created
*******************************************************************************/
#ifndef _SPIMEMINTERFACE_H
#define _SPIMEMINTERFACE_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/******************************************************************************
* void gpioX_set() sets GPIOX pin to logical 1 level.
* void gpioX_clr() clears GPIOX pin to logical 0 level.
* void gpioX_make_in makes GPIOX pin as input.
* void gpioX_make_in makes GPIOX pin as output.
* uint8_t gpioX_read() returns logical level GPIOX pin.
* uint8_t gpioX_state() returns configuration of GPIOX port.
*******************************************************************************/
#define ASSIGN_PIN(name, port, bit) \
static inline void  GPIO_##name##_set()         {PORT##port |= (1 << bit);} \
static inline void  GPIO_##name##_clr()         {PORT##port &= ~(1 << bit);} \
static inline uint8_t  GPIO_##name##_read()     {return (PIN##port & (1 << bit)) != 0;} \
static inline uint8_t  GPIO_##name##_state()    {return (DDR##port & (1 << bit)) != 0;} \
static inline void  GPIO_##name##_make_out()    {DDR##port |= (1 << bit);} \
static inline void  GPIO_##name##_make_in()     {DDR##port &= ~(1 << bit); PORT##port &= ~(1 << bit);} \
static inline void  GPIO_##name##_make_pullup() {PORT##port |= (1 << bit);}\
static inline void  GPIO_##name##_toggle()      {PORT##port ^= (1 << bit);}

#define USART_CHANNEL0
//#define USART_CHANNEL1

#if defined(USART_CHANNEL0)
  #define UCSRASPI UCSR0A
  #define UCSRBSPI UCSR0B
  #define UCSRCSPI UCSR0C
  #define UBRRSPI  UBRR0
  #define UDRSPI   UDR0
  #define UBRRLSPI UBRR0L
  #define UBRRSPI  UBRR0
#elif defined(USART_CHANNEL1)
  #define UCSRASPI UCSR1A
  #define UCSRBSPI UCSR1B
  #define UCSRCSPI UCSR1C
  #define UBRRSPI  UBRR1
  #define UDRSPI   UDR1
  #define UBRRLSPI UBRR1L
  #define UBRRSPI  UBRR1
#endif

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  SPI_TRANSACTION_TYPE_READ,
  SPI_TRANSACTION_TYPE_WRITE
} TransactionType_t;

/******************************************************************************
                     Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes usart as spi.
******************************************************************************/
void spiMemInit(void);

/**************************************************************************//**
\brief Writes or reads a length bytes from the external spi memory.

\param[in]
  type   -  transaction type;
\param[in]
  buffer -  pointer to the data buffer;
\param[in]
  length -  number bytes for transfer;
******************************************************************************/
void spiMemTransac(TransactionType_t type, uint8_t *buffer, uint16_t length);

/******************************************************************************
                     Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief Functions for xck pin manipulation.
******************************************************************************/
#if defined(USART_CHANNEL0)
  ASSIGN_PIN(XCK, E, 2);
#elif defined(USART_CHANNEL1)
  ASSIGN_PIN(XCK, D, 5);
#endif

/**************************************************************************//**
\brief Functions for cs pin manipulation.
******************************************************************************/
#if defined(ATMEGA1281) || defined(ATMEGA2561)
ASSIGN_PIN(EXT_MEM_CS, F, 3);
#elif defined(ATMEGA128RFA1)
ASSIGN_PIN(EXT_MEM_CS, G, 5);
#endif

/**************************************************************************//**
\brief Unintializies usart.
******************************************************************************/
static inline void spiMemUnInit(void)
{
  UCSRASPI = 0;
  UCSRBSPI = 0;
  UCSRCSPI = 0;
}

#endif /* _SPIMEMINTERFACE_H */
