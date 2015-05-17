/****************************************************************************//**
  \file srecParser.h

  \brief Declaration of parser of the srec format interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    30/03/09 A. Khromykh - Created
*******************************************************************************/
#ifndef _SRECPARSER_H
#define _SRECPARSER_H

/******************************************************************************
                   Define(s) section
******************************************************************************/
/******************************************************************************
                   Define(s) section
******************************************************************************/
#define S_SYMBOL                  0x53
#define HANDSHAKE_REQ             0x4b65a5b2
#define HANDSHAKE_CONF            0x26d2d369
#define ACK                       0xb49a5a4d
#define NACK                      0xb25a592d
#define CHECKSUM_AREA_SIZE        1
#define SRECORD_SIZE              128                        // The conventional data field length in S-Record

#if defined(ATXMEGA256A3) || defined(ATXMEGA256D3) || defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA128RFA1) || defined (ATXMEGA128A1)
  #define EEPROM_START_ADDRESS      0x810000                   // Start address of EEPROM area
  #define FLASH_START_ADDRESS       0x000000                   // Start address of FLASH area
#elif defined(AT91SAM3S4C)
  #define EEPROM_START_ADDRESS      0x810000                   // Start address of EEPROM area
  #define FLASH_START_ADDRESS       0x400000                   // Start address of FLASH area
#endif
#if defined(ATXMEGA256A3) || defined(ATXMEGA256D3)
  #define EEPROM_END_ADDRESS      0x810FFF                   // End address of EEPROM area
  #define FLASH_END_ADDRESS       0x03FFFF                   // End address of FLASH area
#elif defined(ATXMEGA128A1)
  #define EEPROM_END_ADDRESS      0x8107FF                   // End address of EEPROM area
  #define FLASH_END_ADDRESS       0x01FFFF                   // End address of FLASH area
#elif defined(ATMEGA1281)
  #if defined(EXT_MEMORY) || defined(INCLUDE_RF4CE_FUNCTIONS)
    #define BOOT_AREA_SIZE          0x1000                   // 4kbytes boot area size
  #else
    #define BOOT_AREA_SIZE          0x800                    // 2kbytes boot area size
  #endif
  #define EEPROM_END_ADDRESS      0x810FFF                   // End address of EEPROM area
  #define FLASH_END_ADDRESS      (0x01FFFF - BOOT_AREA_SIZE) // End address of FLASH area
#elif defined(ATMEGA2561)
  #if defined(EXT_MEMORY)
    #define BOOT_AREA_SIZE          0x1000                   // 4kbytes boot area size
  #else
    #define BOOT_AREA_SIZE          0x800                    // 2kbytes boot area size
  #endif
  #define EEPROM_END_ADDRESS      0x810FFF                   // End address of EEPROM area
  #define FLASH_END_ADDRESS      (0x03FFFF - BOOT_AREA_SIZE) // End address of FLASH area
#elif defined(ATMEGA128RFA1)
  #if defined(EXT_MEMORY) || defined(INCLUDE_RF4CE_FUNCTIONS)
    #define BOOT_AREA_SIZE          0x1000                   // 4kbytes boot area size
  #else
    #define BOOT_AREA_SIZE          0x800                    // 2kbytes boot area size
  #endif
  #define EEPROM_END_ADDRESS      0x810FFF                   // End address of EEPROM area
  #define FLASH_END_ADDRESS      (0x01FFFF - BOOT_AREA_SIZE) // End address of FLASH area
#elif defined(AT91SAM3S4C)
  #define BOOT_AREA_SIZE          0x1000                     // 4kbytes boot area size
  #define EEPROM_END_ADDRESS      0x810000                   // End address of EEPROM area
  #define FLASH_END_ADDRESS      (0x43FFFF - BOOT_AREA_SIZE) // End address of FLASH area
#else
  #error "Unsupported microcontroller."
#endif

/******************************************************************************
                   Types section
******************************************************************************/
// Type of record (S0, S1, S2, S3, S5, S7, S8, or S9).
typedef enum
{
  S0 = 0x5330,
  S1 = 0x5331,
  S2 = 0x5332,
  S3 = 0x5333,
  S5 = 0x5335,
  S7 = 0x5337,
  S8 = 0x5338,
  S9 = 0x5339
} SrecordType_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Handshake procedure.
******************************************************************************/
void srecHandshake(void);

/**************************************************************************//**
\brief Communication support between host and mcu
******************************************************************************/
void srecProtocol(void);

#endif // _SRECPARSER_H
