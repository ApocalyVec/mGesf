/**
 * \file
 *
 * \brief Smart Card Standard ISO7816 driver.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef ISO7816_H_INCLUDED
#define ISO7816_H_INCLUDED

#include "board.h"
#include "usart.h"

#if defined(SMART_CARD_USING_GPIO)
#include "gpio.h"
#elif defined(SMART_CARD_USING_IOPORT)
#include "ioport.h"
#else
#error Smart Card service is not supported.
#endif

void iso7816_init(const usart_iso7816_opt_t *p_usart_opt, uint32_t ul_mck,
		uint32_t ul_rst_pin_idx);
void iso7816_icc_power_off(void);
uint16_t iso7816_xfr_block_tpdu_t0(const uint8_t *p_apdu, uint8_t *p_message,
		uint16_t us_length);
void iso7816_data_block_atr(uint8_t *p_atr, uint8_t *p_length);
uint8_t iso7816_get_reset_statuts(void);
void iso7816_cold_reset(void);
void iso7816_warm_reset(void);
void iso7816_decode_atr(uint8_t *p_atr);

#endif /* ISO7816_H_INCLUDED */

