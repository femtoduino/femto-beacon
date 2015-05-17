/****************************************************************************//**
  \file hardwareInit.h

  \brief Hardware initialization routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15.10.09 A. Taradov - Created
*******************************************************************************/
#ifndef _HARDWAREINIT_H
#define _HARDWAREINIT_H

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Startup initialization (frequency, wdt)
******************************************************************************/
void lowLevelInit(void);

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void lowLevelUnInit(void);

#endif // _HARDWAREINIT_H

// eof hardwareInit.h
