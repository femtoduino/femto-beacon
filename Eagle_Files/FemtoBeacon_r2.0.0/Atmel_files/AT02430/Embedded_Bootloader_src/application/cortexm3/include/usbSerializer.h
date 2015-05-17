/****************************************************************************//**
  \file usbSerializer.h

  \brief Declaration of usb serializer interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    01.09.11 A. Khromykh - Created
*******************************************************************************/
#ifndef _USBSERIALIZER_H
#define _USBSERIALIZER_H

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
\brief Startup initialization (frequency, io, usb, dfu)
******************************************************************************/
void hwInitUsbDfu(void);

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitUsbDfu(void);

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
bool getByteUsbDfu(uint8_t *p);

/**************************************************************************//**
\brief Generate interrupt and transmit byte to spi.

\param[in]
  len - number of bytes to transmit;
\param[in]
  p - pointer to byte memory;
******************************************************************************/
void setByteUsbDfu(uint16_t len, uint8_t *p);

#endif /* _USBSERIALIZER_H */
