/****************************************************************************//**
  \file descriptors.c

  \brief Usb port descriptors.

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
#include <descriptors.h>
#include <usbShared.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// String definitions' indexes.
#define MANUFACTURER_STRING_INDEX     0
#define PRODUCT_STRING_INDEX          0
#define SERIAL_NUMBER_STRING_INDEX    0
#define CONFIGURATION_STRING_INDEX    0
#define THIS_INTERFACE_STRING_INDEX   0

#define VENDOR_ID       0x03EB    // Atmel
//#define PRODUCT_ID      0x6119    // COM port from application notes for AT91
#define PRODUCT_ID      0x6118    // bootloader DFU mode 
#define DEVICE_RELEASE  0x0001
#define USB_BUS_RELEASE 0x0200

// Number of possible configurations for the device.
#define NUMBER_OF_CONFIGURATIONS     0x01

// Class specification parameters of communication device.
#define DEVICE_CLASS       0x00
#define DEVICE_SUBCLASS    0x00
#define DEVICE_PROTOCOL    0x00

// Endpoint definitions' sizes.
#define SZ_CONTROL_ENDPOINT      END_POINT_MAX_SIZE  // endpoint 0 is control pipe

// Configuration descriptor parameters.
// Common size all descriptors in the configuration
#define COMMON_CFG_SIZE          (sizeof(ConfigurationDescriptor_t) + sizeof(InterfaceDescriptor_t) + sizeof(DfuFunctionalDescriptor_t))
#define NUMBER_OF_INTERFACES      0x01
#define CFG_SELECTING_VALUE       0x01
#define CFG_CHARACTERISTICS       0x80  // D7 is reserved and must be set to one for historical reasons.
#define MAXIMUM_POWER_CONSUMPTION 0x32  // Step is 2 mA.

// Parameters for interfaces descriptors
#define NUMBER_OF_THIS_INTERFACE           0x00
#define ALTERNATIVE_SETTING                0x00
#define NUMBER_USING_ENDPOINTS_THIS_IFC    0x00
#define THIS_IFC_CLASS                     0xFE
#define THIS_IFC_SUBCLASS                  0x01
#define THIS_IFC_PROTOCOL                  0x02

// Parameters for functional descriptors
#define ATTRIBUTE_BIT_MASK        0x0D    //7..4 bits reserved, device will perform a bus detach-attach sequence when it recieves DFU_DETACH,
                                          // device is able to communicate via USB after Manifestation phase,
                                          // uploade disable, download enable
#define DETACH_TIMEOUT            0x0064  // waiting time of usb reset after dfu detach command
#define TRANSFER_SIZE             END_POINT_MAX_SIZE
#define DFU_VERSION               0x0101

/******************************************************************************
                   Constants section
******************************************************************************/
const DeviceDescriptor_t deviceDescrptr = {
  sizeof(DeviceDescriptor_t), // Size of this descriptor in bytes
  DESCRIPTOR_TYPE_DEVICE,     // Descriptor type
  USB_BUS_RELEASE,            // USB specification release number in BCD format
  DEVICE_CLASS,               // Device class code
  DEVICE_SUBCLASS,            // Device subclass code
  DEVICE_PROTOCOL,            // Device protocol code
  SZ_CONTROL_ENDPOINT,        // Maximum packet size of endpoint 0 (in bytes)
  VENDOR_ID,                  // Vendor ID
  PRODUCT_ID,                 // Product ID
  DEVICE_RELEASE,             // Device release number in BCD format
  MANUFACTURER_STRING_INDEX,  // Index of the manufacturer string descriptor
  PRODUCT_STRING_INDEX,       // Index of the product string descriptor
  SERIAL_NUMBER_STRING_INDEX, // Index of the serial number string descriptor
  NUMBER_OF_CONFIGURATIONS    // Number of possible configurations for the device
};

const ConfigurationFrameResponse_t configDescriptor = {
  { // configuration
    sizeof(ConfigurationDescriptor_t),  // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_CONFIGURATION,      // Descriptor type
    COMMON_CFG_SIZE,                    // Length of all descriptors returned along with this configuration descriptor
    NUMBER_OF_INTERFACES,               // Number of interfaces in this configuration
    CFG_SELECTING_VALUE,                // Value for selecting this configuration
    CONFIGURATION_STRING_INDEX,         // Index of the configuration string descriptor
    CFG_CHARACTERISTICS,                // Configuration characteristics
    MAXIMUM_POWER_CONSUMPTION           // Maximum power consumption of the device when in this configuration
  },
  { // interface
    sizeof(InterfaceDescriptor_t),      // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_INTERFACE,          // Descriptor type
    NUMBER_OF_THIS_INTERFACE,           // Number of the interface in its configuration
    ALTERNATIVE_SETTING,                // Value to select this alternate interface setting
    NUMBER_USING_ENDPOINTS_THIS_IFC,    // Number of endpoints used by the interface (excluding endpoint 0)
    THIS_IFC_CLASS,                     // Interface class code
    THIS_IFC_SUBCLASS,                  // Interface subclass code
    THIS_IFC_PROTOCOL,                  // Interface protocol code
    THIS_INTERFACE_STRING_INDEX         // Index of the interface string descriptor
  },
  { // dfu functional descriptor
    sizeof(DfuFunctionalDescriptor_t),     // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_DFU_FUNCTIONAL,        // Descriptor type
    ATTRIBUTE_BIT_MASK,                    // DFU attributes
    DETACH_TIMEOUT,                        // waiting time of usb reset after dfu detach command
    TRANSFER_SIZE,                         // Maximum number of bytes that the device can accept per control-write transaction.
    DFU_VERSION                            // dfu version in BCD
  }
};

// eof descriptors.c
