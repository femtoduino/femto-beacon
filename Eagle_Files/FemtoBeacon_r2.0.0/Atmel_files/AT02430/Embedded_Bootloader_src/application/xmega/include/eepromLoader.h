/****************************************************************************//**
  \file eepromLoader.h

  \brief Declaration api of eeprom loader.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    3/04/09 A. Khromykh - Created
*******************************************************************************/
#ifndef _EEPROMLOADER_H
#define _EEPROMLOADER_H

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Load received date to the flash.
******************************************************************************/
void eepromPacketToPage(void);

/**************************************************************************//**
\brief Flush unloaded data before exit from bootloader.
******************************************************************************/
void eepromFlushPage(void);

/**************************************************************************//**
\brief Reads data from the EEPROM.

\param[in]
  address - address of data byte.
\return
  read byte
******************************************************************************/
uint8_t eepromReadData(uint16_t address);

#endif // _EEPROMLOADER_H
