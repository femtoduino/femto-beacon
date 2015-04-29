/**
 * \file
 *
 * \brief SAM EEPROM Emulator Service Quick Start
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>

void configure_eeprom(void);

//! [setup]
void configure_eeprom(void)
{
	/* Setup EEPROM emulator service */
//! [init_eeprom_service]
	enum status_code error_code = rww_eeprom_emulator_init();
//! [init_eeprom_service]

//! [check_re-init]
	if (error_code != STATUS_OK) {
		/* Erase the emulated EEPROM memory (assume it is unformatted or
		 * irrecoverably corrupt) */
		rww_eeprom_emulator_erase_memory();
		rww_eeprom_emulator_init();
	}
//! [check_re-init]
}
//! [setup]

int main(void)
{
	system_init();

//! [setup_init]
	configure_eeprom();
//! [setup_init]

//! [main]
//! [read_page]
	uint8_t page_data[RWW_EEPROM_PAGE_SIZE];
	rww_eeprom_emulator_read_page(0, page_data);
//! [read_page]

//! [toggle_first_byte]
	page_data[0] = !page_data[0];
//! [toggle_first_byte]
//! [set_led]
	port_pin_set_output_level(LED_0_PIN, page_data[0]);
//! [set_led]

//! [write_page]
	rww_eeprom_emulator_write_page(0, page_data);
	rww_eeprom_emulator_commit_page_buffer();
//! [write_page]

	while (true) {

	}
//! [main]
}
