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
    6/05/13 S. Kanchana - Created
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

/* Buffer size defines. */
#define TWIS_SEND_BUFFER_SIZE            160

/* Transaction status defines.*/
#define TWIS_STATUS_READY                0
#define TWIS_STATUS_BUSY                 1

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
	uint8_t receivedData;                           /*!< Read data*/
	uint8_t *sendData;                              /*!< Data to write*/
	uint8_t bytesSent;                              /*!< Number of bytes sent*/
	uint8_t status;                                 /*!< Status of transaction*/
	uint8_t result;                                 /*!< Result of transaction*/
	bool abort;                                     /*!< Strobe to abort*/
} TWI_Slave_t;

#if (USE_TWIS_C == 1) 
/**************************************************************************//**
\brief Startup initialization of the TWI as slave 
******************************************************************************/
void hwInitTwi_C(void);

/**************************************************************************//**
 \brief TWI module Uninitialisation
 ******************************************************************************/
void hwUnInitTwi_C(void);
#endif

#if (USE_TWIS_D == 1) 
/**************************************************************************//**
\brief Startup initialization of the TWI as slave 
******************************************************************************/
void hwInitTwi_D(void);

/**************************************************************************//**
 \brief TWI module Uninitialisation
 ******************************************************************************/
void hwUnInitTwi_D(void);
#endif

#if (USE_TWIS_E == 1) 
/**************************************************************************//**
\brief Startup initialization of the TWI as slave 
******************************************************************************/
void hwInitTwi_E(void);

/**************************************************************************//**
 \brief TWI module Uninitialisation
 ******************************************************************************/
void hwUnInitTwi_E(void);
#endif

#if (USE_TWIS_F == 1) 
/**************************************************************************//**
\brief Startup initialization of the TWI as slave 
******************************************************************************/
void hwInitTwi_F(void);

/**************************************************************************//**
 \brief TWI module Uninitialisation
 ******************************************************************************/
void hwUnInitTwi_F(void);
#endif
/**************************************************************************//**
\brief Receive byte on TWI.
******************************************************************************/
bool getByteTwi(uint8_t *p);

/**************************************************************************//**
\brief Transmit byte to TWI.
******************************************************************************/
void setByteTwi(uint16_t len, uint8_t *p);

#endif  /* _TWISERIALIZER_H*/
