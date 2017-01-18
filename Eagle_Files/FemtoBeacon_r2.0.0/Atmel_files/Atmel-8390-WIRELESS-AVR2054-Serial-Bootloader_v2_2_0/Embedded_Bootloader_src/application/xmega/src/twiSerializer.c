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
    6/05/13 S. Kanchana - Created
*******************************************************************************/
#if (USE_TWIS == 1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include "twiSerializer.h"


/******************************************************************************
                   Global variables section
******************************************************************************/

static TWI_Slave_t twiSlave;

/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
 \brief TWI transaction finished function.
 Prepares module for new transaction.
 
 \param twi    The TWI_Slave_t struct instance.
 \param result The result of the transaction.
******************************************************************************/
static void twiSlaveTransactionFinished(TWI_Slave_t *twi, uint8_t result)
{
	twi->result = result;
	twi->status = TWIS_STATUS_READY;
}

/**************************************************************************//**
 \brief TWI address match interrupt handler.
 Prepares TWI module for transaction when an address match occurs.
 
 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void twiSlaveAddressMatchHandler(TWI_Slave_t *twi)
{
	/* If application signalling need to abort (error occured). */
	if (twi->abort) {
		twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
		twiSlaveTransactionFinished(twi, TWIS_RESULT_ABORTED);
		twi->abort = false;
	} else {
		twi->status = TWIS_STATUS_BUSY;
		twi->result = TWIS_RESULT_UNKNOWN;

		/* Disable stop interrupt. */
		uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
		twi->interface->SLAVE.CTRLA = currentCtrlA & ~TWI_SLAVE_PIEN_bm;

		twi->bytesSent = 0;

		/* Send ACK, wait for data interrupt. */
		twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
	}
}


/**************************************************************************//**
 \brief TWI stop condition interrupt handler.

 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void twiSlaveStopHandler(TWI_Slave_t *twi)
{
	/* Disable stop interrupt. */
	uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
	twi->interface->SLAVE.CTRLA = currentCtrlA & ~TWI_SLAVE_PIEN_bm;

	/* Clear APIF, according to flowchart don't ACK or NACK */
	uint8_t currentStatus = twi->interface->SLAVE.STATUS;
	twi->interface->SLAVE.STATUS = currentStatus | TWI_SLAVE_APIF_bm;

}

/**************************************************************************//**
 \brief TWI slave read interrupt handler.
 Handles TWI slave read transactions and responses.
 
 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void twiSlaveReadHandler(TWI_Slave_t *twi)
{
	/* Enable stop interrupt. */
	uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
	twi->interface->SLAVE.CTRLA = currentCtrlA | TWI_SLAVE_PIEN_bm;

		/* Fetch data */
		uint8_t data = twi->interface->SLAVE.DATA;
		twi->receivedData = data;

		/* If application signalling need to abort (error occured),
		 * complete transaction and wait for next START. Otherwise
		 * send ACK and wait for data interrupt.
		 */
		if (twi->abort) {
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
			twiSlaveTransactionFinished(twi, TWIS_RESULT_ABORTED);
			twi->abort = false;
		} else {                
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
                        twiSlaveTransactionFinished(twi, TWIS_RESULT_OK);
		}

}


/**************************************************************************//**
 \brief TWI slave write interrupt handler.

 Handles TWI slave write transactions and responses.

 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void twiSlaveWriteHandler(TWI_Slave_t *twi)
{
	/* If NACK, slave write transaction finished. */
	if ((twi->bytesSent > 0) && (twi->interface->SLAVE.STATUS &
	                             TWI_SLAVE_RXACK_bm)) {

		twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;             
		twiSlaveTransactionFinished(twi, TWIS_RESULT_OK);
	}
	/* If ACK, master expects more data. */
	else {
		if (twi->bytesSent < TWIS_SEND_BUFFER_SIZE) {
			uint8_t data = twi->sendData[twi->bytesSent];
			twi->interface->SLAVE.DATA = data;
			twi->bytesSent++;

			/* Send data, wait for data interrupt. */
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;                      
		}
		/* If buffer overflow. */
		else {
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
			twiSlaveTransactionFinished(twi, TWIS_RESULT_BUFFER_OVERFLOW);
		}
	}
}

/**************************************************************************//**
 \brief TWI data handler.

 Calls the appropriate slave read or write handler.

 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void twiSlaveDataHandler(TWI_Slave_t *twi)
{
	if (twi->interface->SLAVE.STATUS & TWI_SLAVE_DIR_bm) {
		twiSlaveWriteHandler(twi);
	} else {
		twiSlaveReadHandler(twi);
	}
}

/**************************************************************************//**
 \brief Initalizes TWI slave driver structure.

 Initialize the instance of the TWI Slave and set the appropriate values.

 \param twi                  The TWI_Slave_t struct instance.
 \param module               Pointer to the TWI module.
 \param processDataFunction  Pointer to the function that handles incoming data.
******************************************************************************/
static void twiSlaveInitializeDriver(TWI_Slave_t *twi,
                               TWI_t *module)
{
	twi->interface = module;
	twi->bytesSent = 0;
	twi->status = TWIS_STATUS_READY;
	twi->result = TWIS_RESULT_UNKNOWN;
	twi->abort = false;
}

/**************************************************************************//**
 \brief Initialize the TWI module.

 \param twi        The TWI_Slave_t struct instance.
 \param address    Slave address for this module.
 ******************************************************************************/
static void twiSlaveInitializeModule(TWI_Slave_t *twi,
                               uint8_t address)
{
	twi->interface->SLAVE.CTRLA = TWI_SLAVE_ENABLE_bm;
	twi->interface->SLAVE.ADDR = (address<<1);
}

/**************************************************************************//**
 \brief Uninitialize the TWI module.

 \param twi        The TWI_Slave_t struct instance.
 ******************************************************************************/
static void twiSlaveUnInitializeModule(TWI_Slave_t *twi)
{
	twi->interface->SLAVE.CTRLA = 0x00;
}

/**************************************************************************//**
 \brief Common TWI slave comm routine.

 Handles all TWI transactions and responses to address match, data reception,
 data transmission, bus error and data collision.

 \param twi The TWI_Slave_t struct instance.
 ******************************************************************************/
static void twiSlaveReadWrite(TWI_Slave_t *twi)
{
	uint8_t currentStatus = twi->interface->SLAVE.STATUS;

	/* If bus error. */
	if (currentStatus & TWI_SLAVE_BUSERR_bm) {
		twi->bytesSent = 0;
		twi->result = TWIS_RESULT_BUS_ERROR;
		twi->status = TWIS_STATUS_READY;
	}

	/* If transmit collision. */
	else if (currentStatus & TWI_SLAVE_COLL_bm) {
		twi->bytesSent = 0;
		twi->result = TWIS_RESULT_TRANSMIT_COLLISION;
		twi->status = TWIS_STATUS_READY;
	}

	/* If address match. */
	else if ((currentStatus & TWI_SLAVE_APIF_bm) &&
	        (currentStatus & TWI_SLAVE_AP_bm)) {

		twiSlaveAddressMatchHandler(twi);
	}

	/* If stop (only enabled through slave read transaction). */
	else if (currentStatus & TWI_SLAVE_APIF_bm) {
		twiSlaveStopHandler(twi);
	}

	/* If data read/write. */
	else if (currentStatus & TWI_SLAVE_DIF_bm) {
		twiSlaveDataHandler(twi);
	}

	/* If unexpected state. */
	else {
	}
}

#if (USE_TWIS_C == 1) 
/**************************************************************************//**
 \brief TWI module initialisation
 ******************************************************************************/
void hwInitTwi_C(void)
{
        
	twiSlaveInitializeDriver(&twiSlave, &TWIC);
	twiSlaveInitializeModule(&twiSlave, SLAVE_BUS_ADDR);
}

/**************************************************************************//**
 \brief TWI module Uninitialisation
 ******************************************************************************/
void hwUnInitTwi_C(void)
{	
  	twiSlaveUnInitializeModule(&twiSlave);
}
#endif

#if (USE_TWIS_D == 1) 
/**************************************************************************//**
 \brief TWI module initialisation
 ******************************************************************************/
void hwInitTwi_D(void)
{
        
	twiSlaveInitializeDriver(&twiSlave, &TWID);
	twiSlaveInitializeModule(&twiSlave, SLAVE_BUS_ADDR);
}

/**************************************************************************//**
 \brief TWI module Uninitialisation
 ******************************************************************************/
void hwUnInitTwi_D(void)
{	
  	twiSlaveUnInitializeModule(&twiSlave);
}
#endif

#if (USE_TWIS_E == 1) 
/**************************************************************************//**
 \brief TWI module initialisation
 ******************************************************************************/
void hwInitTwi_E(void)
{
        
	twiSlaveInitializeDriver(&twiSlave, &TWIE);
	twiSlaveInitializeModule(&twiSlave, SLAVE_BUS_ADDR);
}

/**************************************************************************//**
 \brief TWI module Uninitialisation
 ******************************************************************************/
void hwUnInitTwi_E(void)
{	
  	twiSlaveUnInitializeModule(&twiSlave);
}
#endif

#if (USE_TWIS_F == 1) 
/**************************************************************************//**
 \brief TWI module initialisation
 ******************************************************************************/
void hwInitTwi_F(void)
{
        
	twiSlaveInitializeDriver(&twiSlave, &TWIF);
	twiSlaveInitializeModule(&twiSlave, SLAVE_BUS_ADDR);
}

/**************************************************************************//**
 \brief TWI module Uninitialisation
 ******************************************************************************/
void hwUnInitTwi_F(void)
{	
  	twiSlaveUnInitializeModule(&twiSlave);
}
#endif

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
     twiSlaveReadWrite(&twiSlave);
     if(twiSlave.result == TWIS_RESULT_OK)
      {
         *p = twiSlave.receivedData;
         twiSlave.result  = TWIS_RESULT_UNKNOWN;
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
    
    twiSlave.sendData= p; // prepare transmission
        
    while (twiSlave.result !=TWIS_RESULT_OK){
      
      twiSlaveReadWrite(&twiSlave);
    }
    
    twiSlave.result  = TWIS_RESULT_UNKNOWN;
}
#endif
