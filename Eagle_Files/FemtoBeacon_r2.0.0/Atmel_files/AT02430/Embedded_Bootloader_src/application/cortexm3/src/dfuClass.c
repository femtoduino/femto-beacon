/****************************************************************************//**
  \file dfuClass.c

  \brief Implementation of dfu class.

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
#include <preparser.h>
#include <flashLoader.h>
#include <eepromLoader.h>
#include <abstractSerializer.h>
#include <srecParser.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define DFU_POLL_TIMEOUT   10

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void getFirmwareData(uint8_t *ptr, uint16_t transferred, uint16_t remaining);
static void finalizeUpdate(uint8_t *ptr, uint16_t transferred, uint16_t remaining);

/******************************************************************************
                   External global variables section
******************************************************************************/
extern UsbResponse_t response;
extern UsbRequest_t request;
extern bool srecFinished;
extern uint32_t appEntryPoint;

/******************************************************************************
                   Global variables section
******************************************************************************/
static DfuStatus_t dfuStatus = errOK;
static DfuState_t  dfuState = dfuIDLE;

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
DFU request handler
******************************************************************************/
void dfuRequestHandler(void)
{
  DfuGetStatusResponse_t *getStatusResponse;
  DfuGetStateResponse_t  *getStateResponse;

  // Check request code
  switch (request.bRequest)
  {
    case DFU_DNLOAD:
      if (request.wLength)
      {
        dfuState = dfuDNLOAD_IDLE;
        rxUsbData(request.bData, (uint8_t)request.wLength, (TransferCallback_t)getFirmwareData, NULL);
      }
      else
      {
        dfuState = dfuIDLE;
        txZLP();
      }
      break;
    case DFU_GETSTATUS:
      getStatusResponse = &response.getStatusResponse;
      getStatusResponse->bStatus = dfuStatus;
      getStatusResponse->bwPollTimeout[0] = DFU_POLL_TIMEOUT;
      getStatusResponse->bState = dfuState;
      getStatusResponse->iString = 0;
      if (srecFinished && (dfuIDLE == dfuState)) // dfu proccess was finished (srec was finished and zero firmware packet was received)
        txUsbData((void *)getStatusResponse, sizeof(DfuGetStatusResponse_t), (TransferCallback_t)finalizeUpdate, 0);
      else
        txUsbData((void *)getStatusResponse, sizeof(DfuGetStatusResponse_t), 0, 0);
      break;
    case DFU_CLRSTATUS:
      dfuStatus = errOK;
      dfuState = dfuIDLE;
      break;
    case DFU_GETSTATE:
      getStateResponse = &response.getStateResponse;
      getStateResponse->bState = dfuState;
      txUsbData((void *)getStateResponse, sizeof(DfuGetStateResponse_t), 0, 0);
      break;
    case DFU_ABORT:
      dfuStatus = errOK;
      dfuState = dfuIDLE;
      break;
    default:
      stallData();
      break;
  }
}

/******************************************************************************
Read piece of firmware.
******************************************************************************/
static void getFirmwareData(uint8_t *ptr, uint16_t transferred, uint16_t remaining)
{
  (void)ptr;
  (void)transferred;
  (void)remaining;
  
  // Acknowledge the request
  txZLP();
  convertStreamToNumbers(request.bData, (uint8_t)request.wLength);
}

/******************************************************************************
Finalize image update and call application software.
******************************************************************************/
static void finalizeUpdate(uint8_t *ptr, uint16_t transferred, uint16_t remaining)
{
  (void)ptr;
  (void)transferred;
  (void)remaining;

  flashFlushPage();
  eepromFlushPage();
  flashExchangeEntryPoints();
  bootUnInitSerializer();
#ifdef  __IAR_SYSTEMS_ICC__
  __set_SP(*(uint32_t *)FLASH_START_ADDRESS);
#elif __GNUC__
  __asm__ volatile ("mov r13, %0; \n\t"
       :
       : "r" (*(uint32_t *)FLASH_START_ADDRESS) /* input */
       );
#endif
  ((void(*)(void))appEntryPoint)();
}

// eof dfuClass
