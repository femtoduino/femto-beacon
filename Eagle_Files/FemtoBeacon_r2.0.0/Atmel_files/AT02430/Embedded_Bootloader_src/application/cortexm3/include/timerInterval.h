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
    06.09.11 A. Khromykh - Created
*******************************************************************************/

#ifndef _TIMERINTERVAL_H
#define _TIMERINTERVAL_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <AT91SAM3S4.h>
#include <core_cm3.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// (TC) UP mode with automatic trigger on RC Compare
#define TC_CMR0_WAVESEL_UP_AUTO  (0x2 << 13)
#define USED_TIMER_CHANNEL   0
#define TIMER_CLOCK_SOURCE   TC_CM0_TCCLKS_TIMER_DIV5_CLOCK
// clock source is slow clock 32kHz
#define COUNTER_200MS   0x2000

/******************************************************************************
                     Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief Timer initialization (interval 200ms).
******************************************************************************/
static inline void timerIntervalInit(void)
{
  /* Enable peripheral clock for the TC0 */
  PMC->PMC_PCER0 = (1ul << ID_TC0);
  /* Set the Mode of the Timer Counter */
  /* Capture mode. Source TIMER_CLOCK5. RC Compare resets the counter and starts the counter clock.*/
  TC0->TC_CHANNEL[USED_TIMER_CHANNEL].TC_CMR = TIMER_CLOCK_SOURCE | TC_CMR0_CPCTRG;
  TC0->TC_CHANNEL[USED_TIMER_CHANNEL].TC_RC = COUNTER_200MS;
}

/**************************************************************************//**
\brief Clear timer settings.
******************************************************************************/
static inline void timerIntervalUnInit(void)
{
  TC0->TC_CHANNEL[USED_TIMER_CHANNEL].TC_RC = 0;
  TC0->TC_CHANNEL[USED_TIMER_CHANNEL].TC_CMR = 0ul;
  /* Disable peripheral clock for the TC0 */
  PMC->PMC_PCDR0 = (1ul << ID_TC0);
}

/**************************************************************************//**
\brief Start timer interval.
******************************************************************************/
static inline void timerIntervalStart(void)
{
  /* Clock is started */
  TC0->TC_CHANNEL[USED_TIMER_CHANNEL].TC_CCR = TC_CCR0_CLKEN;
  /* Counter is reseted and the clock is started */
  TC0->TC_CHANNEL[USED_TIMER_CHANNEL].TC_CCR = TC_CCR0_SWTRG;
}

/**************************************************************************//**
\brief Stop timer.
******************************************************************************/
static inline void timerIntervalStop(void)
{
  /* Disable the clock */
  TC0->TC_CHANNEL[USED_TIMER_CHANNEL].TC_CCR = TC_CCR0_CLKDIS;
}

/**************************************************************************//**
\brief Return expired state.

\return
  true - timer expired; \n
  false - otherwise.
******************************************************************************/
static inline bool timerReadIntervalState(void)
{
  return TC_SR0_CPCS & TC0->TC_CHANNEL[USED_TIMER_CHANNEL].TC_SR;
}

#endif /* _TIMERINTERVAL_H */
