/****************************************************************************//**
  \file bootStructure.h

  \brief Declaration of common types for bootloader.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2/04/09 A. Khromykh - Created
*******************************************************************************/
#ifndef _BOOTSTRUCTURE_H
#define _BOOTSTRUCTURE_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <srecParser.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define SREC_DATA_LENGTH   SRECORD_SIZE

/******************************************************************************
                   Types section
******************************************************************************/
// Buffer between parser and memory loader
typedef struct
{
  SrecordType_t         recordType;
  uint32_t              address;
  uint8_t               data[SREC_DATA_LENGTH];
  uint8_t               dataSize;
} BootBuffer_t;

#endif // _BOOTSTRUCTURE_H
