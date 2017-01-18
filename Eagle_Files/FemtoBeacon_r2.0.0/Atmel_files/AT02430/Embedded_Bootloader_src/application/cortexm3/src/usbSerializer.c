/****************************************************************************//**
  \file usbSerializer.c

  \brief Implementation of handler of usb hw.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29.08.11 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <AT91SAM3S4.h>
#include <core_cm3.h>
#include <types.h>
#include <hardwareInit.h>
#include <usbShared.h>
#include <srecParser.h>
#include <preparser.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define EP_OUT                       0
#define EP_NUMBER                    0
// Endpoint is idle (i.e. ready for transmission)
#define UDP_ENDPOINT_IDLE            0
// Endpoint is sending data
#define UDP_ENDPOINT_SENDING         1
// Endpoint is receiving data
#define UDP_ENDPOINT_RECEIVING       2
// cortex peripheral ID features for power managment controller
#define START_ID_NUMBER_FOR_PMC1     32
// UDP_RXDATA - Bit mask for both banks of the UDP_CSR register.
#define UDP_RXDATA              (UDP_CSR_RX_DATA_BK0 | UDP_CSR_RX_DATA_BK1)

/******************************************************************************
                   Types section
******************************************************************************/
/* Describes an ongoing transfer on a UDP endpoint. */
typedef struct
{
  uint8_t *data;               // data - Pointer to a data buffer used for emission/reception
  uint32_t buffered;           // buffered - Number of bytes which have been written into the UDP internal FIFO buffers
  uint32_t transferred;        // transferred - Number of bytes which have been sent/received
  uint32_t remaining;          // remaining - Number of bytes which have not been buffered/transferred yet
  TransferCallback_t callback; // callback - Optional callback to invoke when the transfer completes
  void *argument;              // argument - Optional argument to the callback function
} UsbTransfer_t;

typedef enum
{
  USB_HANDSHAKE_STATE,
  USB_DFU_STATE
} UsbState_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void writePayload(void);
static void readPayload(uint16_t size);
static void endOfTransfer(void);
static void epHandler(void);

/******************************************************************************
                   Static inline functions section
******************************************************************************/
/**************************************************************************//**
\brief Clears the specified bit(s) in the UDP_CSR register.
******************************************************************************/
static inline void clearCSR(uint32_t flags)
{
  volatile uint32_t reg;

  reg = UDP->UDP_CSR[EP_NUMBER];
  reg &= ~(flags);
  reg |= UDP_RXDATA;
  UDP->UDP_CSR[EP_NUMBER] = reg;
  while ((UDP->UDP_CSR[EP_NUMBER] & (flags)) == ((uint32_t)flags));
}

/**************************************************************************//**
\brief Sets the specified bit(s) in the UDP_CSR register.
******************************************************************************/
static inline void setCSR(uint32_t flags)
{
  volatile uint32_t reg;

  reg = UDP->UDP_CSR[EP_NUMBER];
  reg |= (flags);
  reg |= UDP_RXDATA;
  UDP->UDP_CSR[EP_NUMBER] = reg;
  while ((UDP->UDP_CSR[EP_NUMBER] & (flags)) != ((uint32_t)flags));
}

/******************************************************************************
                   Global variables section
******************************************************************************/
uint32_t epStatus;
uint8_t epState = UDP_ENDPOINT_IDLE;
UsbTransfer_t transfer;
UsbRequest_t request;
UsbResponse_t response;
UsbState_t usbState = USB_HANDSHAKE_STATE;

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Sends data through a USB endpoint.

Parameters:
  data - Pointer to a buffer with the data to send.
  size - Size of the data buffer.
  callback - Optional callback function to invoke when the transfer is complete.
  argument - Optional argument to the callback function.
******************************************************************************/
void txUsbData(void *data, uint32_t size, TransferCallback_t callback, void *argument)
{
  // Setup the transfer descriptor
  transfer.data = (void *) data;
  transfer.remaining = size;
  transfer.buffered = 0;
  transfer.transferred = 0;
  transfer.callback = callback;
  transfer.argument = argument;

  // Send the first packet
  epState = UDP_ENDPOINT_SENDING;
  while (UDP_CSR_TXPKTRDY == (UDP->UDP_CSR[EP_NUMBER] & UDP_CSR_TXPKTRDY));
  writePayload();
  setCSR(UDP_CSR_TXPKTRDY);
}

/******************************************************************************
Reads incoming data on an USB endpoint.

Parameters:
  data - Pointer to a data buffer.
  size - Size of the data buffer in bytes.
  callback - Optional end-of-transfer callback function.
  argument - Optional argument to the callback function.
******************************************************************************/
void rxUsbData(void *data, uint32_t size, TransferCallback_t callback, void *argument)
{
  // Endpoint enters Receiving state
  epState = UDP_ENDPOINT_RECEIVING;

  // Set the transfer descriptor
  transfer.data = data;
  transfer.remaining = size;
  transfer.buffered = 0;
  transfer.transferred = 0;
  transfer.callback = callback;
  transfer.argument = argument;
}

/******************************************************************************
send zero-length packet
******************************************************************************/
void txZLP(void)
{
  // Acknowledge the request
  txUsbData(NULL, 0, NULL, NULL);
}

/******************************************************************************
Causes the given endpoint to acknowledge the next packet it receives with
a STALL handshake.
******************************************************************************/
void stallData(void)
{
  if (UDP_ENDPOINT_IDLE != epState)
    return;

  setCSR(UDP_CSR_FORCESTALL);
}

/******************************************************************************
Sets the device address to the given value.

Parameters:
  address - New device address.
******************************************************************************/
void setUsbAddress(uint8_t *address, uint16_t transferred, uint16_t remaining)
{
  (void)transferred;
  (void)remaining;

  // Set address
  UDP->UDP_FADDR = UDP_FADDR_FEN | *address;

  // If the address is non-zero, the device enters the Address state
  if (*address)
    UDP->UDP_GLB_STAT = UDP_GLB_STAT_FADDEN;
  // If the address is 0, the device returns to the Default state
  else
    UDP->UDP_GLB_STAT = 0;
}

/******************************************************************************
Sets the current device configuration.

Parameters:
  cfgnum - Configuration number to set.
******************************************************************************/
void setConfig(uint8_t cfgnum)
{
  // Configured state
  if (cfgnum)
    UDP->UDP_GLB_STAT |= UDP_GLB_STAT_CONFG;
  // If the configuration number is zero, the device goes back to the Address state
  else
    UDP->UDP_GLB_STAT = UDP_GLB_STAT_FADDEN;
}

/******************************************************************************
Writes a data payload into the current FIFO buffer of the UDP.
******************************************************************************/
static void writePayload(void)
{
  uint16_t size = END_POINT_MAX_SIZE;

  // Get the number of bytes to send
  if (size > transfer.remaining)
    size = transfer.remaining;

  // Update transfer descriptor information
  transfer.buffered += size;
  transfer.remaining -= size;

  // Write packet in the FIFO buffer
  while (size > 0)
  {
    UDP->UDP_FDR[EP_NUMBER] = *(transfer.data);
    transfer.data++;
    size--;
  }
}

/******************************************************************************
Reads a data payload from the current FIFO buffer of an endpoint.

Parameters:
  size - Size of the data to read.
******************************************************************************/
static void readPayload(uint16_t size)
{
  // Check that the requested size is not bigger than the remaining transfer
  if (size > transfer.remaining)
  {
    transfer.buffered += size - transfer.remaining;
    size = transfer.remaining;
  }

  // Update transfer descriptor information
  transfer.remaining -= size;
  transfer.transferred += size;

  // Retrieve packet
  while (size > 0)
  {
    *(transfer.data) = (uint8_t)UDP->UDP_FDR[EP_NUMBER];
    transfer.data++;
    size--;
  }
}

/******************************************************************************
Handles a completed transfer on the given endpoint, invoking the configured
callback if any.
******************************************************************************/
static void endOfTransfer(void)
{
  // Invoke callback is present
  if (NULL != transfer.callback)
  {
    ((TransferCallback_t) transfer.callback)(transfer.argument,
                                             transfer.transferred,
                                             transfer.remaining + transfer.buffered);
  }
}

/**************************************************************************//**
\brief End point handler.
******************************************************************************/
static void epHandler(void)
{
  uint16_t i;
  uint8_t *ptr;

  // wait for transaction on 0 endpoint
  while (!(UDP->UDP_ISR & UDP_ISR_EP0INT));
  // acknowledge end point 0 interrupt
  UDP->UDP_ICR = UDP_ISR_EP0INT;

  epStatus = UDP->UDP_CSR[EP_NUMBER];
  // Handle endpoint actions
  // IN packet sent
  if (epStatus & UDP_CSR_TXCOMP)
  {
    // Check that endpoint was in Sending state
    if (UDP_ENDPOINT_SENDING == epState)
    {
      if (transfer.buffered < END_POINT_MAX_SIZE)
      { // transfer is finished
        transfer.transferred += transfer.buffered;
        transfer.buffered = 0;

        clearCSR(UDP_CSR_TXCOMP);
        // Endpoint returns in Idle state
        epState = UDP_ENDPOINT_IDLE;
        endOfTransfer();
      }
      else
      {
        // Transfer remaining data
        transfer.transferred += END_POINT_MAX_SIZE;
        transfer.buffered -= END_POINT_MAX_SIZE;
        // Send next packet
        writePayload();
        setCSR(UDP_CSR_TXPKTRDY);
        clearCSR(UDP_CSR_TXCOMP);
      }
    }
    else
    {
      // Acknowledge transaction
      clearCSR(UDP_CSR_TXCOMP);
    }
  }

  // OUT packet received
  if (epStatus & UDP_RXDATA)
  {
    // Check that the endpoint is in Receiving state
    if (UDP_ENDPOINT_RECEIVING != epState)
    {
      // Check if an ACK has been received on a Control endpoint
      if ((epStatus & UDP_CSR_RXBYTECNT) == 0)
      {
        // Acknowledge the data and finish the current transfer
        // Notify USB peripheral device that data have been read in the FIFO's Bank 0.
        UDP->UDP_CSR[EP_NUMBER] &= ~UDP_CSR_RX_DATA_BK0;
        while ((UDP->UDP_CSR[EP_NUMBER] & UDP_CSR_RX_DATA_BK0) == UDP_CSR_RX_DATA_BK0);
        // Endpoint returns in Idle state
        epState = UDP_ENDPOINT_IDLE;
        endOfTransfer();
      }
      // Check if the data has been STALLed
      else if (epStatus & UDP_CSR_FORCESTALL)
      {
        // Discard STALLed data
        // Notify USB peripheral device that data have been read in the FIFO's Bank 0.
        UDP->UDP_CSR[EP_NUMBER] &= ~UDP_CSR_RX_DATA_BK0;
        while ((UDP->UDP_CSR[EP_NUMBER] & UDP_CSR_RX_DATA_BK0) == UDP_CSR_RX_DATA_BK0);
      }
    }
    // Endpoint is in Read state
    else
    {
      // Retrieve data and store it into the current transfer buffer
      uint16_t size = (uint16_t)(epStatus >> 16);

      readPayload(size);
      // Notify USB peripheral device that data have been read in the FIFO's Bank 0.
      UDP->UDP_CSR[EP_NUMBER] &= ~UDP_CSR_RX_DATA_BK0;
      while ((UDP->UDP_CSR[EP_NUMBER] & UDP_CSR_RX_DATA_BK0) == UDP_CSR_RX_DATA_BK0);
      // Check if the transfer is finished
      if ((transfer.remaining == 0) || (size < END_POINT_MAX_SIZE))
      {
        // Endpoint returns in Idle state
        epState = UDP_ENDPOINT_IDLE;
        endOfTransfer();
      }
    }
  }

  // SETUP packet received
  if (epStatus & UDP_CSR_RXSETUP)
  {
    // If a transfer was pending, complete it
    // Handles the case where during the status phase of a control write
    // transfer, the host receives the device ZLP and ack it, but the ack
    // is not received by the device
    if ((UDP_ENDPOINT_RECEIVING == epState) || (UDP_ENDPOINT_SENDING == epState))
    {
      // Endpoint returns in Idle state
      epState = UDP_ENDPOINT_IDLE;
      endOfTransfer();
    }
    // Copy packet
    ptr = (uint8_t *)&request;
    for (i = 0; i < (uint16_t)(epStatus >> 16); i++)
    {
      *ptr = (uint8_t)UDP->UDP_FDR[EP_NUMBER];
      ptr++;
    }
    // Set the DIR bit before clearing RXSETUP in Control IN sequence
    // Transfer direction is located in bit D7 of the bmRequestType field
    if (request.bmRequestType & 0x80)
      setCSR(UDP_CSR_DIR);
    clearCSR(UDP_CSR_RXSETUP);
    // Forward the request to the upper layer
    runtimeRequestHandler();
  }

  // STALL sent
  if (epStatus & UDP_CSR_STALLSENTISOERROR)
  {
    // If the endpoint is not halted, clear the STALL condition
    clearCSR(UDP_CSR_STALLSENTISOERROR);
    clearCSR(UDP_CSR_FORCESTALL);
  }
}

/**************************************************************************//**
\brief Startup initialization of the usb.
******************************************************************************/
void hwInitUsbDfu(void)
{
  // enables main clock on UDP for master clock domain
  PMC->PMC_PCER1 = (1ul << (ID_UDP - START_ID_NUMBER_FOR_PMC1));
  // enables 48 MHz clock on UDP phy for 12 MHz domain
  PMC->PMC_SCER = PMC_SCER_UDP;
  // Clear interrupt
  UDP->UDP_ICR = UDP_ICR_WAKEUP | UDP_ICR_RXSUSP | UDP_ICR_RXRSM;
  // enables UDP transceiver
  UDP->UDP_TXVC &= ~UDP_TXVC_TXVDIS;
  UDP->UDP_TXVC |= UDP_TXVC_PUON;
  // wait for usb bus reset
  while (!(UDP->UDP_ISR & UDP_ISR_ENDBUSRES));
  // Acknowledge end of bus reset interrupt and the others
  UDP->UDP_ICR = UDP_ISR_ENDBUSRES | UDP_ISR_WAKEUP | UDP_ISR_RXRSM | UDP_ISR_RXSUSP;
  // Reset Endpoint 0 Fifos
  UDP->UDP_RST_EP |= UDP_RST_EP_EP0;
  UDP->UDP_RST_EP &= ~UDP_RST_EP_EP0;
  // Configure endpoint 0
  setCSR(UDP_CSR_EPEDS | UDP_CSR_EPTYPE_CTRL | EP_OUT);
  // enumeration
  while (!(UDP->UDP_GLB_STAT & UDP_GLB_STAT_CONFG))
    epHandler();
}

/**************************************************************************//**
\brief Startup initialization of the usb.
******************************************************************************/
void hwUnInitUsbDfu(void)
{
  // clear global state
  UDP->UDP_GLB_STAT &= (~UDP_GLB_STAT_CONFG & ~UDP_GLB_STAT_FADDEN);
  // clear usb address
  UDP->UDP_FADDR = UDP_FADDR_FEN;
  // Reset Endpoint 0 Fifos
  UDP->UDP_RST_EP |= UDP_RST_EP_EP0;
  UDP->UDP_RST_EP &= ~UDP_RST_EP_EP0;
  // clear end point control and status
  UDP->UDP_CSR[EP_NUMBER] = 0ul;
  // switch off pull on
  UDP->UDP_TXVC &= ~UDP_TXVC_PUON;
  // disable usb tranceiver
  UDP->UDP_TXVC |= UDP_TXVC_TXVDIS;
  // disables 48 MHz clock on UDP phy for 12 MHz domain
  PMC->PMC_SCDR = PMC_SCER_UDP;
  // disables main clock on UDP for master clock domain
  PMC->PMC_PCDR1 = (1ul << (ID_UDP - START_ID_NUMBER_FOR_PMC1));
}

/**************************************************************************//**
\brief Receive byte on uart.

\param[out]
  p - pointer to byte memory;

\return
    true - there is received byte; \n
    false - there is not received byte;
******************************************************************************/
bool getByteUsbDfu(uint8_t *p)
{
  if (USB_HANDSHAKE_STATE == usbState)
  {
    *(uint32_t *)(p - 3) = HANDSHAKE_REQ;
    return true;
  }

  if (readFromPreBuffer(p))
    return true;
  else
    epHandler();

  return false;
}

/**************************************************************************//**
\brief Transmit byte to uart.

\param[in]
  len - number of bytes to transmit;
\param[in]
  p - pointer to byte memory;
******************************************************************************/
void setByteUsbDfu(uint16_t len, uint8_t *p)
{
  (void)len;
  (void)p;

  if (USB_HANDSHAKE_STATE == usbState)
    usbState = USB_DFU_STATE;
}

// eof usbSerializer.c
