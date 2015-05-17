/****************************************************************************//**
  \file uartSerializer.h

  \brief Declaration of uart serialize interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    30/03/09 A. Khromykh - Created
*******************************************************************************/
#ifndef _UARTSERIALIZER_H
#define _UARTSERIALIZER_H

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
\brief Startup initialization of the usart0
******************************************************************************/
void hwInitUsart0(void);

/**************************************************************************//**
\brief Startup initialization of the usart1
******************************************************************************/
void hwInitUsart1(void);

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitUsart0(void);

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitUsart1(void);

/**************************************************************************//**
\brief Receive byte on uart.

\param[out]
  p - pointer to byte memory;

\return
    true - there is received byte; \n
    false - there is not received byte;
******************************************************************************/
bool getByteUsart(uint8_t *p);

/**************************************************************************//**
\brief Transmit byte to uart.

\param[in]
  len - number of bytes to transmit;
\param[in]
  p - pointer to byte memory;
******************************************************************************/
void setByteUsart(uint16_t len, uint8_t *p);

#endif //_UARTSERIALIZER_H
