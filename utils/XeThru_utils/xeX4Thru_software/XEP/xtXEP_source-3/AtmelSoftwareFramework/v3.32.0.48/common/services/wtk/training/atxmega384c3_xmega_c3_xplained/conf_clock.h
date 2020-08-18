/**
 * \file
 *
 * \brief Clock system configuration
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef CONF_CLOCK_H
#define CONF_CLOCK_H

/* #define CONFIG_SYSCLK_SOURCE          SYSCLK_SRC_RC2MHZ */
#define CONFIG_SYSCLK_SOURCE     SYSCLK_SRC_RC32MHZ
/* #define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_RC32KHZ */
/* #define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_XOSC */
/* #define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_PLL */

/* Fbus = Fsys / (2 ^ BUS_div) */
#define CONFIG_SYSCLK_PSADIV     SYSCLK_PSADIV_1
#define CONFIG_SYSCLK_PSBCDIV    SYSCLK_PSBCDIV_1_1

/* #define CONFIG_PLL0_SOURCE          PLL_SRC_XOSC */
/* #define CONFIG_PLL0_SOURCE          PLL_SRC_RC2MHZ */
/* #define CONFIG_PLL0_SOURCE          PLL_SRC_RC32MHZ */

/* Fpll = (Fclk * PLL_mul) / PLL_div */
/* #define CONFIG_PLL0_MUL             (24000000UL / BOARD_XOSC_HZ) */
/* #define CONFIG_PLL0_DIV             1 */

/* External oscillator frequency range */
/* 0.4 to 2 MHz frequency range */
/* define CONFIG_XOSC_RANGE XOSC_RANGE_04TO2 */
/* 2 to 9 MHz frequency range */
/* define CONFIG_XOSC_RANGE XOSC_RANGE_2TO9 */
/* 9 to 12 MHz frequency range */
/* define CONFIG_XOSC_RANGE XOSC_RANGE_9TO12 */
/* 12 to 16 MHz frequency range */
/* define CONFIG_XOSC_RANGE XOSC_RANGE_12TO16 */

/* DFLL autocalibration */
/* #define CONFIG_OSC_AUTOCAL_RC2MHZ_REF_OSC  OSC_ID_RC32KHZ */
/* #define CONFIG_OSC_AUTOCAL_RC32MHZ_REF_OSC OSC_ID_XOSC */

/* ***************************************************************
 * **                  CONFIGURATION WITH USB                   **
 * ***************************************************************
 * The following clock configuraiton can be used for USB operation
 * It allows to operate USB using On-Chip RC oscillator at 48MHz
 * The RC oscillator is calibrated via USB Start Of Frame
 * Clk USB     = 48MHz (used by USB)
 * Clk sys     = 48MHz
 * Clk cpu/per = 24MHz */

#define CONFIG_USBCLK_SOURCE                USBCLK_SRC_RCOSC
/* #define CONFIG_OSC_RC32_CAL                 48000000UL */
/* #define CONFIG_OSC_AUTOCAL_RC32MHZ_REF_OSC  OSC_ID_USBSOF */
/* #define CONFIG_SYSCLK_SOURCE                SYSCLK_SRC_RC32MHZ */
/* #define CONFIG_SYSCLK_PSADIV                SYSCLK_PSADIV_2 */
/* #define CONFIG_SYSCLK_PSBCDIV               SYSCLK_PSBCDIV_1_1 */

/* ********** END OF USB CLOCK CONFIGURATION ********************** */

#endif /* CONF_CLOCK_H */
