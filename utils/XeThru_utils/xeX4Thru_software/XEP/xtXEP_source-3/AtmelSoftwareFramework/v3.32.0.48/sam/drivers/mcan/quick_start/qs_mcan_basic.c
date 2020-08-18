/**
 * \file
 *
 * \brief SAM MCAN basic Quick Start
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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
 *  \mainpage MCAN example
 *
 *  \par Purpose
 *
 *  This example demonstrates the basic functions of MCAN controller.
 *
 *  \par Requirements
 *
 *  This package can be used with two SAMV71 Xplained Ultra boards.
 *  The CAN1 in two board should be connected externally before running
 *  the example.
 *
 *  \par Description
 *
 *  In this example, one board sends messages over CAN bus to another board.
 *  The CAN message will display on the terminal window.
 *
 *  \par Usage
 *
 *  -# Build the program and download it into the evaluation board.
 *  -# On the computer, open and configure a terminal application
 *     (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *    - 115200 bauds
 *    - 8 bits of data
 *    - No parity
 *    - 1 stop bit
 *    - No flow control
 *  -# Connect CAN1 in two boards.
 *  -# Start the application.
 *  -# Upon startup, the application will output the following lines
 *     on the terminal window.
 *      \code
			"  -- Select the action:\r\n"
			"  0: Set standard filter ID 0: 0x45A, store into Rx buffer. \r\n"
			"  1: Set standard filter ID 1: 0x469, store into Rx FIFO 0. \r\n"
			"  2: Send standard message with ID: 0x45A and 4 byte data 0 to 3. \r\n"
			"  3: Send standard message with ID: 0x469 and 4 byte data 128 to 131. \r\n"
			"  4: Set extended filter ID 0: 0x100000A5, store into Rx buffer. \r\n"
			"  5: Set extended filter ID 1: 0x10000096, store into Rx FIFO 1. \r\n"
			"  6: Send extended message with ID: 0x100000A5 and 8 byte data 0 to 7. \r\n"
			"  7: Send extended message with ID: 0x10000096 and 8 byte data 128 to 135. \r\n"
			"  h: Display menu \r\n\r\n");
 \endcode
 *  -# Press '0' or '1' or '4'  or '5' key in the terminal window to configure one board to
 *     receive CAN message.
 *  -# Press '2' or '3' or '6'  or '7' key in the terminal window to configure another board to
 *     send CAN message. The message will be displayed on the terminal window.
 */

/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <string.h>
#include <conf_mcan.h>


/* module_inst */
static struct mcan_module mcan_instance;


/* mcan_filter_setting */
#define MCAN_RX_STANDARD_FILTER_INDEX_0    0
#define MCAN_RX_STANDARD_FILTER_INDEX_1    1
#define MCAN_RX_STANDARD_FILTER_ID_0     0x45A
#define MCAN_RX_STANDARD_FILTER_ID_0_BUFFER_INDEX     2
#define MCAN_RX_STANDARD_FILTER_ID_1     0x469
#define MCAN_RX_EXTENDED_FILTER_INDEX_0    0
#define MCAN_RX_EXTENDED_FILTER_INDEX_1    1
#define MCAN_RX_EXTENDED_FILTER_ID_0     0x100000A5
#define MCAN_RX_EXTENDED_FILTER_ID_0_BUFFER_INDEX     1
#define MCAN_RX_EXTENDED_FILTER_ID_1     0x10000096


/* mcan_transfer_message_setting */
#define MCAN_TX_BUFFER_INDEX    0
static uint8_t tx_message_0[CONF_MCAN_ELEMENT_DATA_SIZE];
static uint8_t tx_message_1[CONF_MCAN_ELEMENT_DATA_SIZE];


/* mcan_receive_message_setting */
static volatile uint32_t standard_receive_index = 0;
static volatile uint32_t extended_receive_index = 0;
static struct mcan_rx_element_fifo_0 rx_element_fifo_0;
static struct mcan_rx_element_fifo_1 rx_element_fifo_1;
static struct mcan_rx_element_buffer rx_element_buffer;

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief MCAN module initialization.
 *
 */
static void configure_mcan(void)
{
	uint32_t i;
	/* Initialize the memory. */
	for (i = 0; i < CONF_MCAN_ELEMENT_DATA_SIZE; i++) {
		tx_message_0[i] = i;
		tx_message_1[i] = i + 0x80;
	}

	/* Initialize the module. */
	struct mcan_config config_mcan;
	mcan_get_config_defaults(&config_mcan);
	mcan_init(&mcan_instance, MCAN_MODULE, &config_mcan);


	mcan_start(&mcan_instance);
	/* Enable interrupts for this MCAN module */
	irq_register_handler(MCAN1_IRQn, 1);
	mcan_enable_interrupt(&mcan_instance, MCAN_FORMAT_ERROR | MCAN_ACKNOWLEDGE_ERROR | MCAN_BUS_OFF);


}

/**
 * \brief set receive standard MCAN ID, dedicated buffer
 *
 */
static void mcan_set_standard_filter_0(void)
{
	struct mcan_standard_message_filter_element sd_filter;

	mcan_get_standard_message_filter_element_default(&sd_filter);
	sd_filter.S0.bit.SFID2 = MCAN_RX_STANDARD_FILTER_ID_0_BUFFER_INDEX;
	sd_filter.S0.bit.SFID1 = MCAN_RX_STANDARD_FILTER_ID_0;
	sd_filter.S0.bit.SFEC =
			MCAN_STANDARD_MESSAGE_FILTER_ELEMENT_S0_SFEC_STRXBUF_Val;

	mcan_set_rx_standard_filter(&mcan_instance, &sd_filter,
			MCAN_RX_STANDARD_FILTER_INDEX_0);
	mcan_enable_interrupt(&mcan_instance, MCAN_RX_BUFFER_NEW_MESSAGE);
}

/**
 * \brief set receive standard MCAN ID,FIFO buffer.
 *
 */
static void mcan_set_standard_filter_1(void)
{
	struct mcan_standard_message_filter_element sd_filter;

	mcan_get_standard_message_filter_element_default(&sd_filter);
	sd_filter.S0.bit.SFID1 = MCAN_RX_STANDARD_FILTER_ID_1;

	mcan_set_rx_standard_filter(&mcan_instance, &sd_filter,
			MCAN_RX_STANDARD_FILTER_INDEX_1);
	mcan_enable_interrupt(&mcan_instance, MCAN_RX_FIFO_0_NEW_MESSAGE);
}

/**
 * \brief set receive extended MCAN ID, dedicated buffer
 *
 */
static void mcan_set_extended_filter_0(void)
{
	struct mcan_extended_message_filter_element et_filter;

	mcan_get_extended_message_filter_element_default(&et_filter);
	et_filter.F0.bit.EFID1 = MCAN_RX_EXTENDED_FILTER_ID_0;
	et_filter.F0.bit.EFEC =
			MCAN_EXTENDED_MESSAGE_FILTER_ELEMENT_F0_EFEC_STRXBUF_Val;
	et_filter.F1.bit.EFID2 = MCAN_RX_EXTENDED_FILTER_ID_0_BUFFER_INDEX;

	mcan_set_rx_extended_filter(&mcan_instance, &et_filter,
			MCAN_RX_EXTENDED_FILTER_INDEX_0);
	mcan_enable_interrupt(&mcan_instance, MCAN_RX_BUFFER_NEW_MESSAGE);
}

/**
 * \brief set receive extended MCAN ID,FIFO buffer.
 *
 */
static void mcan_set_extended_filter_1(void)
{
	struct mcan_extended_message_filter_element et_filter;

	mcan_get_extended_message_filter_element_default(&et_filter);
	et_filter.F0.bit.EFID1 = MCAN_RX_EXTENDED_FILTER_ID_1;

	mcan_set_rx_extended_filter(&mcan_instance, &et_filter,
			MCAN_RX_EXTENDED_FILTER_INDEX_1);
	mcan_enable_interrupt(&mcan_instance, MCAN_RX_FIFO_1_NEW_MESSAGE);
}

/**
 * \brief send standard MCAN message,
 *
 *\param id_value standard MCAN ID
 *\param *data  content to be sent
 *\param data_length data length code
 */
static void mcan_send_standard_message(uint32_t id_value, uint8_t *data,
		uint32_t data_length)
{
	uint32_t i;
	struct mcan_tx_element tx_element;

	mcan_get_tx_buffer_element_defaults(&tx_element);
	tx_element.T0.reg |= MCAN_TX_ELEMENT_T0_STANDARD_ID(id_value);
	tx_element.T1.bit.DLC = data_length;
	for (i = 0; i < data_length; i++) {
		tx_element.data[i] = *data;
		data++;
	}

	mcan_set_tx_buffer_element(&mcan_instance, &tx_element,
			MCAN_TX_BUFFER_INDEX);
	mcan_tx_transfer_request(&mcan_instance, 1 << MCAN_TX_BUFFER_INDEX);
}

/**
 * \brief send extended MCAN message,
 *
 *\param id_value extended MCAN ID
 *\param *data  content to be sent
 *\param data_length data length code
 */
static void mcan_send_extended_message(uint32_t id_value, uint8_t *data,
		uint32_t data_length)
{
	uint32_t i;
	struct mcan_tx_element tx_element;

	mcan_get_tx_buffer_element_defaults(&tx_element);
	tx_element.T0.reg |= MCAN_TX_ELEMENT_T0_EXTENDED_ID(id_value) |
			MCAN_TX_ELEMENT_T0_XTD;
	tx_element.T1.bit.DLC = data_length;
	for (i = 0; i < data_length; i++) {
		tx_element.data[i] = *data;
		data++;
	}

	mcan_set_tx_buffer_element(&mcan_instance, &tx_element,
			MCAN_TX_BUFFER_INDEX);
	mcan_tx_transfer_request(&mcan_instance, 1 << MCAN_TX_BUFFER_INDEX);
}

/**
 * \brief Interrupt handler for MCAN,
 *   inlcuding RX,TX,ERROR and so on processes.
 */
void MCAN1_Handler(void)
{
	volatile uint32_t status, i, rx_buffer_index;
	status = mcan_read_interrupt_status(&mcan_instance);

	if (status & MCAN_RX_BUFFER_NEW_MESSAGE) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_RX_BUFFER_NEW_MESSAGE);
		for (i = 0; i < CONF_MCAN0_RX_BUFFER_NUM; i++) {
			if (mcan_rx_get_buffer_status(&mcan_instance, i)) {
				rx_buffer_index = i;
				mcan_rx_clear_buffer_status(&mcan_instance, i);
				mcan_get_rx_buffer_element(&mcan_instance, &rx_element_buffer,
				rx_buffer_index);
				if (rx_element_buffer.R0.bit.XTD) {
					printf("\n\r Extended message received in Rx buffer. The received data is: \r\n");
				} else {
					printf("\n\r Standard message received in Rx buffer. The received data is: \r\n");
				}
				for (i = 0; i < rx_element_buffer.R1.bit.DLC; i++) {
					printf("  %d",rx_element_buffer.data[i]);
				}
				printf("\r\n\r\n");
			}
		}
	}

	if (status & MCAN_RX_FIFO_0_NEW_MESSAGE) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_RX_FIFO_0_NEW_MESSAGE);
		mcan_get_rx_fifo_0_element(&mcan_instance, &rx_element_fifo_0,
				standard_receive_index);
		mcan_rx_fifo_acknowledge(&mcan_instance, 0,
				standard_receive_index);
		standard_receive_index++;
		if (standard_receive_index == CONF_MCAN0_RX_FIFO_0_NUM) {
			standard_receive_index = 0;
		}

		printf("\n\r Standard message received in FIFO 0. The received data is: \r\n");
		for (i = 0; i < rx_element_fifo_0.R1.bit.DLC; i++) {
			printf("  %d",rx_element_fifo_0.data[i]);
		}
		printf("\r\n\r\n");
	}

	if (status & MCAN_RX_FIFO_1_NEW_MESSAGE) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_RX_FIFO_1_NEW_MESSAGE);
		mcan_get_rx_fifo_1_element(&mcan_instance, &rx_element_fifo_1,
				extended_receive_index);
		mcan_rx_fifo_acknowledge(&mcan_instance, 0,
				extended_receive_index);
		extended_receive_index++;
		if (extended_receive_index == CONF_MCAN0_RX_FIFO_1_NUM) {
			extended_receive_index = 0;
		}

		printf("\n\r Extended message received in FIFO 1. The received data is: \r\n");
		for (i = 0; i < rx_element_fifo_1.R1.bit.DLC; i++) {
			printf("  %d",rx_element_fifo_1.data[i]);
		}
		printf("\r\n\r\n");
	}

	if (status & MCAN_BUS_OFF) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_BUS_OFF);
		mcan_stop(&mcan_instance);
		printf(": MCAN bus off error, re-initialization. \r\n\r\n");
		configure_mcan();
	}

	if (status & MCAN_ACKNOWLEDGE_ERROR) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_ACKNOWLEDGE_ERROR);
		printf("Protocol ACK error, please double check the clock in two boards. \r\n\r\n");
	}

	if (status & MCAN_FORMAT_ERROR) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_FORMAT_ERROR);
		printf("Protocol format error, please double check the clock in two boards. \r\n\r\n");
	}
}

/**
 * \brief display configuration menu.
 */
static void display_menu(void)
{
	printf("Menu :\r\n"
			"  -- Select the action:\r\n"
			"  0: Set standard filter ID 0: 0x45A, store into Rx buffer. \r\n"
			"  1: Set standard filter ID 1: 0x469, store into Rx FIFO 0. \r\n"
			"  2: Send standard message with ID: 0x45A and 4 byte data 0 to 3. \r\n"
			"  3: Send standard message with ID: 0x469 and 4 byte data 128 to 131. \r\n"
			"  4: Set extended filter ID 0: 0x100000A5, store into Rx buffer. \r\n"
			"  5: Set extended filter ID 1: 0x10000096, store into Rx FIFO 1. \r\n"
			"  6: Send extended message with ID: 0x100000A5 and 8 byte data 0 to 7. \r\n"
			"  7: Send extended message with ID: 0x10000096 and 8 byte data 128 to 135. \r\n"
			"  h: Display menu \r\n\r\n");
}

int main(void)
{
	uint8_t key;

	sysclk_init();
	board_init();

	configure_console();
	configure_mcan();

	display_menu();



	while(1) {
		scanf("%c", (char *)&key);

		switch (key) {
		case 'h':
			display_menu();
			break;

		case '0':
			printf("  0: Set standard filter ID 0: 0x45A, store into Rx buffer. \r\n");
			mcan_set_standard_filter_0();
			break;

		case '1':
			printf("  1: Set standard filter ID 1: 0x469, store into Rx FIFO 0. \r\n");
			mcan_set_standard_filter_1();
			break;

		case '2':
			printf("  2: Send standard message with ID: 0x45A and 4 byte data 0 to 3. \r\n");
			mcan_send_standard_message(MCAN_RX_STANDARD_FILTER_ID_0, tx_message_0,
					CONF_MCAN_ELEMENT_DATA_SIZE / 2);
			break;

		case '3':
			printf("  3: Send standard message with ID: 0x469 and 4 byte data 128 to 131. \r\n");
			mcan_send_standard_message(MCAN_RX_STANDARD_FILTER_ID_1, tx_message_1,
					CONF_MCAN_ELEMENT_DATA_SIZE / 2);
			break;

		case '4':
			printf("  4: Set extended filter ID 0: 0x100000A5, store into Rx buffer. \r\n");
			mcan_set_extended_filter_0();
			break;

		case '5':
			printf("  5: Set extended filter ID 1: 0x10000096, store into Rx FIFO 1. \r\n");
			mcan_set_extended_filter_1();
			break;

		case '6':
			printf("  6: Send extended message with ID: 0x100000A5 and 8 byte data 0 to 7. \r\n");
			mcan_send_extended_message(MCAN_RX_EXTENDED_FILTER_ID_0, tx_message_0,
					CONF_MCAN_ELEMENT_DATA_SIZE);
			break;

		case '7':
			printf("  7: Send extended message with ID: 0x10000096 and 8 byte data 128 to 135. \r\n");
			mcan_send_extended_message(MCAN_RX_EXTENDED_FILTER_ID_1, tx_message_1,
					CONF_MCAN_ELEMENT_DATA_SIZE);
			break;

		default:
			break;
		}
	}

}
