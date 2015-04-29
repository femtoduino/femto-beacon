/**
 * \file
 *
 * \brief SAM Power Driver Quick Start
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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

 /** \page asfdoc_sam0_power_basic_use_case Quick Start Guide for Power Driver
 *
 * The supported board list:
 *    - SAM L21 Xplained Pro
 *
 * This example demonstrates how to use the power driver. BUTTON0 is used to
 * wake up system from standby mode and as an external wake up
 * pin to wake up system from BACKUP mode. The wakeup pin level is low.
 * PB22/PB23 are used as GCLK0/GCLK1 output pin, oscilloscope can be used to
 * monitor their clock frequency.
 *
 * After POR, LED0 is ON and GCLK0/GCLK1 are running at 4MHz, after one second,
 * LED0 becomes OFF and system enters standby mode. BUTTON0 can be used to wake up
 * system. After system wake up, LED0 becomes ON and performance level switch to PL2,
 * GCLK0 is running at 48MHz, GCLK1 is running at 4MHz. Then LED0 toggles
 * two times and becomes OFF, system enters BACKUP mode.
 *
 * When PA04 is connected to low level, system wakes up from BACKUP mode, LED0
 * toggles four times. GCLK0/GCLK1 are running at 4MHz.
 *
 * \section asfdoc_sam0_power_basic_use_case_setup Quick Start
 *
 * \subsection asfdoc_sam0_power_basic_use_case_prereq Prerequisites
 * There are no prerequisites for this use case.
 *
 * \subsection asfdoc_sam0_power_basic_use_case_setup_code Code
 *
 * Copy-paste the following setup code to your user application:
 * \snippet quick_start/qs_power.c setup
 *
 * \subsection asfdoc_sam0_power_basic_use_case_setup_flow Workflow
 * -# Switch performance level to PL2.
 *    \snippet quick_start/qs_power.c switch_pl
 *
 * -# Configure GCLK0/GCLK1 output pin and extwakeup pin.
 *    \snippet quick_start/qs_power.c pin_mux
 *
 * -# Config external interrupt.
 *    \snippet quick_start/qs_power.c config_extint
 *
 * \section asfdoc_sam0_power_basic_use_case_main Use Case
 *
 * \subsection asfdoc_sam0_power_basic_use_case_main_code Code
 * Copy-paste the following code to your user application:
 * \snippet quick_start/qs_power.c  setup_init
 *
 * \subsection asfdoc_sam0_power_basic_use_case_main_flow Workflow
 * -# Check if the RESET is caused by external wakeup pin.
 *    \snippet quick_start/qs_power.c ext_wakeup
 * -# Check STANDBY mode and BACKUP mode.
 *    \snippet quick_start/qs_power.c backup_stanby_mode
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
