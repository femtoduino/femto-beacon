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
    28/07/10 A. Khromykh - Created
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
INLINE void  GPIO_##name##_set()           {PORT##port.OUTSET = (1 << bit);} \
INLINE void  GPIO_##name##_clr()           {PORT##port.OUTCLR = (1 << bit);} \
INLINE uint8_t  GPIO_##name##_read()       {return (PORT##port.IN & (1 << bit)) != 0;} \
INLINE uint8_t  GPIO_##name##_state()      {return (PORT##port.DIR & (1 << bit)) != 0;} \
INLINE void  GPIO_##name##_make_out()      {PORT##port.DIRSET = (1 << bit);} \
INLINE void  GPIO_##name##_make_in()       {PORT##port.DIRCLR = (1 << bit);} \
INLINE void  GPIO_##name##_make_pullup()   {PORT##port.PIN##bit##CTRL = PORT_OPC_PULLUP_gc;}\
INLINE void  GPIO_##name##_make_pulldown() {PORT##port.PIN##bit##CTRL = PORT_OPC_PULLDOWN_gc;}\
INLINE void  GPIO_##name##_toggle()        {PORT##port.OUTTGL = (1 << bit);}

//#define SPI_CHANNEL_C 0x08C0
#define SPI_CHANNEL_D 0x09C0
//#define SPI_CHANNEL_E 0x0AC0
#if defined(ATXMEGA128A1)
  //#define SPI_CHANNEL_F 0x0BC0
#endif

#define SPI_SS_pin_bm   (1 << 4)
#define SPI_MOSI_pin_bm (1 << 5)
#define SPI_MISO_pin_bm (1 << 6)
#define SPI_SCK_pin_bm  (1 << 7)

#define SS_PIN         SPI_SS_pin_bm
#define MOSI_PIN       SPI_MOSI_pin_bm
#define MISO_PIN       SPI_MISO_pin_bm
#define SCK_PIN        SPI_SCK_pin_bm

#if defined(SPI_CHANNEL_C)
  #define  SPI_CHANNEL    SPI_CHANNEL_C
  #define  SPI_PIN_PORT       PORTC
#elif defined(SPI_CHANNEL_D)
  #define  SPI_CHANNEL    SPI_CHANNEL_D
  #define  SPI_PIN_PORT       PORTD
#elif defined(SPI_CHANNEL_E)
  #define  SPI_CHANNEL    SPI_CHANNEL_E
  #define  SPI_PIN_PORT       PORTE
#elif defined(SPI_CHANNEL_F)
  #define  SPI_CHANNEL    SPI_CHANNEL_F
  #define  SPI_PIN_PORT       PORTF
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
\brief Initializes flash spi.
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

/**************************************************************************//**
\brief Unintializies spi.
******************************************************************************/
void spiMemUnInit(void);

/******************************************************************************
                     Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief Functions for xck pin manipulation.
******************************************************************************/
#if defined(SPI_CHANNEL_C)
  ASSIGN_PIN(EXT_MEM_CS, C, 4);
#elif defined(SPI_CHANNEL_D)
  ASSIGN_PIN(EXT_MEM_CS, D, 4);
#elif defined(SPI_CHANNEL_E)
  ASSIGN_PIN(EXT_MEM_CS, E, 4);
#elif defined(SPI_CHANNEL_F)
  ASSIGN_PIN(EXT_MEM_CS, F, 4);
#endif

#endif /* _SPIMEMINTERFACE_H */
