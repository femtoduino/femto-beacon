/****************************************************************************//**
  \file freqSelector.h

  \brief Hardware initialization routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    01.11.11 N. Fomin - Created
*******************************************************************************/
#ifndef _FREQSELECTOR_H
#define _FREQSELECTOR_H

/******************************************************************************
                   Define(s) section
******************************************************************************/
#if CRYSTAL_FREQUENCY == CRYSTAL_12MHz
  // Master clock settings
  // Resulting freq 64.000 MHz (0.000 ppm error)
  #define PLLA_DIV 3
  #define PLLA_MUL 16
  #define PLLA_DIV2 0
  #define MCK_DIV PMC_MCKR_PRES_CLK
  #define ACTIVE_FWS 3
  // USB (48MHz) clock settings
  // Resulting freq 48.000 MHz (0.000 ppm error)
  #define PLLB_DIV 1
  #define PLLB_MUL 8
  #define PLLB_DIV2 0
  #define USB_DIV 2
#elif CRYSTAL_FREQUENCY == CRYSTAL_16MHz
  // Master clock settings
  // Resulting freq 64.000 MHz (0.000 ppm error)
  #define PLLA_DIV 1
  #define PLLA_MUL 4
  #define PLLA_DIV2 0
  #define MCK_DIV PMC_MCKR_PRES_CLK
  #define ACTIVE_FWS 3
  // USB (48MHz) clock settings
  // Resulting freq 48.000 MHz (0.000 ppm error)
  #define PLLB_DIV 1
  #define PLLB_MUL 6
  #define PLLB_DIV2 0
  #define USB_DIV 2
#elif CRYSTAL_FREQUENCY == CRYSTAL_18d432MHz
  // Master clock settings
  // Resulting freq 64.512 MHz (-7936.508 ppm error)
  #define PLLA_DIV 1
  #define PLLA_MUL 7
  #define PLLA_DIV2 0
  #define MCK_DIV PMC_MCKR_PRES_CLK_2
  #define ACTIVE_FWS 3
  // USB (48MHz) clock settings
  // Resulting freq 47.923 MHz (1602.564 ppm error)
  #define PLLB_DIV 5
  #define PLLB_MUL 26
  #define PLLB_DIV2 0
  #define USB_DIV 2
#else
  #error "Unknown frequency configuration"
#endif

#endif // _FREQSELECTOR_H

// eof freqSelector.h
