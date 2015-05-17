/****************************************************************************//**
  \file abstractSerializer.h

  \brief Declaration of abstract serialize interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    5.02.11 A. Khromykh - Created
*******************************************************************************/
#ifndef _ABSTRACTSERIALIZER_H
#define _ABSTRACTSERIALIZER_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <hardwareInit.h>
#if (USE_USART0 == 1) || (USE_USART1 == 1) || (USE_USARTD0 == 1) || (USE_USARTF0 == 1)
  #include <uartSerializer.h>
#endif
#if (USE_SPIE == 1)
  #include <spiSerializer.h>
#endif
#if (USE_USB_FIFO == 1)
  #include <usbFifoSerializer.h>
#endif
#if (USE_USB_DFU == 1)
  #include <usbSerializer.h>
#endif
#if (USE_TWI == 1) || (USE_TWIC == 1) || (USE_TWID == 1) || (USE_TWIE == 1) || (USE_TWIF ==1)
  #include <twiSerializer.h>
#endif

/******************************************************************************
                   Types section
******************************************************************************/
/* Some serial interface descriptor. */
typedef struct
{
  void (* hwInit)(void);
  void (* hwUnInit)(void);
  bool (* getByte)(uint8_t *p);
  void (* setByte)(uint16_t len, uint8_t *p);
} SerialInterface_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Clear interface setting.
******************************************************************************/
void bootUnInitSerializer(void);

/**************************************************************************//**
\brief Read byte from interface.

\param[out]
  p - pointer to data buffer;
\param[in]
  flag - flag about unlimited or limited reading time.

\return
  true - byte was read, false - byte was not read.
******************************************************************************/
bool bootGetChar(uint8_t *p, bool flag);

/**************************************************************************//**
\brief Write byte to interface.

\param[in]
  len - data length;
\param[in]
  p - pointer to data buffer.
******************************************************************************/
void bootSetChar(uint16_t len, uint8_t *p);

/**************************************************************************//**
\brief Perform search interface activity and handshake message.

\return
  true - handshake was read, false - there is no activity.
******************************************************************************/
bool bootScanInterfaces(void);

#endif //_ABSTRACTSERIALIZER_H
