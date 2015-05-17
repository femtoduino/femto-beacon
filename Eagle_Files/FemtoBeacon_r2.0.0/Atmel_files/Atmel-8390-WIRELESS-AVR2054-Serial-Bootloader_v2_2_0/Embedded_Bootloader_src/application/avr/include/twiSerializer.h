/****************************************************************************//**
  \file twiSerializer.h

  \brief Declaration of TWI interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2/05/13 S. Kanchana - Created
*******************************************************************************/
#ifndef _TWISERIALIZER_H
#define _TWISERIALIZER_H

/******************************************************************************
                  Includes section
******************************************************************************/
#include <types.h>


/******************************************************************************
                   Define(s) section
******************************************************************************/
#ifndef INFINITE
  #define INFINITE true
#endif


#define SLAVE_BUS_ADDR       0x55        /* !< TWI slave bus address */
#define TWI_GCE              0x01        /* !< TWI slave general call enable */


/* Transaction status defines.*/
#define TWIS_STATUS_READY                0
#define TWI_STATUS_TX_COMPLETE           1
#define TWI_STATUS_RX_COMPLETE           2



/* TWSR status mask */
#define TWI_TWSR_STATUS_MASK    (TWSR & 0xF8)

/**************************************************************************//**
 \brief TWI Slave status codes
 ******************************************************************************/
enum {

	TWS_ST_SLA_ACK                 = 0xA8, /* ! Own SLA+R has
	                                                       * been received;
	                                                       * ACK has been
	                                                       * returned */
	TWS_ST_DATA_ACK                = 0xB8, /* ! Data byte in TWDR has
	                                               * been transmitted; ACK
	                                               * has been received */
	TWS_ST_DATA_NACK               = 0xC0, /* ! Data byte in TWDR has been
	                                       * transmitted; NOT ACK has been
	                                       * received */
	TWS_ST_DATA_ACK_LAST_BYTE      = 0xC8, /* ! Last data byte in TWDR has
	                                       * been transmitted (TWEA = “0”);
	                                       * ACK has been received */
	TWS_SR_SLA_ACK                 = 0x60, /* ! Own SLA+W has
	                                                   * been received ACK
	                                                   * has been returned */

	TWS_SR_SLA_DATA_ACK            = 0x80, /* ! Previously addressed
	                                               * with own SLA+W; data
	                                               * has been received; ACK
	                                               * has been returned */
	TWS_SR_SLA_DATA_NACK           = 0x88, /* ! Previously addressed with
	                                           * own SLA+W; data has been
	                                           * received; NOT ACK has been
	                                           * returned */
	TWS_SR_STOP_RESTART            = 0xA0 /* ! A STOP condition or repeated
	                                      * START condition has been
	                                      * received while still addressed
	                                      * as Slave */
};


/******************************************************************************
                   Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Startup initialization of the twi as slave 
******************************************************************************/
void hwInitTwi_D(void);

/**************************************************************************//**
 \brief TWI communication Uninitialise
******************************************************************************/
void hwUnInitTwi_D(void);

/**************************************************************************//**
\brief TWI read write interface
******************************************************************************/
uint8_t twiSlaveReadWrite(uint8_t *twiData);

/**************************************************************************//**
\brief Receive byte on TWI.
******************************************************************************/
bool getByteTwi(uint8_t *p);

/**************************************************************************//**
\brief Transmit byte to TWI.
******************************************************************************/
void setByteTwi(uint16_t len, uint8_t *p);

#endif  /* _TWISERIALIZER_H*/
