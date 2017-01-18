/****************************************************************************//**
  \file AVR1624twiParser.h

  \brief Declaration of parser of the TWI communication protocol as per AVR1624.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    25/02/13 S. Kanchana - Created
*******************************************************************************/
#ifndef _AVR1624TWIPARSER_H
#define _AVR1624TWIPARSER_H

/******************************************************************************
                   Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Handshake procedure.
******************************************************************************/
void twiBootHandshake(void);

/**************************************************************************//**
 \brief TWI communication Check
******************************************************************************/
void twiBootCheck(void);

#endif //_AVR1624TWIPARSER_H