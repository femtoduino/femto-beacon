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
#if (USE_TWI == 1)

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
 \brief Enable TWI Module as slave

 \param none.
******************************************************************************/
static void TwiSlaveInterface_enable(void)
{
	TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | 
			(0 << TWWC);
}

/**************************************************************************//**
 \brief Send nack as a response

 \param none.
******************************************************************************/
static void TwiSlave_nack(void)
{
	TWCR = (1 << TWEN) |  (1 << TWIE) | (1 << TWINT) | (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | 
			(0 << TWWC);
}

/**************************************************************************//**
 \brief Sending data to twi bus.
 
 \param none.
******************************************************************************/
static void TwiSlave_data_write(void)
{
	if (twiSlave.bytesSent < TWIS_SEND_BUFFER_SIZE) 
        {
	    uint8_t data = twiSlave.sendData[twiSlave.bytesSent];
	    TWDR =  data;
	    twiSlave.bytesSent++;
	    TwiSlaveInterface_enable();
	}
}

/**************************************************************************//**
 \brief Notification of Last byte is transmitted

 \param none.
******************************************************************************/
static void TwiSlave_last_byte_write_done(void)
{
	TwiSlaveInterface_enable();
}

/**************************************************************************//**
 \brief Read received data

 \param data - Contains data read from twi bus to be written to data receive
        buffer.
******************************************************************************/
static void TwiSlave_data_read(uint8_t data)
{
	/* If free space in buffer. */
	if (twiSlave.bytesReceived < TWIS_RECEIVE_BUFFER_SIZE)
        {
	    twiSlave.receivedData[twiSlave.bytesReceived] = data;

	    twiSlave.bytesReceived++;
		
            TwiSlaveInterface_enable();

	}
	/* If buffer overflow, send NACK and wait for next START. Set
	 * result buffer overflow.
	 */
	else 
        {
            TwiSlave_nack();
			
	    TwiSlaveInterface_enable();
	}
}

/**************************************************************************//**
 \brief reads the data
 
 \param none.
 
 \return byte that was read from the TWI.
******************************************************************************/
static inline uint8_t Twi_read_byte(void)
{
	return TWDR;
}

/**************************************************************************//**
 \brief Reset TWI bus

 \param none.
******************************************************************************/
static void TwiSlave_bus_reset(void)
{
     TWCR = ((1 << TWSTO) | (1 << TWINT));
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
 \brief Stop the TWI communication

 \param none.
******************************************************************************/
static void TwiSlave_Stop(void)
{
    TwiSlaveInterface_enable();
	
    // Process the received data.
    TwiDataParser(twiSlave);

    // Flush the buffers used.
    flush_Twibuffer();
}

/**************************************************************************//**
 \brief TWI communication interface

 \param none.
******************************************************************************/
void TWI_SlaveInterruptHandler(void)
{
	switch (TWI_TWSR_STATUS_MASK) {
	case TWS_ST_SLA_ACK:        /* Own SLA+R has been received; ACK has been
		                     * returned */
		twiSlave.bytesSent   = 0; /* Set buffer pointer to first data
		                            * location */

	case TWS_ST_DATA_ACK:       /* Data byte in TWDR has been transmitted;
		                     * ACK has been received */
		TwiSlave_data_write();
		break;

	case TWS_ST_DATA_NACK:      /* Data byte in TWDR has been transmitted;
		                     * NACK has been received. */
		                    /* I.e. this could be the end of the
		                     * transmission. */
		TwiSlave_last_byte_write_done();
		break;

	case TWS_SR_GEN_ACK:        /* General call address has been received;
		                     * ACK has been returned */
	case TWS_SR_SLA_ACK:        /* Own SLA+W has been received ACK has been
		                     * returned */
		twiSlave.status = TWIS_STATUS_BUSY;
		twiSlave.bytesReceived = 0;
		twiSlave.bytesSent = 0;
		TwiSlaveInterface_enable();
		break;

	case TWS_SR_SLA_DATA_ACK:   /* Previously addressed with own SLA+W; data
		                     * has been received; ACK has been returned */
	case TWS_SR_GEN_DATA_ACK:   /* Previously addressed with general call;
		                     * data has been received; ACK has been
		                     * returned */
		TwiSlave_data_read(Twi_read_byte());
		break;

	case TWS_SR_STOP_RESTART:   /* A STOP condition or repeated START
		                     * condition has been received while still
		                     * addressed as Slave */
		/* Enter not addressed mode and listen to address match */
		TwiSlave_Stop();
		break;

	case TWS_SR_SLA_DATA_NACK:  /* Previously addressed with own SLA+W; data
		                     * has been received; NOT ACK has been
		                     * returned */
	case TWS_SR_GEN_DATA_NACK:  /* Previously addressed with general call;
		                     * data has been received; NOT ACK has been
		                     * returned */
	case TWS_ST_DATA_ACK_LAST_BYTE: /* Last data byte in TWDR has been
		                         * transmitted (TWEA = “0”); ACK has
		                         * been received */
	case TWS_BUS_ERROR:     /* Bus error due to an illegal START or STOP
		                * condition */
		TwiSlave_bus_reset();
		break;

	default:
		break;
	}
}

/**************************************************************************//**
 \brief TWI read write interface

 \param none.
******************************************************************************/
void TwiSlave_ReadWrite(void)
{
   TWI_SlaveInterruptHandler();
}

/**************************************************************************//**
 \brief TWI communication initialise

 \param none.
******************************************************************************/
void hwInitTwi(void)
{
	/* Enable Clock for TWI module */
	PRR0 = PRR0 & PRTWI_MASK;	
	/* Set own TWI slave address */
	TWAR = (SLAVE_BUS_ADDR<<1) | TWI_GCE;                      
	TWCR = (1 << TWEN) | (0 << TWIE) | (0 << TWINT) | (0 << TWEA) |
          (0 << TWSTA) | (0 << TWSTO) | (0 << TWWC); 
        
        
	twiSlave.bytesReceived = 0;
	twiSlave.bytesSent = 0;
	twiSlave.status = TWIS_STATUS_READY;
	TWI_State = TWI_Start;
	Write_Pending = false;
        
    TwiSlaveInterface_enable();	

}

/**************************************************************************//**
 \brief IO Port initialisation for selecting between Boot Section and App Sec

 \param none.
******************************************************************************/
void ButtonLedInit(void)
{
#if MESHBEAN_BOARD == YES  
   /* Configure PortE.6 pin as input pin for SW PortB.5 as output for LED */
  DDRE = 0x00;
  
  DDRB = 0x20;
  
  PORTB = 0x20;
  
  /* Enable the Pullup */
  PORTE = 0x40;
  
#else
   /* Configure PortE.5 pin as input for SW PortE.2 as output for LED*/
  DDRE = 0x04;
  
  /* Enable the Pullup */
  PORTE = 0x24;  
#endif
}

/**************************************************************************//**
 \brief IO Port uninitialisation for selecting between Boot Section and App Sec

 \param none.
******************************************************************************/
void ButtonLedUnInit(void)
{
#if MESHBEAN_BOARD == YES  
   /* Configure PortE.6 pin as input pin for SW PortB.5 as output for LED */
  DDRE = 0x00;
  
  DDRB = 0x00;
  
  /* Enable the Pullup */
  PORTE = 0x00;
#else
   /* Configure PortE.5 pin as input for SW PortE.2 as output for LED*/
  DDRE = 0x00;
  
  /* Enable the Pullup */
  PORTE = 0x00;  
#endif
}

/**************************************************************************//**
 \brief Moving the interrupt vector from Boot Section  to App Section

 \param none.
******************************************************************************/
void moveIntVectortoApp(void)
{
   // Intensionalnally left empty as the TWI interface is not interrupt based 
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
	while ( len ) {

		for (char page_no=0; page_no<2; page_no++ )
		{
                  // Perform page erase.
                  SP_EraseApplicationPage(page_address);
                  // Wait for SPM to finish.
                  flashWaitForSPM();
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
                    if (len)
                    {
                      if (opt_address == (page_address+1)) 
                      {
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
                    // Wait for SPM to finish.
                    flashWaitForSPM();
                    page_address+=2;
                  }
			
                 // Perform page write.
                 SP_WriteApplicationPage(page_address-FLASH_PAGE_SIZE);
                 // Wait for SPM to finish.
                 flashWaitForSPM();
                }
	}

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.
    SP_RWWSectionEnable();
    // Wait for SPM to finish.
    flashWaitForSPM();

}
#endif