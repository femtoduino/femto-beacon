/****************************************************************************//**
  \file enumeration.c

  \brief Implementation of enumeration proccess.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    01.09.11 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <usbShared.h>
#include <descriptors.h>
#include <dfuClass.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// request codes for runtime work
#define USB_GET_STATUS             0
#define USB_CLEAR_FEATURE          1
#define USB_SET_FEATURE            3
#define USB_SET_ADDRESS            5
#define USB_GET_DESCRIPTOR         6
#define USB_SET_DESCRIPTOR         7
#define USB_GET_CONFIGURATION      8
#define USB_SET_CONFIGURATION      9
#define USB_GET_INTERFACE          10
#define USB_SET_INTERFACE          11
#define USB_SYNCH_FRAME            12

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void runtimeSetConfiguration(uint8_t cfgnum);
static void runtimeGetDescriptorHandler(uint8_t type, uint8_t index, uint16_t length);

/******************************************************************************
                   External global variables section
******************************************************************************/
extern UsbRequest_t request;
extern const ConfigurationFrameResponse_t configDescriptor;
extern const DeviceDescriptor_t deviceDescrptr;

/******************************************************************************
                   Global variables section
******************************************************************************/
// virtual communication port address on usb
static uint16_t usbAddress = 0;

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Configures the device by setting it into the configured state.

Parameters:
  cfgnum - configuration number to set
******************************************************************************/
static void runtimeSetConfiguration(uint8_t cfgnum)
{
  // Set & save the desired configuration
  setConfig(cfgnum);
  // Acknowledge the request
  txZLP();
}

/******************************************************************************
runtime get descriptor command handler

Parameters:
  type   - type of the requested descriptor
  index  - index of the requested descriptor
  length - maximum number of bytes to return
******************************************************************************/
static void runtimeGetDescriptorHandler(uint8_t type, uint8_t index, uint16_t length)
{
  (void)index;

  // Check the descriptor type
  switch (type)
  {
    case DESCRIPTOR_TYPE_DEVICE:
        // Adjust length and send descriptor
        if (length > deviceDescrptr.bLength)
          length = deviceDescrptr.bLength;
        txUsbData((void *)&deviceDescrptr, length, 0, 0);
      break;
    case DESCRIPTOR_TYPE_CONFIGURATION:
        // Adjust length and send descriptor
        if (length > configDescriptor.config.wTotalLength)
          length = configDescriptor.config.wTotalLength;
        txUsbData((void *)&configDescriptor, length, 0, 0);
      break;
    default:
        stallData();
      break;
  }
}

/******************************************************************************
Standard usb request handler
******************************************************************************/
void runtimeRequestHandler(void)
{
  // Check request code
  switch (request.bRequest)
  {
    case USB_GET_DESCRIPTOR:
        // Send the requested descriptor
        runtimeGetDescriptorHandler((uint8_t)(request.wValue >> 8), (uint8_t)(request.wValue), request.wLength);
      break;
    case USB_SET_ADDRESS:
        usbAddress = request.wValue & 0x7F;
        txUsbData(0, 0, (TransferCallback_t)setUsbAddress, (void *)&usbAddress);
      break;
    case USB_SET_CONFIGURATION:
        // Set the requested configuration
        runtimeSetConfiguration((uint8_t)request.wValue);
      break;
    default:
        dfuRequestHandler();
      break;
  }
}

// eof enumeration.c
