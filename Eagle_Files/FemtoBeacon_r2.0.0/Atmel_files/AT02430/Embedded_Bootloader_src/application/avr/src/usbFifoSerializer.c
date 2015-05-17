/****************************************************************************//**
  \file usbFifoSerializer.c

  \brief Implementation of FT245R USB FIFO IC serialize interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    4.02.11 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#if (USE_USB_FIFO == 1)
#include <types.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/******************************************************************************
* void gpioX_set() sets GPIOX pin to logical 1 level.
* void gpioX_clr() clears GPIOX pin to logical 0 level.
* void gpioX_make_in makes GPIOX pin as input.
* void gpioX_make_in makes GPIOX pin as output.
* uint8_t gpioX_read() returns logical level GPIOX pin.
* uint8_t gpioX_state() returns configuration of GPIOX port.
*******************************************************************************/
#define HAL_ASSIGN_PIN(name, port, bit) \
INLINE void  GPIO_##name##_set()         {PORT##port |= (1 << bit);} \
INLINE void  GPIO_##name##_clr()         {PORT##port &= ~(1 << bit);} \
INLINE uint8_t  GPIO_##name##_read()     {return (PIN##port & (1 << bit)) != 0;} \
INLINE void  GPIO_##name##_make_out()    {DDR##port |= (1 << bit);} \
INLINE void  GPIO_##name##_make_in()     {DDR##port &= ~(1 << bit); PORT##port &= ~(1 << bit);}

/******************************************************************************
                   Prototypes section
******************************************************************************/
#if defined(ATMEGA128RFA1)
// the macros for the manipulation by PC6 for binary decoder
HAL_ASSIGN_PIN(PD6, D, 6);
// the macros for the manipulation by PC7 for binary decoder
HAL_ASSIGN_PIN(PD7, D, 7);
// the macros for the manipulation by RD
HAL_ASSIGN_PIN(RD, E, 5);
// the macros for the manipulation by WR
HAL_ASSIGN_PIN(WR, E, 4);
// the macros for the manipulation by TXE
HAL_ASSIGN_PIN(TXE, E, 6);
// the macros for the manipulation by RXF
HAL_ASSIGN_PIN(RXF, E, 7);
// data port
#define USB_DATA_PORT   PORTB
#define USB_DATA_DDR    DDRB
#define USB_DATA_PIN    PINB

#elif defined(ATMEGA1281)

// the macros for the manipulation by PC6 for binary decoder
HAL_ASSIGN_PIN(PD6, C, 6);
// the macros for the manipulation by PC7 for binary decoder
HAL_ASSIGN_PIN(PD7, C, 7);
// the macros for the manipulation by RD
HAL_ASSIGN_PIN(RD, G, 1);
// the macros for the manipulation by WR
HAL_ASSIGN_PIN(WR, G, 0);
// the macros for the manipulation by TXE
HAL_ASSIGN_PIN(TXE, E, 6);
// the macros for the manipulation by RXF
HAL_ASSIGN_PIN(RXF, E, 7);
// data port
#define USB_DATA_PORT   PORTA
#define USB_DATA_DDR    DDRA
#define USB_DATA_PIN    PINA

#else

  #error 'USB FIFO is not supported for this mcu.'

#endif
/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Startup initialization.
******************************************************************************/
void hwInitUsbFifo(void)
{
  // init decoder input pins
  GPIO_PD6_make_out();
  GPIO_PD7_make_out();
  // set pins to zero to select usb FIFO
  GPIO_PD6_clr();
  GPIO_PD7_clr();

  // setup default value
  GPIO_RD_set();
  GPIO_WR_set();
  // init R\W pins
  GPIO_RD_make_out();
  GPIO_WR_make_out();

  // init message pins
  GPIO_TXE_make_in();
  GPIO_RXF_make_in();

  USB_DATA_DDR = 0;
  //to avoid IAR warnings
  (void)GPIO_PD6_set;
  (void)GPIO_PD6_read;
  (void)GPIO_PD7_set;
  (void)GPIO_PD7_read;
  (void)GPIO_RD_read;
  (void)GPIO_WR_read;
  (void)GPIO_TXE_set;
  (void)GPIO_TXE_clr;
  (void)GPIO_TXE_make_out;
  (void)GPIO_RXF_set;
  (void)GPIO_RXF_clr;
  (void)GPIO_RXF_make_out;
}

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void hwUnInitUsbFifo(void)
{
  GPIO_PD6_make_in();
  GPIO_PD7_make_in();

  GPIO_RD_make_in();
  GPIO_WR_make_in();
}

/**************************************************************************//**
\brief Receive byte from usb FIFO.

\param[out]
  p - pointer to byte memory;

\return
    true - there is received byte; \n
    false - there is not received byte;
******************************************************************************/
bool getByteUsbFifo(uint8_t *p)
{
  if (!GPIO_RXF_read())
  {
    GPIO_RD_clr();
    NOP;
    *p = USB_DATA_PIN;
    GPIO_RD_set();
    return true;
  }

  return false;
}

/**************************************************************************//**
\brief Transmit byte to usb FIFO.

\param[in]
  len - number of bytes to transmit;
\param[in]
  p - pointer to byte memory;
******************************************************************************/
void setByteUsbFifo(uint16_t len, uint8_t *p)
{
  wdt_reset();

  // set PORTA as output
  USB_DATA_DDR = 0xFF;

  while (len--)
  {
    while (GPIO_TXE_read());
    USB_DATA_PORT = *p++; // prepare transmission
    GPIO_WR_clr();
    GPIO_WR_set();
  }

  // set PORTA as input
  USB_DATA_DDR = 0;
}

#endif // (USE_USB_FIFO == 1)

// eof usbFifoSerializer.c
