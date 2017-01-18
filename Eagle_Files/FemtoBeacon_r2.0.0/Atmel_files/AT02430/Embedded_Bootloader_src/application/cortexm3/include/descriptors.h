/****************************************************************************//**
  \file descriptors.h

  \brief Declaration of descriptors structures.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    05.09.11 A. Khromykh - Created
*******************************************************************************/
#ifndef _DESCRIPTORS_H
#define _DESCRIPTORS_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// Descriptor's type definitions.
#define DESCRIPTOR_TYPE_DEVICE           0x01
#define DESCRIPTOR_TYPE_CONFIGURATION    0x02
#define DESCRIPTOR_TYPE_INTERFACE        0x04
#define DESCRIPTOR_TYPE_DFU_FUNCTIONAL   0x21

/******************************************************************************
                   Types section
******************************************************************************/
// USB standard device descriptor structure.
BEGIN_PACK
typedef struct PACK
{
  uint8_t  bLength;            // Size of this descriptor in bytes
  uint8_t  bDescriptorType;    // Descriptor type
  uint16_t bcdUSB;             // USB specification release number in BCD format
  uint8_t  bDeviceClass;       // Device class code
  uint8_t  bDeviceSubClass;    // Device subclass code
  uint8_t  bDeviceProtocol;    // Device protocol code
  uint8_t  bMaxPacketSize0;    // Maximum packet size of endpoint 0 (in bytes)
  uint16_t idVendor;           // Vendor ID
  uint16_t idProduct;          // Product ID
  uint16_t bcdDevice;          // Device release number in BCD format
  uint8_t  iManufacturer;      // Index of the manufacturer string descriptor
  uint8_t  iProduct;           // Index of the product string descriptor
  uint8_t  iSerialNumber;      // Index of the serial number string descriptor
  uint8_t  bNumConfigurations; // Number of possible configurations for the device
} DeviceDescriptor_t;

// USB standard configuration descriptor structure.
typedef struct PACK
{
  uint8_t  bLength;             // Size of the descriptor in bytes
  uint8_t  bDescriptorType;     // Descriptor type
  uint16_t wTotalLength;        // Length of all descriptors returned along with this configuration descriptor
  uint8_t  bNumInterfaces;      // Number of interfaces in this configuration
  uint8_t  bConfigurationValue; // Value for selecting this configuration
  uint8_t  iConfiguration;      // Index of the configuration string descriptor
  uint8_t  bmAttributes;        // Configuration characteristics
  uint8_t  bMaxPower;           // Maximum power consumption of the device when in this configuration
} ConfigurationDescriptor_t;

// USB standard interface descriptor structure.
typedef struct PACK
{
  uint8_t bLength;            // Size of the descriptor in bytes
  uint8_t bDescriptorType;    // Descriptor type
  uint8_t bInterfaceNumber;   // Number of the interface in its configuration
  uint8_t bAlternateSetting;  // Value to select this alternate interface setting
  uint8_t bNumEndpoints;      // Number of endpoints used by the interface (excluding endpoint 0)
  uint8_t bInterfaceClass;    // Interface class code
  uint8_t bInterfaceSubClass; // Interface subclass code
  uint8_t bInterfaceProtocol; // Interface protocol code
  uint8_t iInterface;         // Index of the interface string descriptor
} InterfaceDescriptor_t;

// USB run-time functional descriptor.
typedef struct PACK
{
  uint8_t  bLength;            // Size of the descriptor in bytes
  uint8_t  bDescriptorType;    // Descriptor type
  uint8_t  bAttributes;        // DFU attributes
  uint16_t wDetachTimeOut;     // Time in milliseconds, that the device will wait after receipt of the DFU_DETACH request.
  uint16_t wtransferSize;      // Maximum number of bytes that the device can accept per control-write transaction.
  uint16_t bcdDFUVersion;      // Numeric expression identifying the version of the DFU Specification release.
} DfuFunctionalDescriptor_t;

// USB frame for interface request.
typedef struct PACK
{
  ConfigurationDescriptor_t             config;
  InterfaceDescriptor_t                 interface;
  DfuFunctionalDescriptor_t             runtimeFunctional;
} ConfigurationFrameResponse_t;
END_PACK

#endif /* _DESCRIPTORS_H */
