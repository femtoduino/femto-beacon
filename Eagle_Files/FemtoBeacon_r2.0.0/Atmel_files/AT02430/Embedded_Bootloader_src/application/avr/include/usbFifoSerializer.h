/****************************************************************************//**
  \file usbFifoSerializer.h

  \brief Declaration of usb FIFO interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    5.02.11 A. Khromykh - Created
*******************************************************************************/

#ifndef _USBFIFOSERIALIZER_H
#define _USBFIFOSERIALIZER_H

/******************************************************************************
                   Define(s) section
******************************************************************************/
#ifndef INFINITE
  #define INFINITE true
#endif

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Startup initialization
******************************************************************************/
void hwInitUsbFifo(void);

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitUsbFifo(void);

/**************************************************************************//**
\brief Receive byte from usb FIFO.

\param[out]
  p - pointer to byte memory;

\return
    true - there is received byte; \n
    false - there is not received byte;
******************************************************************************/
bool getByteUsbFifo(uint8_t *p);

/**************************************************************************//**
\brief Transmit byte to usb FIFO.

\param[in]
  len - number of bytes to transmit;
\param[in]
  p - pointer to byte memory;
******************************************************************************/
void setByteUsbFifo(uint16_t len, uint8_t *p);

#endif /* _USBFIFOSERIALIZER_H */
