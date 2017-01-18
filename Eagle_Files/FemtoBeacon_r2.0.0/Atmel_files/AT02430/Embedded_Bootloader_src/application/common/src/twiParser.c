/****************************************************************************//**
  \file AVR1624twiParser.c

  \brief Implementation of protocol as per AVR1624.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    25/02/13 S. Kanchana - Created
*******************************************************************************/
#if (USE_TWI == 1) || (USE_TWIC == 1) || (USE_TWID == 1) || (USE_TWIE == 1) || (USE_TWIF ==1)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <abstractSerializer.h>
#include <twiParser.h>

/******************************************************************************
                   Global variables section
******************************************************************************/
bool Write_Pending;
extern TWI_State_t TWI_State;

static bool New_Page = true;
static uint8_t Write_Buffer[FLASH_PAGE_SIZE_TWI];

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
 \brief TWI data parser as per AVR1624 protocol
 
 \param TwiData data received through TWI interface
******************************************************************************/
void TwiDataParser(TWI_Slave_t TwiData)
{
	uint32_t bufIndex = TwiData.bytesReceived;
	uint32_t address = 0;
	bool Data_Ready = false;
	uint16_t Data_Length;
	uint8_t twi_command = 0;
	
	// First three bytes received are address bytes.
	address = TwiData.receivedData[2];
	address = (address << 8) | (TwiData.receivedData[1]);
	address = (address << 8) | (TwiData.receivedData[0]);
	
	// Next byte received is the Command/operation to be performed.
	twi_command = TwiData.receivedData[3];
	

	// Check for the received command.
	switch(twi_command)
	{
		// 'P' - Enter programming mode.
		case 'P':
		//Confirm TWI is Start state and then change it programming mode.
		if(TWI_State == TWI_Start) {
			TWI_State = TWI_Prog_Mode;
		}
		break;
		
		// 'E' - Erase Application section.
		case 'E':
		// Check TWI state and erase application section.
		if(TWI_State == TWI_Prog_Mode) {
                  TWI_State = TWI_Start;
			//erasing is performed during writing the data into flash
		}
		break;
		
		// 'D' - Following information is data to be programmed.
		case 'D':
		if(TWI_State == TWI_Prog_Mode) {
			// Data_Length is total number of bytes received excluding
			// 4 bytes - 3 address bytes and one command byte.
			Data_Length = bufIndex-4;
			
			// Confirm that 256 bytes of data has been received.
			if(Data_Length == (FLASH_PAGE_SIZE_TWI/2)) {
				
				// Check whether data received is first half of a page.
				if(New_Page == true) {
					
					// Copy the data to Write buffer.
					for (int i = 0; i < FLASH_PAGE_SIZE_TWI/2; i++) {
						Write_Buffer[i] = TwiData.receivedData[i+4];
					}
					
					// Set flag to indicate next data will be second half of a page
					New_Page = false;
					// Set flag to indicate data is pending to be written to flash.
					Write_Pending = true;
				}
				
				// Check whether received data is second half of a page.
				else if(New_Page == false) {
					int j = 4;
					
					// Copy the data to Write buffer.
					for(int i = FLASH_PAGE_SIZE_TWI/2; i < FLASH_PAGE_SIZE_TWI; i++) {
						Write_Buffer[i] = TwiData.receivedData[j];
						j++;
					}
					
					// Set flag to indicate next data will be first half of a page.
					New_Page = true;
					// Set flag to indicate data is ready to be written to flash.
					Data_Ready = true;
				}
			}
			else {
				Data_Ready = false;
				TwiData.sendData[0] = Write_Pending;
			}
		}
		break;
		
		// 'X' - Exit programming mode and reset the device.
		case 'X':
		if(TWI_State == TWI_Prog_Mode) {
		       // Set TWI State to Start state.
		       TWI_State = TWI_Start;                        
                       
                // Move back Interrupt vector table to Application section
                moveIntVectortoApp();
                
                LED_OFF();
     
                ButtonLedUnInit();                
                
		    	// Reset the device.
                void(* startAddress)(void) =(void(*)(void))NULL;

                // nulling extended indirect register for indirect call out memory
                CLEAR_EIND();
				
                // go to NULL address
                startAddress();
		}
		break;
	}
	
	if(Data_Ready == true) {
          
		
		nvm_flash_erase_and_write_buffer(address, (const void *)Write_Buffer, sizeof(Write_Buffer));
		
		// Set flag to indicate there is no more data to be programmed.
		Data_Ready = false;
		// Set flag to indicate that write to flash has been completed.
		Write_Pending = false;
		// Set the status for master to read.
		TwiData.sendData[0] = Write_Pending;
	}
}

/**************************************************************************//**
\brief Handshake procedure.
******************************************************************************/
void twiBootHandshake(void)
{
  ButtonLedInit();
  
  if(!ISBUTTONPRESSED())
  {   
     /* Switch ON LED to indicate Boot mode */
     LED_ON();
     
     hwInitTwi();
  }
  else
  {
    
    // Move back Interrupt vector table to Application section
     moveIntVectortoApp();  
     
     LED_OFF();
     
     ButtonLedUnInit();
    
     void(* startAddress)(void) =(void(*)(void))NULL;

     // nulling extended indirect register for indirect call out memory
     CLEAR_EIND();
     // go to NULL address
     startAddress();
  }
}

/**************************************************************************//**
\brief TWI communication
******************************************************************************/
void twiBootCheck(void)
{
  while(1)
  {
	  TwiSlave_ReadWrite();
      wdt_reset();
  }
}
#endif