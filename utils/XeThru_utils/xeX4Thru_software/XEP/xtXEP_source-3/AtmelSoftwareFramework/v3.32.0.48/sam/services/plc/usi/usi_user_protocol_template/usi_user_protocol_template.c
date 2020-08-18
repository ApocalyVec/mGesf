/**
 * \file
 *
 * \brief Template of a user-defined protocol support for PLC USI service
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <compiler.h>
#include <asf.h>
#include "conf_example.h"

/**
 * \mainpage
 *
 * \section intro Introduction
 * This an example of how to use \ref usi.h "PLC Universal Serial
 * Interface service" to communicate with the target through a
 * user-defined protocol.
 *
 * In this file, a template for the serialization of a user-defined
 * protocol is provided. It can be used as a basis to further implement
 * a given desired functionality.
 *
 * \section files Main Files
 * - \ref usi_user_protocol_template.c
 * - \ref conf_usi.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_buart_if.h
 * - \ref conf_busart_if.h
 * - \ref conf_uart_serial.h
 * - \ref conf_example.h
 *
 * \section device_info Device Info
 * SAM4CP and SAM4S devices can be used.
 *
 * \section description Description
 * This template can be used as basis to implement USI support
 * for any user-defined protocol. This is done through the function:
 *
 *   uint8_t serial_if_user_def_api_parser(uint8_t *puc_rx_msg, uint16_t
 *   us_len);
 *
 *   This function is used as a callback by USI whenever a message with:
 *   TYPE_PROTOCOL = PROTOCOL_USER_DEFINED is received, in order to process it.
 *   By extending the capabilities of that function to the user-defined
 *   protocol, the USI is provided with the intelligence to support it.
 *
 *   This function receives two parameters:
 *
 *  uint8_t *puc_rx_msg  Pointer to the message payload.
 *  uint16_t us_len      Length of the payload.
 *
 *   As a basis of the user-defined protocol, two primitives have been defined:
 *
 *  SERIAL_IF_USER_DEF_GET_CMD: this primitive gets the value in a given
 *  memory address of the target.
 *  SERIAL_IF_USER_DEF_SET_CMD: this primitive stores a value in a given
 *  memory address of the target.
 *
 *   Respectively, the target will respond to them with:
 *
 *  SERIAL_IF_USER_DEF_GET_CMD_RSP: this primitive contains the value stored
 *  in the requested memory address of the target.
 *  SERIAL_IF_USER_DEF_SET_CMD_RSP: this primitive simply acknowledges the
 *  execution of the set command.
 *
 *   The message format of this protocol template is defined as:
 *
 *   Read command:
 *
 * [ COMMAND ][ ADDRESS ]
 *
 * <--1Byte--><--4 Bytes-->
 *
 *   Write command:
 *
 * [ COMMAND ][ ADDRESS ][ VALUE ]
 *
 * <--1Byte--><--4 Bytes--><--4 Bytes-->
 *
 *   The list of supported primitives can be extended in the function
 *   serial_if_user_def_api_parser() and in the typedef serial_if_user_def_cmd_t
 *   as needed by the user-defined protocol.
 *
 * \section dependencies Dependencies
 * This template depends on the following modules:
 * - \ref usi.h "PLC Universal Serial Interface service"
 *
 * \section compinfo Compilation information
 * This software was written for the IAR for ARM. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */

uint8_t serial_if_user_def_api_parser(uint8_t *puc_rx_msg, uint16_t us_len);

static bool b_led_swap = false;

/**
 * \name Response buffer
 */
/* @{ */
static uint8_t uc_serial_rsp_buf[5];
/* @} */

/* ! \name Data structure to communicate with USI service */
/* @{ */
static x_usi_serial_cmd_params_t x_phy_serial_msg;
/* @} */

/* ! \name Serial interface user defined protocol message identifiers */
/* @{ */
typedef enum {
	SERIAL_IF_USER_DEF_GET_CMD,
	SERIAL_IF_USER_DEF_GET_CMD_RSP,
	SERIAL_IF_USER_DEF_SET_CMD,
	SERIAL_IF_USER_DEF_SET_CMD_RSP,
	SERIAL_IF_USER_DEF_INVALID
} serial_if_user_def_cmd_t;

/**
 * \brief Configure the hardware.
 */
static void prvSetupHardware(void)
{
	/* ASF function to setup clocking. */
	sysclk_init();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS);

	/* Atmel library function to setup for the evaluation kit being used. */
	board_init();
}

/**
 * \brief Main code entry point.
 */
int main( void )
{
	/* Prepare the hardware */
	prvSetupHardware();

	/* Usi initialization */
	usi_init();

	/* Initialize USI communication structure */
	x_phy_serial_msg.uc_protocol_type = PROTOCOL_USER_DEFINED;

	while (1) {
		usi_process();
		/* blink led 0 */
		if (b_led_swap) {
			b_led_swap = false;
#if (BOARD == SAM4CMP_DB || BOARD == SAM4CMS_DB)
			LED_Toggle(LED4);
#else
			LED_Toggle(LED0);
#endif
		}
	}
}

/**
 * \brief Task to manage the serialization of the user defined protocol.
 *
 * \param   puc_rx_msg  Pointer to the message received through USI
 * \param   us_len      Length of the received message
 *
 * \retval true if there is no error
 * \retval false if length is invalid or serial command is wrong
 */
uint8_t serial_if_user_def_api_parser(uint8_t *puc_rx_msg, uint16_t us_len)
{
	uint8_t uc_serial_if_cmd;
	uint32_t uc_value;
	uint16_t us_serial_response_len;

	uint8_t *puc_rx;
	uint32_t ul_address;
	uint32_t *pul_address;

	/* Protection for invalid us_length */
	if (!us_len) {
		return false;
	}

	/* Process received message */
	uc_serial_if_cmd = puc_rx_msg[0];
	puc_rx = &puc_rx_msg[1];
	us_serial_response_len = 0;

	switch (uc_serial_if_cmd) {
	/* GET command */
	case SERIAL_IF_USER_DEF_GET_CMD:
		ul_address = ((uint32_t)*puc_rx++) << 24;
		ul_address |= ((uint32_t)*puc_rx++) << 16;
		ul_address |= ((uint32_t)*puc_rx++) << 8;
		ul_address |= ((uint32_t)*puc_rx);

		/* Read the requested data */
		uc_value = *((uint32_t *)ul_address);

		/* Build response */
		uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_USER_DEF_GET_CMD_RSP;
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(uc_value >> 24);
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(uc_value >> 16);
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(uc_value >> 8);
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(uc_value);

		/* Set USI parameters and send command */
		x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
		x_phy_serial_msg.us_len = us_serial_response_len;
		usi_send_cmd(&x_phy_serial_msg);
		break;

	/* SET command */
	case SERIAL_IF_USER_DEF_SET_CMD:
		ul_address = ((uint32_t)*puc_rx++) << 24;
		ul_address |= ((uint32_t)*puc_rx++) << 16;
		ul_address |= ((uint32_t)*puc_rx++) << 8;
		ul_address |= ((uint32_t)*puc_rx++);

		uc_value = ((uint32_t)*puc_rx++) << 24;
		uc_value |= ((uint32_t)*puc_rx++) << 16;
		uc_value |= ((uint32_t)*puc_rx++) << 8;
		uc_value |= ((uint32_t)*puc_rx);

		/* Write the data in the requested address */
		pul_address = (uint32_t *)ul_address;
		*pul_address = uc_value;

		/* Build response */
		uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_USER_DEF_SET_CMD_RSP;

		/* Set USI parameters and send command */
		x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
		x_phy_serial_msg.us_len = us_serial_response_len;
		usi_send_cmd(&x_phy_serial_msg);
		break;

	default:
		return false;
	}

	return true;
}
