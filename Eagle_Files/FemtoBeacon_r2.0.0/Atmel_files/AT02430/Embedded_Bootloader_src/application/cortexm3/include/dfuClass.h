/****************************************************************************//**
  \file dfuClass.h

  \brief Declaration of dfu class.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    05.09.11 A. Khromykh - Created
*******************************************************************************/

#ifndef _DFUCLASS_H
#define _DFUCLASS_H

/******************************************************************************
                   Includes section
******************************************************************************/

/******************************************************************************
                   Define(s) section
******************************************************************************/
enum
{
  DFU_DETACH,
  DFU_DNLOAD,
  DFU_UPLOAD,
  DFU_GETSTATUS,
  DFU_CLRSTATUS,
  DFU_GETSTATE,
  DFU_ABORT
};

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  errOK,
  errTARGET,
  errFILE,
  errWRITE,
  errERASE,
  errCHECK_ERASED,
  errPROG,
  errVERIFY,
  errADDRESS,
  errNOTDONE,
  errFIRMWARE,
  errVENDOR,
  errUSBR,
  errPOR,
  errUNKNOWN,
  errSTALLEDPKT
} DfuStatus_t;

typedef enum
{
  appIDLE,
  appDETACH,
  dfuIDLE,
  dfuDNLOAD_SYNC,
  dfuDNBUSY,
  dfuDNLOAD_IDLE,
  dfuMANIFEST_SYNC,
  dfuMANIFEST,
  dfuMANIFEST_WAIT_RESET,
  dfuUPLOAD_IDLE,
  dfuERROR
} DfuState_t;

BEGIN_PACK
typedef struct PACK
{
  uint8_t bStatus;
  uint8_t bwPollTimeout[3];
  uint8_t bState;
  uint8_t iString;
} DfuGetStatusResponse_t;

typedef struct PACK
{
  uint8_t bState;
} DfuGetStateResponse_t;
END_PACK

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief DFU request handler
******************************************************************************/
void dfuRequestHandler(void);

#endif /* _DFUCLASS_H */
