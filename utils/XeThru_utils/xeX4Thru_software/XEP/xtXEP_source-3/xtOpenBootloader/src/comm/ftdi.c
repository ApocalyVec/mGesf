/**
 * @file
 *
 * @brief USART interface functions
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <asf.h>
#include "conf_board.h"

#include <comm/comm_iface.h>
#include <comm/ftdi.h>


/** All interrupt mask. */
#define ALL_INTERRUPT_MASK  0xffffffff

static void enable_tx(void);

/**
 * @brief Initializes USART interface 
 */
void ftdi_init(void)
{	
	
	
	const sam_usart_opt_t usart_console_settings = {
		BOARD_USART_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
        //  This field is only used in IrDA mode. 
		
	};

    //  Enable the peripheral clock in the PMC. 
	sysclk_enable_peripheral_clock(BOARD_ID_USART);

    //  Configure USART in serial mode. 
	usart_init_rs232(BOARD_USART, &usart_console_settings,
			sysclk_get_cpu_hz()/2);//user pherfiral clock 

    //  Disable all the interrupts. 
	usart_disable_interrupt(BOARD_USART, ALL_INTERRUPT_MASK);

    //  Enable the receiver and transmitter. 
	usart_enable_tx(BOARD_USART);
	usart_enable_rx(BOARD_USART);

    //  Configure and enable interrupt of USART. 
	NVIC_EnableIRQ(USART_IRQn);
	
	usart_enable_interrupt(BOARD_USART, US_IDR_RXRDY);
	enable_tx();
}

/**
 * @brief Enables USART interrupt 
 */
static void enable_tx(void)
{
	usart_enable_interrupt(BOARD_USART, US_IER_TXRDY);
}


/**
 * \brief Interrupt handler for USART. Echo the bytes received and start the next receive.
 */
void USART_Handler(void)
{
	uint32_t c, ul_status;

    //  Read USART Status. 
	ul_status = usart_get_status(BOARD_USART);
    //  Transfer without PDC.
	if (ul_status & US_CSR_RXRDY) {
		usart_getchar(BOARD_USART, &c);
		CommIface_put(IFACE_IN, (uint8_t)c);
	} else if (ul_status & US_CSR_TXRDY) {
		if (!CommIface_is_empty(IFACE_OUT)) {
			usart_write(BOARD_USART, CommIface_get(IFACE_OUT));
		} else {
			usart_disable_interrupt(BOARD_USART, US_IDR_TXRDY);
		}
	}
	
	(uint32_t)BOARD_USART->US_IMR;
}
