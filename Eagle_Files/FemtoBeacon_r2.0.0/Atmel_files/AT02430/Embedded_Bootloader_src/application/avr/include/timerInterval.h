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
#define COUNTER_200MS   0xB4

/******************************************************************************
                     Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief Timer initialization (interval 200ms).
******************************************************************************/
static inline void timerIntervalInit(void)
{
  TCCR0A = 1 << WGM01;   // CTC mode
  OCR0A = COUNTER_200MS; // top value (about 200 ms)
}

/**************************************************************************//**
\brief Clear timer settings.
******************************************************************************/
static inline void timerIntervalUnInit(void)
{
  // clear timer
  TCCR0A = 0;
  OCR0A = 0;
  TCNT0 = 0;
  TIFR0 = 0;
}

/**************************************************************************//**
\brief Start timer interval.
******************************************************************************/
static inline void timerIntervalStart(void)
{
  TCNT0 = 0;
  TIFR0 = 1 << OCF0A; // clear compare a match flag
  TCCR0B = (1 << CS02) | (1 << CS00); // start timer with clk/1024
}

/**************************************************************************//**
\brief Stop timer.
******************************************************************************/
static inline void timerIntervalStop(void)
{
  TCCR0B = 0; // stop timer
}

/**************************************************************************//**
\brief Return expired state.

\return
  true - timer expired; \n
  false - otherwise.
******************************************************************************/
static inline bool timerReadIntervalState(void)
{
  return TIFR0 & (1 << OCF0A);
}

#endif /* _TIMERINTERVAL_H */
