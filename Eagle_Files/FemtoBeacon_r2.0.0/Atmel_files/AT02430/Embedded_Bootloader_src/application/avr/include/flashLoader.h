/****************************************************************************//**
  \file flashLoader.h

  \brief Declaration api of flash loader.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2/04/09 A. Khromykh - Created
*******************************************************************************/
#ifndef _FLASHLOADER_H
#define _FLASHLOADER_H

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define PAGE_NUMBER_SHIFT              8
#define PAGE_POSITION_OFFSET_MASK   0xFF

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Load received date to the flash.
******************************************************************************/
void flashPacketToPage(void);

/**************************************************************************//**
\brief Flush unloaded data before exit from bootloader.
******************************************************************************/
void flashFlushPage(void);

/******************************************************************************
                     Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief This routine wait for the SPM to finish. \n
 Note that this routine is blocking, and will halt any execution until the SPM
 is finished.
******************************************************************************/
static inline void flashWaitForSPM(void)
{
  while (SPMCSR & (1 << SPMEN));
}

#endif // _FLASHLOADER_H
