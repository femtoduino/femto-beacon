/****************************************************************************//**
  \file hardwareInit.c

  \brief Hardware initialization routine.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29.08.11 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <AT91SAM3S4.h>
#include <core_cm3.h>
#include <types.h>
#include <hardwareInit.h>
#include <srecParser.h>
#include <freqSelector.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/* Key for Clock Generator Main Oscillator Register */
#define MAIN_OSCILLATOR_REGISTER_KEY    (0x37ul << 16)
/* Key for Supply Controller Control Register */
#define SUPPLY_CONTROL_REGISTER_KEY     (0xA5ul << 24)
/* Key for Reset Controller Control Register */
#define RESET_CONTROL_REGISTER_KEY      (0xA5ul << 24)
/* Bit 29 must always be set to 1 when programming the CKGR_PLLXR register */
#define PMC_PLL_SET_BIT29               (0x1 << 29)
/* Specifies the number of Slow Clock cycles x8 before the LOCKX bit is set in PMC_SR after CKGR_PLLAR is written */
#define PMC_PLL_COUNT                   (0xBF << 8)
/* RC oscillator frequency selection */
#define RC_8MHZ_PRESCALER               (1 << 4)
/* PLLx Multiplier field offset in CKGR_PLLAx register */
#define MULx 16

///////////////////////////////////////////////////////////////////////////////
/* application entry point defines */
#define APP_ENTRY_POINT        0xFFFFFFFF
/* application message for bootloader to change image */
#define APP_LABEL_MESSAGE      0x5A000000  
/* mask  for application label */
#define APP_LABEL_MASK         0xFF000000  

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void frequencyInit(void);
static void setFrequencyDefault(void);

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
 \brief This function performs frequency low level HW initialization
******************************************************************************/
static void frequencyInit(void)
{
  /* Configure clocking from xtal oscillator */
  /* 1. Enabling the Main Oscillator:
     Start crystal oscillator.
     Typically startup time 1.4 ms for crystals from 12 to 16 MHz. 1 ms for 18 MHz.
     Slow clock = 1/32768 = 30.51 us approximately.
     Start up time = 8 * 32 / SCK = 256 / 32768 = 7.8 ms */
  PMC->CKGR_MOR |= MAIN_OSCILLATOR_REGISTER_KEY | CKGR_MOR_MOSCXTEN | (32ul << 8);
  /* Wait for stabilization of the crystal oscillator. */
  while (!(PMC->PMC_SR & PMC_SR_MOSCXTS));
  /* Select crystal oscilator as the clock generator output */
  PMC->CKGR_MOR |= MAIN_OSCILLATOR_REGISTER_KEY | CKGR_MOR_MOSCSEL;
  /* Wait for the crystal oscillator selection is done. */
  while (!(PMC->PMC_SR & PMC_SR_MOSCSELS));

  /* 2. Checking the Main Oscillator Frequency (Optional) */
    /* Skipped */

  /* 3-4. Setting PLL and divider & Selection of Master Clock and Processor Clock: */
  /* Use Main Clock as a source of Master Clock if possible.
     Set up the  Clock prescaler to produce required frequency */
  /* Initialize PLLA to provide required frequncy */
  PMC->CKGR_PLLAR = PMC_PLL_SET_BIT29 | ((PLLA_MUL - 1) << MULx) | PLLA_DIV | PMC_PLL_COUNT;
  /* Wait for the LOCKA bit to be set by polling the status register.*/
  while (!(PMC->PMC_SR & PMC_SR_LOCKA));
  /* Set master clock prescaler and PLL/2 prescaler if required */
  PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_PRES) | MCK_DIV | PLLA_DIV2;
  /* Wait for the MCKRDY bit to be set by polling the status register. */
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY));
  /* Set up PLLA as master and processor clock source */
  PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_CSS) | PMC_MCKR_CSS_PLLA_CLK;
  /* Wait for the MCKRDY bit to be set by polling the status register. */
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY));

  /* Switch off internal RC oscillator */
  PMC->CKGR_MOR = (PMC->CKGR_MOR | MAIN_OSCILLATOR_REGISTER_KEY) & ~CKGR_MOR_MOSCRCEN;
  while (PMC->PMC_SR & PMC_SR_MOSCRCS);

  /* Configuring USB Controller clock if required */
  /* Switch on and setup PLLB for USB domain.
     PLL B is going to provide 48 MHz clock to USB controller. */
  PMC->CKGR_PLLBR = PMC_PLL_SET_BIT29 | ((PLLB_MUL - 1) << MULx) | PLLB_DIV | PMC_PLL_COUNT;
  /* Wait for the LOCKB bit to be set by polling the status register.*/
  while (!(PMC->PMC_SR & PMC_SR_LOCKB));
  /* Set PLL/2 divider if required */
  PMC->PMC_MCKR |= PLLB_DIV2;
  /* Set PLL B as USB Controller clock source */
  PMC->PMC_USB = ((USB_DIV - 1) << 8) | PMC_USB_USBS;
  /* switch off pull on */
  UDP->UDP_TXVC &= ~UDP_TXVC_PUON;
  /* disable usb tranceiver */
  UDP->UDP_TXVC |= UDP_TXVC_TXVDIS;
}

/**************************************************************************//**
\brief Set up default value
******************************************************************************/
static void setFrequencyDefault(void)
{
  /* Restore default setup value: enable Main On-Chip RC oscillator running on
     4 MHz for Main Clock source */
  /* Enable Main clock first */
  PMC->CKGR_MOR |= MAIN_OSCILLATOR_REGISTER_KEY | CKGR_MOR_MOSCRCEN;
  /* Wait for stabilization of the RC oscillator */
  while (!(PMC->PMC_SR & PMC_SR_MOSCRCS));
  /* Configuring Master clock to work from Main clock source
     (it is needed after IAR startup script) */
  PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_CSS) | PMC_MCKR_CSS_MAIN_CLK;
  /* Wait for the MCKRDY bit to be set by polling the status register */
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY));
  /* Disabling Master clock prescaler  */
  PMC->PMC_MCKR &= ~PMC_MCKR_PRES;
  /* Wait for the MCKRDY bit to be set by polling the status register */
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY));
  /* Disable crystal oscillator */
  PMC->CKGR_MOR = (PMC->CKGR_MOR | MAIN_OSCILLATOR_REGISTER_KEY) & ~CKGR_MOR_MOSCSEL;
  /* Disable PLLA */
  PMC->CKGR_PLLAR = PMC_PLL_SET_BIT29;
  /* Disable PLLB */
  PMC->CKGR_PLLBR = PMC_PLL_SET_BIT29;
}

/**************************************************************************//**
\brief Startup initialization (frequency, io, usb)
******************************************************************************/
void lowLevelInit(void)
{
  uint32_t appEntryPoint = *(uint32_t *)(FLASH_END_ADDRESS + BOOT_AREA_SIZE - sizeof(uint32_t) + 1);
  uint32_t appLabel = *(uint32_t *)(FLASH_END_ADDRESS + BOOT_AREA_SIZE - 2 * sizeof(uint32_t) + 1);
  bool startBootloader = false;

  /* Enable power for PIOB */
  PMC->PMC_PCER0 = (1UL << ID_PIOB);
  /* set TCK as gpio */
  MATRIX->CCFG_SYSIO = CCFG_SYSIO_SYSIO7;
  PIOB->PIO_ODR = PIO_PB7;
  PIOB->PIO_PPDDR = PIO_PB7;
  PIOB->PIO_PUER = PIO_PB7;

  if (!(PIOB->PIO_PDSR & PIO_PB7))  /* jumper was set for recover image */
    startBootloader = true;

  /* clear gpio function */
  MATRIX->CCFG_SYSIO = 0ul;
  /* Disable power for PIOB */
  PMC->PMC_PCER0 = (1UL << ID_PIOB);
  
  if (APP_ENTRY_POINT == appEntryPoint) /* empty entry point */
    startBootloader = true;

  if ((appLabel & APP_LABEL_MASK) == APP_LABEL_MESSAGE) /* application initiated boot process */
    startBootloader = true;

  if (!startBootloader)
  {
#ifdef  __IAR_SYSTEMS_ICC__
    __set_SP(*(uint32_t *)FLASH_START_ADDRESS);
#elif __GNUC__
    __asm__ volatile ("mov r13, %0; \n\t"
          :
          : "r" (*(uint32_t *)FLASH_START_ADDRESS) /* input */
          );
#endif
    ((void (*)(void))appEntryPoint)();
  }
  /* It needed if bootloder would not start the first. */
  setFrequencyDefault();

  /* Select  RC 8MHz as the clock generator output
     to avoid bug with 2 WS when running at a frequency lower than 5 MHz */
  PMC->CKGR_MOR |= MAIN_OSCILLATOR_REGISTER_KEY | RC_8MHZ_PRESCALER;
  /* Wait for stabilization of the RC oscillator. */
  while (!(PMC->PMC_SR & PMC_SR_MOSCRCS));
  /* Set 3 WS for Embedded Flash Access &
     128-bit access in read Mode only, to enhance access speed */
  EFC->EEFC_FMR = (ACTIVE_FWS << 8);

  /* Configure clock source system */
  frequencyInit();

  /* Configure NRST pin */
  RSTC->RSTC_CR = RESET_CONTROL_REGISTER_KEY | RSTC_CR_EXTRST;

  /* Disable watchdog timer */
  WDT->WDT_MR = WDT_MR_WDDIS;
}

/**************************************************************************//**
\brief Clear startup initialization parameters to start user application
******************************************************************************/
void lowLevelUnInit(void)
{
  setFrequencyDefault();
}

// eof hardwareInit.c
