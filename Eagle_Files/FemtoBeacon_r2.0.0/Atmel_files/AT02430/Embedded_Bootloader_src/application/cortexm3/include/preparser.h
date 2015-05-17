/****************************************************************************//**
  \file preparser.h

  \brief Declaration of the preparser interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    06.09.11 A. Khromykh - Created
*******************************************************************************/

#ifndef _PREPARSER_H
#define _PREPARSER_H

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Waiting for a field of the type from the internal address

\param[in]
  stream - byte stream from usb;
\param[in]
  size - size of the converted stream;
******************************************************************************/
void convertStreamToNumbers(uint8_t *stream, uint8_t size);

/**************************************************************************//**
\brief Read data byte from preparser buffer

\param[in]
  data - pointer to memory for data byte;

\return
  true - there is data byte;
  false - preparser buffer is empty
******************************************************************************/
bool readFromPreBuffer(uint8_t *data);

#endif /* _PREPARSER_H */
