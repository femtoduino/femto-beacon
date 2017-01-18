/****************************************************************************//**
  \file timerInterval.h

  \brief Implementation of timer interval interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    28.02.11 A. Khromykh - Created
*******************************************************************************/

#ifndef _TIMERINTERVAL_H
#define _TIMERINTERVAL_H

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define COUNTER_200MS   0x186A

/******************************************************************************
                     Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief Timer initialization (interval 200ms).
******************************************************************************/
static inline void timerIntervalInit(void)
{
  TCC1.PER = COUNTER_200MS;                  // top value (about 200 ms)
  TCC1.CTRLB = TC_WGMODE_NORMAL_gc;   // normal mode
}

/**************************************************************************//**
\brief Clear timer settings.
******************************************************************************/
static inline void timerIntervalUnInit(void)
{
  // clear timer
  TCC1.CNT = 0;
  TCC1.INTFLAGS = 0;
  TCC1.CTRLA = 0;
  TCC1.PER = 0;
}

/**************************************************************************//**
\brief Start timer interval.
******************************************************************************/
static inline void timerIntervalStart(void)
{
  TCC1.CNT = 0;
  TCC1.INTFLAGS = TC1_OVFIF_bm;
  TCC1.CTRLA = TC_CLKSEL_DIV64_gc;     // prescaler clk / 64
}

/**************************************************************************//**
\brief Stop timer.
******************************************************************************/
static inline void timerIntervalStop(void)
{
  TCC1.CTRLB = 0; // stop timer
}

/**************************************************************************//**
\brief Return expired state.

\return
  true - timer expired; \n
  false - otherwise.
******************************************************************************/
static inline bool timerReadIntervalState(void)
{
  return TCC1.INTFLAGS & TC1_OVFIF_bm;
}

#endif /* _TIMERINTERVAL_H */
