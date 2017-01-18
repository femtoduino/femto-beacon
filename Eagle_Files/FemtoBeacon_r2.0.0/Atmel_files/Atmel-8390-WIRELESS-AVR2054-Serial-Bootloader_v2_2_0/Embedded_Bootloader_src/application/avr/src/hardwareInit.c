/****************************************************************************//**
  \file hardwareInit.c

  \brief Hardware initialization routine.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15.10.09 A. Taradov - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <hardwareInit.h>
#if defined(EXT_MEMORY)
  #include <extMemReader.h>
#endif

/******************************************************************************
                   Define(s) section
******************************************************************************/
// mcu clock
#define INTERNAL_CLOCK 921600
// low frequency oscillator clock for 1 cycle measurement
#define EXTERNAL_TICKS 100
// mcu clocks number of one procedure measurement
#define CYCLE_LENGTH 7
// stability crystal oscillator frequency
#define REFERENCE_CLOCK 32768
// Etalon mcu clock number for 100 ticks of 32.768 kHz asynchronous timer
#define REFERENCE_COUNT (INTERNAL_CLOCK * EXTERNAL_TICKS) / (REFERENCE_CLOCK * CYCLE_LENGTH)
// up direction
#define UP_DIRECT      2
// down direction
#define DOWN_DIRECT    1

// defines fuse mask for RC oscillator
#define HAL_RC_OSCILLATOR_CLOCK 0x02
// mask for CKSEL bits
#define HAL_CKSEL_MASK          0x0F

/******************************************************************************
                   Global variables section
******************************************************************************/
// variable for default value
static uint8_t calibrConst = 0;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
Calculates number of cycles during EXTERNAL_TICKS period.

Returns:
  number of the cycles.
******************************************************************************/
uint16_t calibMeasurement(void);

/**************************************************************************//**
\brief Performs delay in microseconds

\param[in]
  delay - number of microseconds to be delay
******************************************************************************/
void __delay_us(uint8_t delay);

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Performs calibration of the internal RC generator.
Parameters:
  none.
Returns:
  none.
******************************************************************************/
static void calibrateInternalRc(void)
{
  uint16_t count;
  uint8_t cycles = 0x80;
  uint16_t counterGate = REFERENCE_COUNT;
  uint8_t direct = 0;

  calibrConst = OSCCAL;

  do
  {
    // make clock measurement
    count = calibMeasurement();
    if (count > REFERENCE_COUNT)
    {
	  OSCCAL--;
	  NOP;
      if ((counterGate < (count - REFERENCE_COUNT)) && (UP_DIRECT == direct))
      { // previous measurement was more exactly
        //OSCCAL--;
        //NOP;
        break;
      }
      counterGate = count - REFERENCE_COUNT;
      direct = DOWN_DIRECT;
    }

    if (count < REFERENCE_COUNT)
    {
	  OSCCAL++;
	  NOP;
      if ((counterGate < (REFERENCE_COUNT - count)) && (DOWN_DIRECT == direct))
      { // previous measurement was more exactly

        break;
      }
      counterGate = REFERENCE_COUNT - count;
      direct = UP_DIRECT;
    }

    if (REFERENCE_COUNT == count)
      break;

  } while (--cycles);
}

/******************************************************************************
Starts calibration after program starting or waking up from power down.
******************************************************************************/
static void startingCalibrate(void)
{
  // Start 32 kHz oscillator
  ASSR |= (1 << AS2);
  TCCR2B = 1 << CS20;

  for (uint16_t i = 0; i < 5000; i++)
  { /* wait for 1 second start up low frequency oscillator */
    __delay_us(200);  // 200 us
  }
  calibrateInternalRc();

  TCCR2B = 0; // Stops the timer
  TIFR2 |= (1 << OCF2A) | (1 << OCF2B) | (1 << TOV2); // Clears interrupt flags
}

/**************************************************************************//**
\brief Startup initialization (frequency, io, usart)
******************************************************************************/
void lowLevelInit(void)
{ // mcu works from 921600 Hz internal oscillator after reset

  // enable extended addressing if it is needded
  SET_EIND();

  // make wdt interval 8s
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR  = (1 << WDE) | (1 << WDP3) | (1 << WDP0);

  #if defined(EXT_MEMORY)
    // Point of enter to loader from external memory.
    extLoadDataFromExtMem();
  #endif

  // initialization clock source
  startingCalibrate(); // start calibration
}

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void lowLevelUnInit(void)
{
  // restore production value
  OSCCAL = calibrConst;
}

// eof hardwareInit.c
