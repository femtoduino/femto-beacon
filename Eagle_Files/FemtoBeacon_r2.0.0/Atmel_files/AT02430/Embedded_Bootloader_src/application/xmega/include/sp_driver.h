/**************************************************************************//**
  \file  sp_driver.h

  \brief Declaration of self programming functions.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      28/07/10 A. Khromykh - Created
*******************************************************************************/
#ifndef SP_DRIVER_H
#define SP_DRIVER_H

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define FLASH_PAGE_SIZE 512

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief This routine writes the word from data into the Flash page buffer at
 address.

\param[in]
  address - Position in inside the flash page buffer.
\param[in]
  data   -  Value to be put into the buffer.
******************************************************************************/
void SP_LoadFlashWord(uint16_t address, uint16_t data);

/**************************************************************************//**
\brief This routine erases first and then writes the page buffer to the
 Flash page at address in the application section. The address
 can point anywhere inside the page.

\param[in]
  address - Byte address for flash page.
******************************************************************************/
void SP_EraseWriteApplicationPage(uint32_t address);

#endif /* SP_DRIVER_H */
