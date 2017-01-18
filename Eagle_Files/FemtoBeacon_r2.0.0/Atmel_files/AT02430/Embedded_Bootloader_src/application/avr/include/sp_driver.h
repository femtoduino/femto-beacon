/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA Self-programming driver header file.
 *
 *      This file contains the function prototypes for the
 *      XMEGA Self-programming driver.
 *      If any SPM instructions are used, the linker file must define
 *      a segment named BOOT which must be located in the device boot section.
 *
 *
 *      None of these functions clean up the NVM Command Register after use.
 *      It is therefore important to write NVMCMD_NO_OPERATION (0x00) to this
 *      register when you are finished using any of the functions in this driver.
 *
 *      For all functions, it is important that no interrupt handlers
 *      perform any NVM operations. The user must implement a scheme for mutually
 *      exclusive access to the NVM. However, the 4-cycle timeout will work fine,
 *      since writing to the Configuration Change Protection register (CCP)
 *      automatically disables interrupts for 4 instruction cycles.
 *
 * \par Application note:
 *      AVR1316: XMEGA Self-programming
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1691 $
 * $Date: 2008-07-29 13:25:40 +0200 (ti, 29 jul 2008) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#ifndef SP_DRIVER_H
#define SP_DRIVER_H

#define FLASH_PAGE_SIZE 256

/*! \brief This routine enables RWW section.
 *
 *  \param Nothing.
 */
void SP_RWWSectionEnable(void);

/*! \brief Erase page buffer to application or application table section at byte address.
 *
 *  This function does a combined erase and write to a flash page in the application
 *  or application table section.
 *
 *  \param address Byte address for flash page.
 */
void SP_EraseApplicationPage(uint32_t address);

/*! \brief Write page buffer to application or application table section at byte address.
 *
 *  This function does a combined erase and write to a flash page in the application
 *  or application table section.
 *
 *  \param address Byte address for flash page.
 */
void SP_WriteApplicationPage(uint32_t address);

/*! \brief Load one word into Flash page buffer.
 *
 *  This function Loads one word into the Flash page buffer.
 *
 *  \param  address   Position in inside the flash page buffer.
 *  \param  data      Value to be put into the buffer.
 */
void SP_LoadFlashWord(uint16_t address, uint16_t data);

/*!
 * \brief Read byte from internal flash
 *
 * \param address - Byte address into flash.
 *
 * \return read byte from flash.
 */
uint8_t SP_ReadInternalFlash(uint32_t address);

#endif /* SP_DRIVER_H */
