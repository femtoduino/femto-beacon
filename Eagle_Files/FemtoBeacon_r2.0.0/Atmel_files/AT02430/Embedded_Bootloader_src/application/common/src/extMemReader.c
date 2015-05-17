/****************************************************************************//**
  \file extMemReader.c

  \brief Look through action, check image crc and read data from external memory.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    17/08/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#ifdef EXT_MEMORY

#include <types.h>
#include <extMemReader.h>
#include <spiMemInterface.h>
#include <bootStructure.h>
#include <eepromLoader.h>
#include <flashLoader.h>
#include <abstractSerializer.h>

/******************************************************************************
                   External global variables section
******************************************************************************/
extern const uint32_t imageStartAddress[POSITION_MAX];
extern BootBuffer_t dataBuffer;

/******************************************************************************
                   Global variables section
******************************************************************************/
uint8_t readCrc;
uint8_t countedCrc = CRC_INITIALIZATION_VALUE;
ImagePosition_t imageNumber;
uint8_t eepromServiceArea[SERVICE_INFO_SIZE];

/******************************************************************************
                     Prototypes section
******************************************************************************/
static inline uint8_t extMemCrc(uint8_t crc, uint8_t *pcBlock, uint8_t length);
static inline void extCleanServiceEepromArea(void);
static inline bool extCheckAction(void);
static inline bool extCheckCrc(void);
static inline void extReadDataForFlashArea(void);
static inline void extReadDataForEepromArea(void);

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Counts crc current memory area. CRC-8. Polynom 0x31    x^8 + x^5 + x^4 + 1.

\param[in]
  crc - first crc state
\param[in]
  pcBlock - pointer to the memory for crc counting
\param[in]
  length - memory size

\return
  current area crc
******************************************************************************/
static inline uint8_t extMemCrc(uint8_t crc, uint8_t *pcBlock, uint8_t length)
{
  uint8_t i;

  while (length--)
  {
    crc ^= *pcBlock++;

    for (i = 0; i < 8; i++)
      crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
  }

  return crc;
}

/**************************************************************************//**
\brief Clears eeprom service area.
******************************************************************************/
static inline void extCleanServiceEepromArea(void)
{
  memset(dataBuffer.data, EMPTY_EEPROM_VALUE, SERVICE_INFO_SIZE);
  dataBuffer.address = EEPROM_START_ADDRESS;
  dataBuffer.dataSize = SERVICE_INFO_SIZE;
  eepromPacketToPage();
  eepromFlushPage();
}

/**************************************************************************//**
\brief Checks bootloader action. Clears service area if incorrect action.

\return
  true - correct action, \n
  false - other
******************************************************************************/
static inline bool extCheckAction(void)
{
  uint8_t i;

  for (i = 0; i < SERVICE_INFO_SIZE; i++)
    eepromServiceArea[i] = eepromReadData(i);

  imageNumber = (ImagePosition_t)eepromServiceArea[ACTION_POSITION];
  // check eeprom service area
  if (!(imageNumber < POSITION_MAX))
  {
    if (EMPTY_EEPROM_VALUE != eepromServiceArea[ACTION_POSITION])
    { // clear service eeprom area
      extCleanServiceEepromArea();
    }
    return false;
  }

  readCrc = eepromServiceArea[imageNumber+1];
  return true;
}

/**************************************************************************//**
\brief Checks image crc. Clears service area if incorrect crc.

\return
  true - correct crc, \n
  false - other
******************************************************************************/
static inline bool extCheckCrc(void)
{
  uint32_t address = 0ul;

  while (address < IMAGE_SIZE)
  {
    memReadData(address+imageStartAddress[imageNumber], dataBuffer.data, SREC_DATA_LENGTH);
    countedCrc = extMemCrc(countedCrc, dataBuffer.data, SREC_DATA_LENGTH);
    address += SREC_DATA_LENGTH;
  }

  if (countedCrc == readCrc)
  {
    return true;
  }
  else
  { // clear service eeprom area
    extCleanServiceEepromArea();
    return false;
  }
}

/**************************************************************************//**
\brief Reads data from external flash and loads to internal flash.
******************************************************************************/
static inline void extReadDataForFlashArea(void)
{
  uint32_t address = 0ul;

  while (address < MCU_FLASH_SIZE)
  {
    memReadData(address+imageStartAddress[imageNumber], dataBuffer.data, SREC_DATA_LENGTH);
    dataBuffer.address = address;
    dataBuffer.dataSize = SREC_DATA_LENGTH;
    flashPacketToPage();
    address += SREC_DATA_LENGTH;
  }

  flashFlushPage();
}

/**************************************************************************//**
\brief Reads data from external flash and loads to internal eeprom.
******************************************************************************/
static inline void extReadDataForEepromArea(void)
{
  uint32_t address = MCU_FLASH_SIZE;
  uint32_t eepromAddres = EEPROM_START_ADDRESS;

  while (address < IMAGE_SIZE)
  {
    memReadData(address+imageStartAddress[imageNumber], dataBuffer.data, SREC_DATA_LENGTH);
    dataBuffer.address = eepromAddres;
    dataBuffer.dataSize = SREC_DATA_LENGTH;
    if (EEPROM_START_ADDRESS == eepromAddres)
    { // delete bootloader action
      eepromServiceArea[ACTION_POSITION] = EMPTY_EEPROM_VALUE;
      memcpy(dataBuffer.data, eepromServiceArea, SERVICE_INFO_SIZE);
    }
    eepromPacketToPage();
    eepromAddres += SREC_DATA_LENGTH;
    address += SREC_DATA_LENGTH;
  }

  eepromFlushPage();
}

/**************************************************************************//**
\brief Starts external flash loader. Makes all tests and loads image.
******************************************************************************/
void extLoadDataFromExtMem(void)
{
  // init serial interface
  spiMemInit();

  // check availability of the external flash
  if (false == memCheckMem())
  {
    spiMemUnInit();
    return;
  }

  // checks action
  if (false == extCheckAction())
  {
    spiMemUnInit();
    return;
  }

  // checks crc
  if (false == extCheckCrc())
  {
    spiMemUnInit();
    return;
  }

  // load internal flash
  extReadDataForFlashArea();
  // load internal eeprom
  extReadDataForEepromArea();

  // clear spi interface
  spiMemUnInit();
  // clear low level initialization
  lowLevelUnInit();

  // go to NULL address
  void(* startAddress)(void) =(void(*)(void))NULL;
  // nulling extended indirect register for indirect call out memory
  CLEAR_EIND();
  // go to NULL address
  startAddress();
}

#endif //EXT_MEMORY

// eof extMemReader.c
