/****************************************************************************//**
  \file extMemReader.h

  \brief Declaration of external memory loader interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    17/08/09 A. Khromykh - Created
*******************************************************************************/
#ifndef _EXTMEMREADER_H
#define _EXTMEMREADER_H

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define CRC_INITIALIZATION_VALUE     0xFF
#define EMPTY_EEPROM_VALUE           0xFF
#define ACTION_POSITION              0

#if EXTERNAL_MEMORY == AT25F2048

  #if !defined(ATMEGA1281) && !defined(ATMEGA128RFA1)
    #error 'at25f2048 is supported only for atmega1281/128rfa1.'
  #endif

  // flash instruction format (see at25f2048 manual)
  #define READ                                          0x03
  #define RDID                                          0x15

  // Atmel ID
  #define MANUFACTURER_ID                               0x1F
  // at25f2048
  #define DEVICE_ID                                     0x63

  #define PAGE_SIZE                                      256
  // 2 Mbits
  #define FLASH_SIZE                                  262144
  // image consists mcu flash - bootloader size + eeprom size. 128k - 4k + 4k
  #define IMAGE_SIZE                                  131072
  #define EXT_FLASH_START_ADDRESS                          0
  #define IMAGE1_START_ADDRESS                     EXT_FLASH_START_ADDRESS
  #define IMAGE2_START_ADDRESS                    (EXT_FLASH_START_ADDRESS + IMAGE_SIZE)

  #define MCU_EEPROM_SIZE                                4096
  #define MCU_FLASH_SIZE                          (IMAGE_SIZE - MCU_EEPROM_SIZE)

#elif EXTERNAL_MEMORY == AT45DB041

  #if !defined(ATMEGA1281) && !defined(ATMEGA128RFA1) && !defined(ATXMEGA256A3) && !defined(ATXMEGA256D3)
    #error 'at45db041 is supported only for atmega1281/128rfa1, atxmega256a/d3.'
  #endif

  // flash instruction format (see at45db041 manual)
  #define RDSR                                            0xD7
  #define READ                                            0xE8

  // status register bits
  #define RDY                                             0x80
  // unchanged bit mask within status register xx0111xx
  #define STATUS_UNCHANGED_BIT_MASK                       0x1C

  #define PAGE_SIZE                                        264
  // block addresses
  #define FIRST_HALF_BLOCK_NUMBER                            0
  #define SECOND_HALF_BLOCK_NUMBER                         128

  // 4 Mbits
  #define FLASH_SIZE                                    540672
  #define EXT_FLASH_START_ADDRESS                            0
  #define MCU_EEPROM_SIZE                                 4096

  #if defined(ATMEGA1281) || defined(ATMEGA128RFA1)
    #define IMAGE_SIZE                                  131072
    #define MCU_FLASH_SIZE                          (IMAGE_SIZE - MCU_EEPROM_SIZE)
    #define IMAGE1_START_ADDRESS                     EXT_FLASH_START_ADDRESS
    #define IMAGE2_START_ADDRESS                    (EXT_FLASH_START_ADDRESS + IMAGE_SIZE)
  #elif  defined(ATXMEGA256A3) || defined(ATXMEGA256D3)
    #define IMAGE_SIZE                                  266240
    #define MCU_FLASH_SIZE                              262144
    #define IMAGE1_START_ADDRESS                     EXT_FLASH_START_ADDRESS
    #define IMAGE2_START_ADDRESS                    (EXT_FLASH_START_ADDRESS + IMAGE_SIZE)
  #endif

#elif EXTERNAL_MEMORY == AT25DF041A

  #if !defined(ATXMEGA256A3)
    #error 'at25df041a is supported only for atxmega256a3.'
  #endif

  // flash instruction format (see at25df041a manual)
  #define READ          0x03
  #define RDID          0x9F

  // Atmel ID
  #define MANUFACTURER_ID     0x1F
  // at25df041a
  #define DEVICE_ID_1         0x44
  #define DEVICE_ID_2         0x01
  #define EXT_STRING_LENGTH   0x00

  #define PAGE_SIZE             256

  // 4 Mbits
  #define FLASH_SIZE                                  524288
  #define EXT_FLASH_START_ADDRESS                          0
  // image consists mcu flash size + eeprom size. 256k + 4k
  #define IMAGE_SIZE                                  266240
  #define IMAGE1_START_ADDRESS       EXT_FLASH_START_ADDRESS
  // this is fake start address used for code compatibility
  #define IMAGE2_START_ADDRESS       (EXT_FLASH_START_ADDRESS + IMAGE_SIZE)

  #define MCU_FLASH_SIZE                               262144
  #define MCU_EEPROM_SIZE                                4096

#else
  #error 'Unknown memory type.'
#endif

// 1 byte (action for bootloader), 2 bytes (images crc), 1 byte table of image types.
#define SERVICE_INFO_SIZE                                 4

#define LITTLE_TO_BIG_ENDIAN(A)  ((((uint32_t)A & 0xFFul) << 24)   \
                                | (((uint32_t)A & 0xFF00ul) << 8)   \
                                | (((uint32_t)A >> 8) & 0xFF00ul)   \
                                | (((uint32_t)A >> 24) & 0xFFul))

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief Number of position for image in the external flash */
typedef enum
{
  POSITION_1,
  POSITION_2,
  POSITION_MAX
} ImagePosition_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Check availability of the external flash. Reads vendor and chip ID
from the external flash.

\return
  true - correct memory, \n
  false - other
******************************************************************************/
bool memCheckMem(void);

/**************************************************************************//**
\brief Reads data from memory.

\param[in]
  offset -  internal flash address;
\param[in]
  buffer -  pointer to the data buffer;
\param[in]
  length -  number bytes for reading;
******************************************************************************/
void memReadData(uint32_t offset, uint8_t *data, uint16_t size);

/**************************************************************************//**
\brief Starts external flash loader. Makes all tests and loads image.
******************************************************************************/
void extLoadDataFromExtMem(void);

#endif /* _EXTMEMREADER_H */
