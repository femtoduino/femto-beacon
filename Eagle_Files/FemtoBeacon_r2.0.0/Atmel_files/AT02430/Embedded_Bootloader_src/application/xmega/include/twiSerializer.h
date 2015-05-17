/****************************************************************************//**
  \file twiInterface.h

  \brief Declaration of twi interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15/02/13 S. Kanchana - Created
*******************************************************************************/
#ifndef _TWIINTERFACE_H
#define _TWIINTERFACE_H

/******************************************************************************
                  Includes section
******************************************************************************/
#include <types.h>
#include <twiParser.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#ifndef INFINITE
  #define INFINITE true
#endif

/* states of the twi slave transaction */
#define TWI_STATE_IDLE                    0
#define TWI_STATE_BUSY                    1

#define SLAVE_BUS_ADDR       0x55        /* !< TWI slave bus address */
#define TWI_GCE              0x01        /* !< TWI slave general call enable */

#if (ATXMEGA128A1 ==1)
#define FLASH_SIZE_TWI      (128*1024L)
#else
#define FLASH_SIZE_TWI      (256*1024L)
#endif

/* Page size is kept const as 512 as the Xplain master will send the 512 bytes */
#define FLASH_PAGE_SIZE_TWI (512)

/* Buffer size defines. */
#define TWIS_RECEIVE_BUFFER_SIZE         ((FLASH_PAGE_SIZE_TWI/2)+4)
#define TWIS_SEND_BUFFER_SIZE            1

/* Transaction status defines.*/
#define TWIS_STATUS_READY                0
#define TWIS_STATUS_BUSY                 1

#define MSB(u16)             (((uint8_t* )&u16)[1])
#define LSB(u16)             (((uint8_t* )&u16)[0])

/* Input port pin status check macro */
#define ISBUTTONPRESSED()     (PORTB_IN & 0x08)

/* LED ON/OFF macro */
#if (REB_CBB_BOARD == YES)
#define LED_ON()      (PORTB_OUT = PORTB_OUT | 0x04)
#define LED_OFF()     (PORTB_OUT = PORTB_OUT & 0xFB)
#else
#define LED_ON()      (PORTB_OUT = PORTB_OUT & 0xFB)
#define LED_OFF()     (PORTB_OUT = PORTB_OUT | 0x04)
#endif

/* TWI module */
#if (USE_TWIC == 1)
#define TWI_MODULE    &TWIC
#define TWI_INT_VECTOR TWIC_TWIS_vect
#endif

#if (USE_TWID == 1)
#define TWI_MODULE    &TWID
#define TWI_INT_VECTOR TWID_TWIS_vect
#endif

#if (USE_TWIE == 1)
#define TWI_MODULE    &TWIE
#define TWI_INT_VECTOR TWIE_TWIS_vect
#endif

#if (USE_TWIF == 1)
#define TWI_MODULE    &TWIF
#define TWI_INT_VECTOR TWIF_TWIS_vect
#endif

/**************************************************************************//**
 \brief TWI communication protocol state
 ******************************************************************************/
typedef enum TWI_State_enum
{
	TWI_Start     =0x00,
	TWI_Prog_Mode =0x01
} TWI_State_t;

/**************************************************************************//**
 \brief Transaction result enumeration
 ******************************************************************************/
typedef enum TWIS_RESULT_enum {
	TWIS_RESULT_UNKNOWN            = (0x00<<0),
	TWIS_RESULT_OK                 = (0x01<<0),
	TWIS_RESULT_BUFFER_OVERFLOW    = (0x02<<0),
	TWIS_RESULT_TRANSMIT_COLLISION = (0x03<<0),
	TWIS_RESULT_BUS_ERROR          = (0x04<<0),
	TWIS_RESULT_FAIL               = (0x05<<0),
	TWIS_RESULT_ABORTED            = (0x06<<0),
} TWIS_RESULT_t;

/**************************************************************************//**
 \brief TWI slave driver struct.
  TWI slave struct. Holds pointer to TWI module and data processing routine,
  buffers and necessary variables.
 ******************************************************************************/
typedef struct TWI_Slave {
	TWI_t *interface;                               /*!< Pointer to what interface to use*/  
	uint8_t receivedData[TWIS_RECEIVE_BUFFER_SIZE]; /*!< Read data*/
	uint8_t sendData[TWIS_SEND_BUFFER_SIZE];        /*!< Data to write*/
	uint16_t bytesReceived;                          /*!< Number of bytes received*/
	uint8_t bytesSent;                              /*!< Number of bytes sent*/
	uint8_t status;                                 /*!< Status of transaction*/
	uint8_t result;                                 /*!< Result of transaction*/
	bool abort;                                     /*!< Strobe to abort*/
} TWI_Slave_t;

/**************************************************************************//**
\brief Startup initialization of the twi as slave 
******************************************************************************/
void hwInitTwi(void);

/**************************************************************************//**
 \brief Communication support between twi master and slave
******************************************************************************/
void TwiDataParser(TWI_Slave_t TwiReceivedData);

/**************************************************************************//**
\brief Port initialisation for selecting the code
******************************************************************************/
void ButtonLedInit(void);

/**************************************************************************//**
 \brief IO Port uninitialisation for selecting between Boot Section and App Sec
******************************************************************************/
void ButtonLedUnInit(void);

/**************************************************************************//**
 \brief TWI read write interface
******************************************************************************/
void TwiSlave_ReadWrite(void);

/**************************************************************************//**
\brief NVM flash write
******************************************************************************/
void nvm_flash_erase_and_write_buffer(uint32_t address, const void *buf, uint16_t len);

/**************************************************************************//**
\brief Move the interrupt vector to application section
******************************************************************************/
void moveIntVectortoApp(void);

/**************************************************************************//**
 \brief Moving the interrupt vector from App Section  to Boot Section
******************************************************************************/
void moveIntVectortoBoot(void);

#endif  /* _TWIINTERFACE_H*/
