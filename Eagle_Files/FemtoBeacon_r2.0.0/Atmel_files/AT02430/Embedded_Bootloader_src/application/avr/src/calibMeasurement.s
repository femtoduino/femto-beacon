/**************************************************************************//**
  \file  calibMeasurement.s

  \brief Implementation of measurement of mcu clock.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      1/10/08 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

.set __external_ticks, 0x64	        ; 100

/*=============================================================
Calculates number of cycles during EXTERNAL_TICKS period.
Parameters:
  none
Returns:
  number of the cycles (r25:r24).
===============================================================*/
.global calibMeasurement
.type calibMeasurement,@function
calibMeasurement:
  ; local copy cnt
  ldi r24, 0    ; cnt = 0
  ldi r25, 0    ; cnt = 0

  ; TCNT2 = 0
  sts  0x00B2, r24

  ; while (ASSR & 0x1B);
  __l0:
  lds  r21, 0x00B6
  andi r21, 0x1B
  brne  __l0

  ; measurement
  __l1:
  adiw r24, 0x01               ; cnt ++ (2 cycle)
  lds  r21, 0x00B2             ; read TCNT2
  subi r21, __external_ticks   ; TCNT2 - __external_ticks
  brmi __l1                    ; if TCNT2 > __external_ticks then exit

  ret

/*=============================================================
 Delay in microseconds.
 Parameters:
   us - delay time in microseconds
===============================================================*/
.global __delay_us
.type __delay_us,@function
__delay_us:
  lsr r24
  lsr r24
__w0:
  dec r24
  tst r24
  brne __w0
  ret

; eof calibMeasurement.s
