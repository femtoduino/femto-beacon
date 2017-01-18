/****************************************************************************//**
  \file srecParser.c

  \brief Implementation of real time srec parser.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2/04/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <abstractSerializer.h>
#include <bootStructure.h>
#include <flashLoader.h>
#include <eepromLoader.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void srecWaitForType(SrecordType_t *srecType);
static uint8_t srecWaitForAddress(uint8_t addressLength);
static bool srecParser(void);

/******************************************************************************
                   Global variables section
******************************************************************************/
BootBuffer_t dataBuffer;
static uint32_t srecACK  = ACK;
static uint32_t srecNACK = NACK;
static uint32_t previousAddress = 0xFFFFFFFF;
static uint32_t handshakeConf = HANDSHAKE_CONF;

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Waiting for a field of the type from the serial interface.

\param[out]
  srecType - pointer to buffer;
******************************************************************************/
static void srecWaitForType(SrecordType_t *srecType)
{
  do
  {
    bootGetChar((uint8_t *)srecType, INFINITE);
  } while(S_SYMBOL != (uint8_t)*srecType);
  *srecType <<= 8;
  bootGetChar((uint8_t *)srecType, INFINITE);
}

/**************************************************************************//**
\brief Waiting for a field of the type from the internal address

\param[in]
  addressLength - length of address field;

\return
  checksum with address field
******************************************************************************/
static uint8_t srecWaitForAddress(uint8_t addressLength)
{
  uint8_t checksum = 0;

  while(addressLength)
  {
    addressLength--;
    bootGetChar((((uint8_t *)&dataBuffer.address) + addressLength), INFINITE);
    checksum += *(((uint8_t *)&dataBuffer.address) + addressLength);
  }

  return checksum;
}

/**************************************************************************//**
\brief Communication support between host and mcu
******************************************************************************/
void srecProtocol(void)
{
  while(1)
  {
    if (srecParser())
    {
      if ((S0 == dataBuffer.recordType) || (S5 == dataBuffer.recordType))
      {
        bootSetChar(sizeof(srecACK), (uint8_t*)(&srecACK));
        continue;
      }

      if ((S7 == dataBuffer.recordType) || (S8 == dataBuffer.recordType) || (S9 == dataBuffer.recordType))
      {
        flashFlushPage();
        eepromFlushPage();
        bootSetChar(sizeof(srecACK), (uint8_t*)(&srecACK));
        bootUnInitSerializer();
        void(* startAddress)(void) =(void(*)(void))(hw_platform_address_size_t)dataBuffer.address;
        // nulling extended indirect register for indirect call out memory
        CLEAR_EIND();
        // go to application start
        startAddress();
      }

      if (previousAddress == dataBuffer.address)
      { // host did not listen ack and repeat previous packet
        bootSetChar(sizeof(srecACK), (uint8_t*)(&srecACK));
        continue;
      }
      previousAddress = dataBuffer.address;

      if ((dataBuffer.address + dataBuffer.dataSize) <= FLASH_END_ADDRESS)
      { // flash section
        flashPacketToPage();
      }
      else if (dataBuffer.address < EEPROM_START_ADDRESS)
      {
        bootSetChar(sizeof(srecNACK), (uint8_t*)(&srecNACK));
        continue;
      }

      if ((dataBuffer.address >= EEPROM_START_ADDRESS) && (dataBuffer.address <= EEPROM_END_ADDRESS))
      { // eeprom section
        eepromPacketToPage();
      }

      // send ack
      bootSetChar(sizeof(srecACK), (uint8_t*)(&srecACK));
    }
    else
    {
      bootSetChar(sizeof(srecNACK), (uint8_t*)(&srecNACK));
    }
  }
}

/**************************************************************************//**
\brief Real time parser of srec format.

\return
  false - parse error
  true  - parse ok
******************************************************************************/
static bool srecParser(void)
{
  uint8_t addressLength;
  uint8_t packetLength;
  uint8_t checksum = 0;
  uint8_t checksumFromSerial;

  // start of packet is transmitted as ascii code.
  // the others are transmitted as hex code.
  // search begin of packet
  srecWaitForType(&dataBuffer.recordType);

  switch (dataBuffer.recordType)
  {
    case S0: // the address field is 2 bytes
    case S1:
    case S5:
    case S9:
        addressLength = 2;
      break;
    case S2: // the address field is 3 bytes
    case S8:
        addressLength = 3;
      break;
    case S3: // the address field is 4 bytes
    case S7:
        addressLength = 4;
      break;
    default: // unsupported type
      return false;
   }

  // get packet length
  bootGetChar(&packetLength, INFINITE);
  if (packetLength > (SREC_DATA_LENGTH + addressLength + CHECKSUM_AREA_SIZE))
    return false; // this length is not correct
  checksum += packetLength;
  dataBuffer.dataSize = packetLength - addressLength - CHECKSUM_AREA_SIZE;

  // get internal address
  checksum += srecWaitForAddress(addressLength);
  packetLength -= addressLength;

  // get data array
  uint8_t *tempPoiner = dataBuffer.data;
  while (--packetLength)
  {
    bootGetChar(tempPoiner, INFINITE);
    checksum += *tempPoiner++;
  }

  // get checksum
  bootGetChar(&checksumFromSerial, INFINITE);
  checksum += checksumFromSerial;
  if (0xFF != checksum)
    return false;

  return true;
}

/**************************************************************************//**
\brief Handshake procedure.
******************************************************************************/
void srecHandshake(void)
{
  if (bootScanInterfaces())
  {
    bootSetChar(sizeof(handshakeConf), (uint8_t *)(&handshakeConf)); // ack for handshake
    return;
  }

  bootUnInitSerializer();
  void(* startAddress)(void) =(void(*)(void))NULL;

  // nulling extended indirect register for indirect call out memory
  CLEAR_EIND();
  // go to NULL address
  startAddress();
}

// eof srecParser.c
