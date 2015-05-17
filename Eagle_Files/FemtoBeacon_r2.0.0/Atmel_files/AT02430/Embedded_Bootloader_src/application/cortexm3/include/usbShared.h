/****************************************************************************//**
  \file usbShared.h

  \brief Declaration of common usb interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    01.09.11 A. Khromykh - Created
*******************************************************************************/

#ifndef _USBSHARED_H
#define _USBSHARED_H
/******************************************************************************
                   Includes section
******************************************************************************/
#include <dfuClass.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define END_POINT_MAX_SIZE          64
#define REQUEST_DATA_SIZE           END_POINT_MAX_SIZE

/******************************************************************************
                   Types section
******************************************************************************/
// transaction callback type
typedef void (* TransferCallback_t)(void *pArg, uint16_t transferred, uint16_t remaining);

BEGIN_PACK
// Usb host request
typedef struct PACK
{
  uint8_t  bmRequestType;
  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
  uint8_t  bData[REQUEST_DATA_SIZE];
} UsbRequest_t;

typedef union PACK
{
  DfuGetStatusResponse_t getStatusResponse;
  DfuGetStateResponse_t  getStateResponse;
} UsbResponse_t;
END_PACK

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Sends data through a USB endpoint.

\param[in]
  data - Pointer to a buffer with the data to send.
\param[in]
  size - Size of the data buffer.
\param[in]
  callback - Optional callback function to invoke when the transfer is complete.
\param[in]
  argument - Optional argument to the callback function.
******************************************************************************/
void txUsbData(void *data, uint32_t size, TransferCallback_t callback, void *argument);

/**************************************************************************//**
\brief Reads incoming data on an USB endpoint.

\param[in]
  data - Pointer to a data buffer.
\param[in]
  size - Size of the data buffer in bytes.
\param[in]
  callback - Optional end-of-transfer callback function.
\param[in]
  argument - Optional argument to the callback function.
******************************************************************************/
void rxUsbData(void *data, uint32_t size, TransferCallback_t callback, void *argument);

/**************************************************************************//**
\brief send zero-length packet
******************************************************************************/
void txZLP(void);

/**************************************************************************//**
\brief Causes the given endpoint to acknowledge the next packet it receives with
a STALL handshake.
******************************************************************************/
void stallData(void);

/**************************************************************************//**
\brief Sets the device address to the given value.

\param[in]
  address - New device address.
******************************************************************************/
void setUsbAddress(uint8_t *address, uint16_t transferred, uint16_t remaining);

/**************************************************************************//**
\brief Sets the current device configuration.

\param[in]
  cfgnum - Configuration number to set.
******************************************************************************/
void setConfig(uint8_t cfgnum);

/**************************************************************************//**
\brief Standard usb request handler
******************************************************************************/
void runtimeRequestHandler(void);

#endif /* _USBSHARED_H */
