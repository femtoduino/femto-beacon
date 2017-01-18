/****************************************************************************//**
  \file twiInterface.c

  \brief Implementation of twi interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15/02/13 S. Kanchana - Created
*******************************************************************************/
#if (USE_TWI == 1) || (USE_TWIC == 1) || (USE_TWID == 1) || (USE_TWIE == 1) || (USE_TWIF ==1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include "twiSerializer.h"
#include <sp_driver.h>
#include <flashLoader.h>

/******************************************************************************
                   Global variables section
******************************************************************************/
extern bool Write_Pending;

static TWI_Slave_t twiSlave;
TWI_State_t TWI_State;

/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
 \brief TWI transaction finished function.
 Prepares module for new transaction.
 
 \param twi    The TWI_Slave_t struct instance.
 \param result The result of the transaction.
******************************************************************************/
static void TWI_SlaveTransactionFinished(TWI_Slave_t *twi, uint8_t result)
{
	twi->result = result;
	twi->status = TWIS_STATUS_READY;
}

/**************************************************************************//**
 \brief TWI address match interrupt handler.
 Prepares TWI module for transaction when an address match occurs.
 
 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void TWI_SlaveAddressMatchHandler(TWI_Slave_t *twi)
{
	/* If application signalling need to abort (error occured). */
	if (twi->abort) {
		twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
		TWI_SlaveTransactionFinished(twi, TWIS_RESULT_ABORTED);
		twi->abort = false;
	} else {
		twi->status = TWIS_STATUS_BUSY;
		twi->result = TWIS_RESULT_UNKNOWN;

		/* Disable stop interrupt. */
		uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
		twi->interface->SLAVE.CTRLA = currentCtrlA & ~TWI_SLAVE_PIEN_bm;

		twi->bytesReceived = 0;
		twi->bytesSent = 0;

		/* Send ACK, wait for data interrupt. */
		twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
	}
}

/**************************************************************************//**
 \brief Clear the twi data buffer

 \param none.
******************************************************************************/
static void flush_Twibuffer(void)
{
    for(uint16_t i = 0;i < TWIS_RECEIVE_BUFFER_SIZE; i++)
	   twiSlave.receivedData[i] = 0;
}

/**************************************************************************//**
 \brief TWI stop condition interrupt handler.

 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void TWI_SlaveStopHandler(TWI_Slave_t *twi)
{
	/* Disable stop interrupt. */
	uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
	twi->interface->SLAVE.CTRLA = currentCtrlA & ~TWI_SLAVE_PIEN_bm;

	/* Clear APIF, according to flowchart don't ACK or NACK */
	uint8_t currentStatus = twi->interface->SLAVE.STATUS;
	twi->interface->SLAVE.STATUS = currentStatus | TWI_SLAVE_APIF_bm;
	
    // Process the received data.
    TwiDataParser(twiSlave);

	TWI_SlaveTransactionFinished(twi, TWIS_RESULT_OK);

    // Flush the buffers used.
    flush_Twibuffer();
}

/**************************************************************************//**
 \brief TWI slave read interrupt handler.
 Handles TWI slave read transactions and responses.
 
 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void TWI_SlaveReadHandler(TWI_Slave_t *twi)
{
	/* Enable stop interrupt. */
	uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
	twi->interface->SLAVE.CTRLA = currentCtrlA | TWI_SLAVE_PIEN_bm;

	/* If free space in buffer. */
	if (twi->bytesReceived < TWIS_RECEIVE_BUFFER_SIZE) {
		/* Fetch data */
		uint8_t data = twi->interface->SLAVE.DATA;
		twi->receivedData[twi->bytesReceived] = data;

		twi->bytesReceived++;

		/* If application signalling need to abort (error occured),
		 * complete transaction and wait for next START. Otherwise
		 * send ACK and wait for data interrupt.
		 */
		if (twi->abort) {
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
			TWI_SlaveTransactionFinished(twi, TWIS_RESULT_ABORTED);
			twi->abort = false;
		} else {
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
		}
	}
	/* If buffer overflow, send NACK and wait for next START. Set
	 * result buffer overflow.
	 */
	else {
		twi->interface->SLAVE.CTRLB = TWI_SLAVE_ACKACT_bm |
		                              TWI_SLAVE_CMD_COMPTRANS_gc;
		TWI_SlaveTransactionFinished(twi, TWIS_RESULT_BUFFER_OVERFLOW);
	}
}


/**************************************************************************//**
 \brief TWI slave write interrupt handler.

 Handles TWI slave write transactions and responses.

 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void TWI_SlaveWriteHandler(TWI_Slave_t *twi)
{
	/* If NACK, slave write transaction finished. */
	if ((twi->bytesSent > 0) && (twi->interface->SLAVE.STATUS &
	                             TWI_SLAVE_RXACK_bm)) {

		twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
		TWI_SlaveTransactionFinished(twi, TWIS_RESULT_OK);
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
			TWI_SlaveTransactionFinished(twi, TWIS_RESULT_BUFFER_OVERFLOW);
		}
	}
}

/**************************************************************************//**
 \brief TWI data interrupt handler.

 Calls the appropriate slave read or write handler.

 \param twi The TWI_Slave_t struct instance.
******************************************************************************/
static void TWI_SlaveDataHandler(TWI_Slave_t *twi)
{
	if (twi->interface->SLAVE.STATUS & TWI_SLAVE_DIR_bm) {
		TWI_SlaveWriteHandler(twi);
	} else {
		TWI_SlaveReadHandler(twi);
	}
}

/**************************************************************************//**
 \brief Initalizes TWI slave driver structure.

 Initialize the instance of the TWI Slave and set the appropriate values.

 \param twi                  The TWI_Slave_t struct instance.
 \param module               Pointer to the TWI module.
 \param processDataFunction  Pointer to the function that handles incoming data.
******************************************************************************/
static void TWI_SlaveInitializeDriver(TWI_Slave_t *twi,
                               TWI_t *module)
{
	twi->interface = module;
	twi->bytesReceived = 0;
	twi->bytesSent = 0;
	twi->status = TWIS_STATUS_READY;
	twi->result = TWIS_RESULT_UNKNOWN;
	twi->abort = false;
	TWI_State = TWI_Start;
	Write_Pending = false;
}

/**************************************************************************//**
 \brief Initialize the TWI module.

 Enables interrupts on address recognition and data available.
 Remember to enable interrupts globally from the main application.

 \param twi        The TWI_Slave_t struct instance.
 \param address    Slave address for this module.
 \param intLevel   Interrupt level for the TWI slave interrupt handler.
 ******************************************************************************/
static void TWI_SlaveInitializeModule(TWI_Slave_t *twi,
                               uint8_t address,
                               TWI_SLAVE_INTLVL_t intLevel)
{
	twi->interface->SLAVE.CTRLA = intLevel |
	                              TWI_SLAVE_DIEN_bm |
	                              TWI_SLAVE_APIEN_bm |
	                              TWI_SLAVE_ENABLE_bm;
	twi->interface->SLAVE.ADDR = (address<<1);
}

/**************************************************************************//**
 \brief Common TWI slave interrupt service routine.

 Handles all TWI transactions and responses to address match, data reception,
 data transmission, bus error and data collision.

 \param twi The TWI_Slave_t struct instance.
 ******************************************************************************/
static void TWI_SlaveInterruptHandler(TWI_Slave_t *twi)
{
	uint8_t currentStatus = twi->interface->SLAVE.STATUS;

	/* If bus error. */
	if (currentStatus & TWI_SLAVE_BUSERR_bm) {
		twi->bytesReceived = 0;
		twi->bytesSent = 0;
		twi->result = TWIS_RESULT_BUS_ERROR;
		twi->status = TWIS_STATUS_READY;
	}

	/* If transmit collision. */
	else if (currentStatus & TWI_SLAVE_COLL_bm) {
		twi->bytesReceived = 0;
		twi->bytesSent = 0;
		twi->result = TWIS_RESULT_TRANSMIT_COLLISION;
		twi->status = TWIS_STATUS_READY;
	}

	/* If address match. */
	else if ((currentStatus & TWI_SLAVE_APIF_bm) &&
	        (currentStatus & TWI_SLAVE_AP_bm)) {

		TWI_SlaveAddressMatchHandler(twi);
	}

	/* If stop (only enabled through slave read transaction). */
	else if (currentStatus & TWI_SLAVE_APIF_bm) {
		TWI_SlaveStopHandler(twi);
	}

	/* If data interrupt. */
	else if (currentStatus & TWI_SLAVE_DIF_bm) {
		TWI_SlaveDataHandler(twi);
	}

	/* If unexpected state. */
	else {
		TWI_SlaveTransactionFinished(twi, TWIS_RESULT_FAIL);
	}
}

/**************************************************************************//**
 \brief TWI read write interface

 \param none.
******************************************************************************/
void TwiSlave_ReadWrite(void)
{
   /* Intentionally left empty as the twi interface is interrupt based */
}

/**************************************************************************//**
 \brief TWI module initialisation

 \param twi The TWI_Slave_t struct instance.
 ******************************************************************************/
void hwInitTwi(void)
{
  	uint8_t temp = PMIC.CTRL | PMIC_IVSEL_bm;
	CCP = CCP_IOREG_gc;
	PMIC.CTRL = temp;
        
	TWI_SlaveInitializeDriver(&twiSlave, TWI_MODULE);
	TWI_SlaveInitializeModule(&twiSlave, SLAVE_BUS_ADDR, TWI_SLAVE_INTLVL_LO_gc);

	// Enable LO interrupt level.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
}

/**************************************************************************//**
 \brief IO Port initialisation for selecting between Boot Section and App Sec

 \param none.
******************************************************************************/
void ButtonLedInit(void)
{
   /* Configure PortB.3 pin as input pin for SW PortB.2 pin as output for LED*/
   PORTB_DIR = 0x04;
   
   PORTB_OUT = 0x04;
  
   /* Enable the Pullup */
   PORTB_PIN3CTRL = 0x18;
   
  
}

/**************************************************************************//**
 \brief IO Port uninitialisation for selecting between Boot Section and App Sec

 \param none.
******************************************************************************/
void ButtonLedUnInit(void)
{

   PORTB_DIR = 0x00;
  
   PORTB_PIN3CTRL = 0x00;
  
   PORTB_OUT = 0x00;
}

/**************************************************************************//**
 \brief Moving the interrupt vector from Boot Section  to App Section
 
 \param none.
******************************************************************************/
void moveIntVectortoApp(void)
{
   uint8_t temp = PMIC.CTRL & ~PMIC_IVSEL_bm;
   CCP = CCP_IOREG_gc;
   PMIC.CTRL = temp;
}

/**************************************************************************//**
 \brief Moving the interrupt vector from App Section  to Boot Section
 
 \param none.
******************************************************************************/
void moveIntVectortoBoot(void)
{
  // Intensionally left empty as this is being done in lowLevelInit();
}

/**************************************************************************//**
 \brief Interrupt service routine
******************************************************************************/
ISR(TWI_INT_VECTOR)
{
  TWI_SlaveInterruptHandler(&twiSlave);
}

/**************************************************************************//**
 \brief Flash erase and write
 
 \param address start of the flash to be written, buf contains the data tp be
          written, len number of bytes to be written
******************************************************************************/
void nvm_flash_erase_and_write_buffer(uint32_t address, const void *buf, uint16_t len)
{
	volatile uint16_t w_value;
	volatile uint16_t page_pos;

	#if (FLASH_SIZE_TWI>0x10000)
	uint32_t page_address;
	uint32_t opt_address = address;
	#else
	uint16_t page_address;
	uint16_t opt_address = (uint16_t)address;
	#endif
        
	// Compute the start of the page to be modified
	page_address = opt_address-(opt_address%FLASH_PAGE_SIZE);
	
	// For each page
	while ( len ) 
        {
			for (page_pos=0; page_pos<FLASH_PAGE_SIZE; page_pos+=2 ) 
                        {

				// Update flash buffer
				if (len) 
                                {
					if (opt_address == page_address)
                                        {
						// The LSB of flash word must be changed
						// because the address is even
						len--;
						opt_address++;
						LSB(w_value)=*(uint8_t*)buf;
						buf=(uint8_t*)buf+1;
					}
				}
				if (len) {
					if (opt_address == (page_address+1)) {
						// The MSB of flash word must be changed
						// because the user buffer is not empty
						len--;
						opt_address++;
						MSB(w_value)=*(uint8_t*)buf;
						buf=(uint8_t*)buf+1;
					}
				}
                            // Perform word load.
                            SP_LoadFlashWord(page_address, w_value);
				
			    page_address+=2;
			}
			
            SP_EraseWriteApplicationPage(page_address-FLASH_PAGE_SIZE);			
	}
}

#endif