/****************************************************************************//**
  \file abstractSerializer.c

  \brief Usage of all declared serial interfaces.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    5.02.11 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <abstractSerializer.h>
#include <timerInterval.h>
#include <srecParser.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define UPPER_BYTE_OFFSET          3

/******************************************************************************
                   Global variables section
******************************************************************************/
// interfaces list
static SerialInterface_t  serialInterface[] =
{
#if (USE_USB_DFU == 1)
  {
    .hwInit   = hwInitUsbDfu,
    .hwUnInit = hwUnInitUsbDfu,
    .getByte  = getByteUsbDfu,
    .setByte  = setByteUsbDfu
  },
#endif
#if (USE_USART0 == 1)
  {
    .hwInit   = hwInitUsart0,
    .hwUnInit = hwUnInitUsart0,
    .getByte  = getByteUsart,
    .setByte  = setByteUsart
  },
#endif
#if (USE_USART1 == 1)
  {
    .hwInit   = hwInitUsart1,
    .hwUnInit = hwUnInitUsart1,
    .getByte  = getByteUsart,
    .setByte  = setByteUsart
  },
#endif
#if (USE_USB_FIFO == 1)
  {
    .hwInit   = hwInitUsbFifo,
    .hwUnInit = hwUnInitUsbFifo,
    .getByte  = getByteUsbFifo,
    .setByte  = setByteUsbFifo
  },
#endif
#if (USE_USARTD0 == 1)
  {
    .hwInit   = hwInitUsartD0,
    .hwUnInit = hwUnInitUsart,
    .getByte  = getByteUsart,
    .setByte  = setByteUsart
  },
#endif
#if (USE_USARTF0 == 1)
  {
    .hwInit   = hwInitUsartF0,
    .hwUnInit = hwUnInitUsart,
    .getByte  = getByteUsart,
    .setByte  = setByteUsart
  },
#endif
#if (USE_SPIE == 1)
  {
    .hwInit   = hwInitSpiE,
    .hwUnInit = hwUnInitSpi,
    .getByte  = getByteSpi,
    .setByte  = setByteSpi
  },
#endif
};

static uint8_t currentInterface;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Clear interface setting.
******************************************************************************/
void bootUnInitSerializer(void)
{
  serialInterface[currentInterface].hwUnInit();
  lowLevelUnInit();
}

/**************************************************************************//**
\brief Perform search interface activity and handshake message.

\return
  true - handshake was read, false - there is no activity.
******************************************************************************/
bool bootScanInterfaces(void)
{
  bool result;
  uint8_t itr = sizeof(serialInterface) / sizeof(SerialInterface_t);
  uint32_t byteThread;

  timerIntervalInit();

  do {
    itr--;

    serialInterface[itr].hwInit();
    timerIntervalStart();

    while (!timerReadIntervalState()) // wait for about 200 ms
    {
      result = serialInterface[itr].getByte((uint8_t *)&byteThread + UPPER_BYTE_OFFSET);

      if (result) // activity has been found
      {
        if (byteThread == HANDSHAKE_REQ)
          break;

        byteThread >>= 8;
        result = false;
      }
    }

    if (!result)
      serialInterface[itr].hwUnInit();

    currentInterface = itr;
    timerIntervalStop();

  } while (itr && !result);

  // clear timer
  timerIntervalUnInit();

  return result;
}

/**************************************************************************//**
\brief Read byte from interface.

\param[out]
  p - pointer to data buffer;
\param[in]
  flag - flag about unlimited or limited reading time.

\return
  true - byte was read, false - byte was not read.
******************************************************************************/
bool bootGetChar(uint8_t *p, bool flag)
{
  bool result;

  do {
    result = serialInterface[currentInterface].getByte(p);
  } while (!result && flag);

  return result;
}

/**************************************************************************//**
\brief Write byte to interface.

\param[in]
  len - data length;
\param[in]
  p - pointer to data buffer.
******************************************************************************/
void bootSetChar(uint16_t len, uint8_t *p)
{
  serialInterface[currentInterface].setByte(len, p);
}

// eof abstractSerializer.c
