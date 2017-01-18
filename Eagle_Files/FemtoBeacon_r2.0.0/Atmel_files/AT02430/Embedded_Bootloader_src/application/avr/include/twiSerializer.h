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
#define PRTWI_MASK           0x7F
#define PRTWI_DISABLE        0x80

#if (ATMEGA2561 == 1)
#define FLASH_SIZE_TWI      (256*1024L)
#else
#define FLASH_SIZE_TWI      (128*1024L)
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

/* TWSR status mask */
#define TWI_TWSR_STATUS_MASK    (TWSR & 0xF8)

/* Input port pin status check macro */
#if MESHBEAN_BOARD == YES
#define ISBUTTONPRESSED()      (PINE & 0x40)
#else
#define ISBUTTONPRESSED()      (PINE & 0x20)
#endif

/* LED ON/OFF macro */
#if MESHBEAN_BOARD == YES
#define LED_ON()      (PORTB = PORTB & 0xDF)
#define LED_OFF()     (PORTB = PORTB & 0x20)
#else
#define LED_ON()       (PORTE = PORTE & 0xFB)
#define LED_OFF()      (PORTE = PORTE | 0x04)
#endif

/**************************************************************************//**
 \brief TWI Slave status codes
 ******************************************************************************/
enum {
	TWS_BUS_ERROR                  = 0x00, /* ! Bus error due to an illegal
	                                       * START or STOP condition */
	TWS_ST_SLA_ACK                 = 0xA8, /* ! Own SLA+R has
	                                                       * been received;
	                                                       * ACK has been
	                                                       * returned */
	TWS_ST_SLA_ACK_M_ARB_LOST      = 0xB0, /* ! Arbitration lost in
	                                           * SLA+R/W as Master; own
	                                           * SLA+R has been received;
	                                           * ACK has been returned */
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
	TWS_SR_SLA_ACK_M_ARB_LOST      = 0x68, /* ! Arbitration lost in
	                                           * SLA+R/W as Master; own
	                                           * SLA+W has been received;
	                                           * ACK has been returned */
	TWS_SR_GEN_ACK                 = 0x70, /* ! General call
	                                                   * address has been
	                                                   * received; ACK has
	                                                   * been returned */
	TWS_SR_GEN_ACK_M_ARB_LOST      = 0x78, /* ! Arbitration lost in SLA+R/W
	                                       * as Master; General call address
	                                       * has been received; ACK has been
	                                       * returned */
	TWS_SR_SLA_DATA_ACK            = 0x80, /* ! Previously addressed
	                                               * with own SLA+W; data
	                                               * has been received; ACK
	                                               * has been returned */
	TWS_SR_SLA_DATA_NACK           = 0x88, /* ! Previously addressed with
	                                           * own SLA+W; data has been
	                                           * received; NOT ACK has been
	                                           * returned */
	TWS_SR_GEN_DATA_ACK            = 0x90, /* ! Previously addressed with
	                                       * general call; data has been
	                                       * received; ACK has been returned */
	TWS_SR_GEN_DATA_NACK           = 0x98, /* ! Previously addressed with
	                                       * general call; data has been
	                                       * received; NOT ACK has been
	                                       * returned */
	TWS_SR_STOP_RESTART            = 0xA0 /* ! A STOP condition or repeated
	                                      * START condition has been
	                                      * received while still addressed
	                                      * as Slave */
};

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
	uint8_t receivedData[TWIS_RECEIVE_BUFFER_SIZE]; /*!< Read data*/
	uint8_t sendData[TWIS_SEND_BUFFER_SIZE];        /*!< Data to write*/
	uint16_t bytesReceived;                          /*!< Number of bytes received*/
	uint8_t bytesSent;                              /*!< Number of bytes sent*/
	uint8_t status;                                 /*!< Status of transaction*/
	uint8_t result;                                 /*!< Result of transaction*/
	bool abort;                                     /*!< Strobe to abort*/
} TWI_Slave_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/

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


#endif  /* _TWIINTERFACE_H*/
