/****************************************************************************//**
  \file twiSerializer.c

  \brief Implementation of twi interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2/05/13 S. Kanchana - Created
*******************************************************************************/
#if (USE_TWIS == 1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include "twiSerializer.h"

/******************************************************************************
                   Global variables section
******************************************************************************/
/*!< Number of bytes sent*/
static uint8_t bytesSent;                            

/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
 \brief TWI read write interface

 \param none.
******************************************************************************/
uint8_t twiSlaveReadWrite(uint8_t *twiData)
{
	uint8_t result  = TWIS_STATUS_READY;
	
	switch (TWI_TWSR_STATUS_MASK) {
	case TWS_ST_SLA_ACK:        /* Own SLA+R has been received; ACK has been
		                     * returned */
		bytesSent   = 0; /* Set buffer pointer to first data
		                            * location */

	case TWS_ST_DATA_ACK:       /* Data byte in TWDR has been transmitted;
		                     * ACK has been received */
	    TWDR = twiData[bytesSent++];
		TWCR = 0xC5;
		break;

	case TWS_SR_SLA_DATA_ACK:   /* Previously addressed with own SLA+W; data
		                     * has been received; ACK has been returned */
		*twiData = TWDR;
         result= TWI_STATUS_RX_COMPLETE;  
	case TWS_SR_SLA_ACK:        /* Own SLA+W has been received ACK has been
		                     * returned */
		TWCR = 0xC5;
		break;		              

	case TWS_ST_DATA_NACK:      /* Data byte in TWDR has been transmitted;
		                     * NACK has been received. */
		                    /* I.e. this could be the end of the
		                     * transmission. */
        result = TWI_STATUS_TX_COMPLETE; 
		/* Break is removed intentionally and execution of follow up code 
		   is needed for correct operation.*/
               
	case TWS_SR_STOP_RESTART:   /* A STOP condition or repeated START
		                     * condition has been received while still
		                     * addressed as Slave */
		/* Enter not addressed mode and listen to address match */
		TWCR = 0xC5;
		break;

	}

   return result;
}

/**************************************************************************//**
 \brief TWI communication initialise

 \param none.
******************************************************************************/
void hwInitTwi_D(void)
{

    /* Set own TWI slave address */
  TWAR = (SLAVE_BUS_ADDR<<1) | TWI_GCE;                     
    
  /* enable TWI module as slave */
  TWCR = 0xC5;

}

/**************************************************************************//**
 \brief TWI communication Uninitialise

 \param none.
******************************************************************************/
void hwUnInitTwi_D(void)
{	
      TWCR = 0x00;
}

/**************************************************************************//**
\brief Receive byte on TWI.

\param[out]
  p - pointer to byte memory;

\return
    true - there is received byte; \n
    false - there is not received byte;
******************************************************************************/
bool getByteTwi(uint8_t *p)
{

     if( twiSlaveReadWrite(p) == TWI_STATUS_RX_COMPLETE)
      {
         return true;
      }
     return false;
}

/**************************************************************************//**
\brief Transmit byte to TWI.

\param[in]
  len - number of bytes to transmit;
\param[in]
  p - pointer to byte memory;
******************************************************************************/
void setByteTwi(uint16_t len, uint8_t *p)
{

    wdt_reset();
        
    while (  twiSlaveReadWrite(p) !=TWI_STATUS_TX_COMPLETE){
    }
}
#endif
