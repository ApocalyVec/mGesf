#include <stdint.h>
#include <stdlib.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#include <asf.h>
#pragma GCC diagnostic pop
#include "freertos_xdma.h"

#define XDMA_CH_CNT					(24)

static void (*handlers[XDMA_CH_CNT])(void);


void freertos_xdma_init(void)
{
	for (uint8_t ch = 0; ch < XDMA_CH_CNT; ++ch) {
		handlers[ch] = NULL;
	}

	NVIC_ClearPendingIRQ(XDMAC_IRQn);
	NVIC_SetPriority(XDMAC_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(XDMAC_IRQn);
}


void freertos_xdma_register(uint8_t ch, void (*fun)(void))
{
	cpu_irq_enter_critical();
	if (ch < XDMA_CH_CNT) {
		handlers[ch] = fun;
	}
	cpu_irq_leave_critical();
}


void freertos_xdma_free(uint8_t ch)
{
	cpu_irq_enter_critical();
	if (ch < XDMA_CH_CNT) {
		handlers[ch] = NULL;
		XDMAC->XDMAC_GID = 1 << ch;
	}
	cpu_irq_leave_critical();}


/*
 * Individual interrupt handlers follow from here.  Each individual interrupt
 * handler calls the common interrupt handler.
 */

void XDMAC_Handler(void)
{
	uint32_t status = XDMAC->XDMAC_GIS;
	status &= XDMAC->XDMAC_GIM;

	uint8_t ch = 0;
	while (status) {
		if (status & 0x1) {
			void (*fun)(void) = handlers[ch];
			if (fun != NULL) {
				fun();
			} else {
				/*Disable unhandled interrupts*/
				XDMAC->XDMAC_GID = 1 << ch;
			}
		}

		++ch;
		status >>= 1;
	}
}


#if 0
/**
 * \file
 *
 * \brief FreeRTOS Peripheral Control API For the SPI
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
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

/* Standard includes. */
#include <string.h>

/* ASF includes. */
#include "xdmac.h"
#include "qspi.h"
#include "freertos_spi_master.h"
#include "freertos_peripheral_control_private.h"
#include "freertos_xdma.h"


/* Every bit in the interrupt mask. */
#define MASK_ALL_INTERRUPTS                         (0xffffffffUL)

#define XDMA_CH_SPI_RX								(0)
#define XDMA_CH_SPI_TX								(1)

/*
 * A common interrupt handler used by all the SPI peripherals.
 */
static void local_spi_handler(const portBASE_TYPE spi_index);

/* Structures to manage the DMA control for both Rx and Tx transactions. */
static freertos_dma_event_control_t tx_dma_control;
static freertos_dma_event_control_t rx_dma_control;

/* Used to detect the init function being called after any part of the DMA
control structure has already been initialilsed - indicating an error condition
as init should only be called once per port. */
static const freertos_dma_event_control_t null_dma_control = {NULL, NULL};


static void xdma_rx_done(void);
static void xdma_tx_done(void);


/**
 * \ingroup freertos_spi_peripheral_control_group
 * \brief Initializes the FreeRTOS ASF SPI master driver for the specified SPI port.
 *
 * freertos_spi_master_init() is an ASF specific FreeRTOS driver function.  It
 * must be called before any other ASF specific FreeRTOS driver functions
 * attempt to access the same SPI port.
 *
 * If freertos_driver_parameters->operation_mode equals SPI_MASTER then
 * freertos_spi_master_init() will configure the SPI port for master mode
 * operation and enable the peripheral.  If
 * freertos_driver_parameters->operation_mode equals any other value then
 * freertos_spi_master_init() will not take any action.
 *
 * Other ASF SPI functions, such as those to set the SPI clock rate and other
 * bus parameters, can be called after freertos_spi_master_init() has completed
 * successfully.
 *
 * The FreeRTOS ASF driver both installs and handles the SPI PDC interrupts.
 * Users do not need to concern themselves with interrupt handling, and must
 * not install their own interrupt handler.
 *
 * This driver is provided with an application note, and an example project that
 * demonstrates the use of this function.
 *
 * \param p_spi    The SPI peripheral being initialized.
 * \param freertos_driver_parameters    Defines the driver behavior.  See the
 *    freertos_peripheral_options_t documentation, and the application note that
 *    accompanies the ASF specific FreeRTOS functions.
 *
 * \return If the initialization completes successfully then a handle that can
 *     be used with FreeRTOS SPI read and write functions is returned.  If
 *     the initialisation fails then NULL is returned.
 */
freertos_spi_if freertos_spi_master_init(void)
{
	/* This function must be called exactly once per supported spi.  Check it
	   has not been called	before. */
	configASSERT(memcmp((void *) &tx_dma_control,
			&null_dma_control, sizeof(null_dma_control)) == 0);
	configASSERT(memcmp((void *) &rx_dma_control,
			&null_dma_control, sizeof(null_dma_control)) == 0);

	/* Ensure everything is disabled before configuration. */
	qspi_disable(QSPI);
	xdmac_disable_interrupt(XDMAC, (1 << XDMA_CH_SPI_RX) | (1 << XDMA_CH_SPI_TX));
	xdmac_channel_disable(XDMAC, XDMA_CH_SPI_RX);
	xdmac_channel_disable(XDMAC, XDMA_CH_SPI_TX);

	qspi_disable_interrupt(QSPI, MASK_ALL_INTERRUPTS);

	sysclk_enable_peripheral_clock(ID_QSPI);

	qspi_reset(QSPI);
	//qspi_disable_mode_fault_detect(QSPI);
	qspi_disable_loopback(QSPI);
	//qspi_set_peripheral_chip_select_value(QSPI, DEFAULT_CHIP_ID);
	//qspi_set_fixed_peripheral_select(QSPI);
	//qspi_disable_peripheral_select_decode(QSPI);
	//qspi_set_delay_between_chip_select(QSPI, CONFIG_SPI_MASTER_DELAY_BCS);

	/* Create any required peripheral access mutexes and transaction complete
	   semaphores.  This peripheral is half duplex so only a single access
	   mutex is required. */
	create_peripheral_control_semaphores(( WAIT_TX_COMPLETE | WAIT_RX_COMPLETE),
				&tx_dma_control,
				&rx_dma_control);

	freertos_xdma_register(XDMA_CH_SPI_RX, xdma_rx_done);
	freertos_xdma_register(XDMA_CH_SPI_TX, xdma_tx_done);

	/* Finally, enable the peripheral. */
	qspi_enable(QSPI);

	return (freertos_spi_if)QSPI;
}

/**
 * \ingroup freertos_spi_peripheral_control_group
 * \brief Initiate a completely asynchronous multi-byte write operation on an
 * SPI peripheral.
 *
 * freertos_spi_write_packet_async() is an ASF specific FreeRTOS driver function.
 * It configures the SPI peripheral DMA controller (PDC) to transmit data on the
 * SPI port, then returns.  freertos_spi_write_packet_async() does not wait for
 * the transmission to complete before returning.
 *
 * The FreeRTOS SPI driver is initialized using a call to
 * freertos_spi_master_init().  The freertos_driver_parameters.options_flags
 * parameter passed into the initialization function defines the driver behavior.
 * freertos_spi_write_packet_async() can only be used if the
 * freertos_driver_parameters.options_flags parameter passed to the initialization
 * function had the WAIT_TX_COMPLETE bit clear.
 *
 * freertos_spi_write_packet_async() is an advanced function and readers are
 * recommended to also reference the application note and examples that
 * accompany the FreeRTOS ASF drivers.  freertos_spi_write_packet() is a version
 * that does not exit until the PDC transfer is complete, but still allows other
 * RTOS tasks to execute while the transmission is in progress.
 *
 * The FreeRTOS ASF driver both installs and handles the SPI PDC interrupts.
 * Users do not need to concern themselves with interrupt handling, and must
 * not install their own interrupt handler.
 *
 * \param p_spi    The handle to the SPI peripheral returned by the
 *     freertos_spi_master_init() call used to initialise the peripheral.
 * \param data    A pointer to the data to be transmitted.
 * \param len    The number of bytes to transmit.
 * \param block_time_ticks    The FreeRTOS ASF SPI driver is initialized using a
 *     call to freertos_spi_master_init().  The
 *     freertos_driver_parameters.options_flags parameter passed to the
 *     initialization function defines the driver behavior.  If
 *     freertos_driver_parameters.options_flags had the USE_TX_ACCESS_MUTEX bit
 *     set, then the driver will only write to the SPI peripheral if it has
 *     first gained exclusive access to it.  block_time_ticks specifies the
 *     maximum amount of time the driver will wait to get exclusive access
 *     before aborting the write operation.  Other tasks will execute during any
 *     waiting time.  block_time_ticks is specified in RTOS tick periods.  To
 *     specify a block time in milliseconds, divide the milliseconds value by
 *     portTICK_RATE_MS, and pass the result in block_time_ticks.
 *     portTICK_RATE_MS is defined by FreeRTOS.
 * \param notification_semaphore    The RTOS task that calls the transmit
 *     function exits the transmit function as soon as the transmission starts.
 *     The data being transmitted by the PDC must not be modified until after
 *     the transmission has completed.  The PDC interrupt (handled internally by
 *     the FreeRTOS ASF driver) 'gives' the semaphore when the PDC transfer
 *     completes.  The notification_semaphore therefore provides a mechanism for
 *     the calling task to know when the PDC has finished accessing the data.
 *     The calling task can call standard FreeRTOS functions to block on the
 *     semaphore until the PDC interrupt occurs.  Other RTOS tasks will execute
 *     while the the calling task is in the Blocked state.  The semaphore must
 *     be created using the FreeRTOS vSemaphoreCreateBinary() API function
 *     before it is used as a parameter.
 *
 * \return     ERR_INVALID_ARG is returned if an input parameter is invalid.
 *     ERR_TIMEOUT is returned if block_time_ticks passed before exclusive
 *     access to the SPI peripheral could be obtained.  STATUS_OK is returned if
 *     the PDC was successfully configured to perform the SPI write operation.
 */
status_code_t freertos_spi_write_packet_async(freertos_spi_if p_spi,
		const uint8_t *data, size_t len, portTickType block_time_ticks,
		xSemaphoreHandle notification_semaphore)
{
	status_code_t return_value;
	xdmac_channel_config_t tx_packet;

	return_value = freertos_obtain_peripheral_access_mutex(&tx_dma_control, &block_time_ticks);

	if (return_value == STATUS_OK) {
		freertos_start_pdc_tx(&tx_dma_control, notification_semaphore);

		irqflags_t status = cpu_irq_save();

		tx_packet.mbr_ubc = len;

		tx_packet.mbr_sa = (uint32_t)data;
		tx_packet.mbr_da = (uint32_t)&(QSPI->QSPI_TDR);
		tx_packet.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN |
					XDMAC_CC_MBSIZE_SINGLE |
					XDMAC_CC_DSYNC_MEM2PER |
					XDMAC_CC_CSIZE_CHK_1 |
					XDMAC_CC_DWIDTH_BYTE |
					XDMAC_CC_SIF_AHB_IF0 |
					XDMAC_CC_DIF_AHB_IF1 |
					XDMAC_CC_SAM_INCREMENTED_AM |
					XDMAC_CC_DAM_FIXED_AM |
					XDMAC_CC_PERID(XDAMC_CHANNEL_HWID_QSPI_TX);

		tx_packet.mbr_bc = 0;
		tx_packet.mbr_ds =  0;
		tx_packet.mbr_sus = 0;
		tx_packet.mbr_dus = 0;

		xdmac_configure_transfer(XDMAC, XDMA_CH_SPI_TX, &tx_packet);
		xdmac_channel_set_descriptor_control(XDMAC, XDMA_CH_SPI_TX, 0);
		xdmac_channel_enable_interrupt(XDMAC, XDMA_CH_SPI_TX, XDMAC_CIE_BIE |
					XDMAC_CIE_RBIE  |
					XDMAC_CIE_WBIE  |
					XDMAC_CIE_ROIE);

		/*Start*/
		xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_SPI_TX);
		xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_SPI_TX);
		xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_SPI_TX);
		xdmac_enable_interrupt(XDMAC, XDMA_CH_SPI_TX);
		xdmac_channel_enable(XDMAC, XDMA_CH_SPI_TX);

		__DMB();

		cpu_irq_restore(status);

		return_value = freertos_optionally_wait_transfer_completion(&tx_dma_control,
					notification_semaphore, block_time_ticks);
	} else {
		return_value = ERR_INVALID_ARG;
	}

	return return_value;
}

/**
 * \ingroup freertos_spi_peripheral_control_group
 * \brief Initiate a completely asynchronous multi-byte read operation on an SPI
 * peripheral.
 *
 * freertos_spi_read_packet_async() is an ASF specific FreeRTOS driver function.
 * It configures the SPI peripheral DMA controller (PDC) to read data from the
 * SPI port, then returns.  freertos_spi_read_packet_async() does not wait for
 * the reception to complete before returning.
 *
 * The FreeRTOS ASF SPI driver is initialized using a call to
 * freertos_spi_master_init().  The freertos_driver_parameters.options_flags
 * parameter passed into the initialization function defines the driver behavior.
 * freertos_spi_read_packet_async() can only be used if the
 * freertos_driver_parameters.options_flags parameter passed to the initialization
 * function had the WAIT_RX_COMPLETE bit clear.
 *
 * freertos_spi_read_packet_async() is an advanced function and readers are
 * recommended to also reference the application note and examples that
 * accompany the FreeRTOS ASF drivers.  freertos_spi_read_packet() is a version
 * that does not exit until the PDC transfer is complete, but still allows other
 * RTOS tasks to execute while the transmission is in progress.
 *
 * The FreeRTOS ASF driver both installs and handles the SPI PDC interrupts.
 * Users do not need to concern themselves with interrupt handling, and must
 * not install their own interrupt handler.
 *
 * \param p_spi    The handle to the SPI port returned by the
 *     freertos_spi_master_init() call used to initialise the port.
 * \param data    A pointer to the buffer into which received data is to be
 *     written.
 * \param len    The number of bytes to receive.
 * \param block_time_ticks    The FreeRTOS ASF SPI driver is initialized using a
 *     call to freertos_spi_master_init().  The
 *     freertos_driver_parameters.options_flags parameter passed to the
 *     initialization function defines the driver behavior.  If
 *     freertos_driver_parameters.options_flags had the USE_RX_ACCESS_MUTEX bit
 *     set, then the driver will only read from the SPI peripheral if it has
 *     first gained exclusive access to it.  block_time_ticks specifies the
 *     maximum amount of time the driver will wait to get exclusive access
 *     before aborting the read operation.  Other tasks will execute during any
 *     waiting time.  block_time_ticks is specified in RTOS tick periods.  To
 *     specify a block time in milliseconds, divide the milliseconds value by
 *     portTICK_RATE_MS, and pass the result in block_time_ticks.
 *     portTICK_RATE_MS is defined by FreeRTOS.
 * \param notification_semaphore    The RTOS task that calls the receive
 *     function exits the receive function as soon as the reception starts.
 *     The data being received by the PDC cannot normally be processed until
 *     after the reception has completed.  The PDC interrupt (handled internally
 *     by the FreeRTOS ASF driver) 'gives' the semaphore when the PDC transfer
 *     completes.  The notification_semaphore therefore provides a mechanism for
 *     the calling task to know when the PDC has read the requested number of
 *     bytes.  The calling task can call standard FreeRTOS functions to block on
 *     the semaphore until the PDC interrupt occurs.  Other RTOS tasks will
 *     execute while the the calling task is in the Blocked state.  The
 *     semaphore must be created using the FreeRTOS vSemaphoreCreateBinary() API
 *     function before it is used as a parameter.
 *
 * \return     ERR_INVALID_ARG is returned if an input parameter is invalid.
 *     ERR_TIMEOUT is returned if block_time_ticks passed before exclusive
 *     access to the SPI peripheral could be obtained.  STATUS_OK is returned if
 *     the PDC was successfully configured to perform the SPI read operation.
 */
status_code_t freertos_spi_read_packet_async(freertos_spi_if p_spi,
		uint8_t *data, uint32_t len, portTickType block_time_ticks,
		xSemaphoreHandle notification_semaphore)
{
	status_code_t return_value;
	xdmac_channel_config_t rx_packet, tx_packet;
	volatile uint16_t junk_value;

	if (len == 0) {
		return STATUS_OK;
	}

	/* Because the peripheral is half duplex, there is only one access mutex
	   and the rx uses the tx mutex. */
	return_value = freertos_obtain_peripheral_access_mutex(&tx_dma_control, &block_time_ticks);

	if (return_value == STATUS_OK) {
		/* Data must be sent for data to be received.  Set the receive
		buffer to all 0xffs so it can also be used as the send buffer. */
		//memset((void *)data, 0xff, (size_t)len); // OJE: Removed in order to send while receiving.

		/* Ensure Rx is already empty. */
		while(qspi_is_rx_full(QSPI) != 0) {
			junk_value = QSPI->QSPI_RDR;
			(void) junk_value;
		}

		/* Start the PDC reception, although nothing is received until the
		   SPI is also transmitting. */
		freertos_start_pdc_rx(&rx_dma_control, notification_semaphore);

		/*Channel configuration*/
		irqflags_t status = cpu_irq_save();
		tx_packet.mbr_ubc = len;

		tx_packet.mbr_sa = (uint32_t)data;
		tx_packet.mbr_da = (uint32_t)&(QSPI->QSPI_TDR);
		tx_packet.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN |
					XDMAC_CC_MBSIZE_SINGLE |
					XDMAC_CC_DSYNC_MEM2PER |
					XDMAC_CC_CSIZE_CHK_1 |
					XDMAC_CC_DWIDTH_BYTE |
					XDMAC_CC_SIF_AHB_IF0 |
					XDMAC_CC_DIF_AHB_IF1 |
					XDMAC_CC_SAM_INCREMENTED_AM |
					XDMAC_CC_DAM_FIXED_AM |
					XDMAC_CC_PERID(XDAMC_CHANNEL_HWID_QSPI_TX);

		tx_packet.mbr_bc = 0;
		tx_packet.mbr_ds =  0;
		tx_packet.mbr_sus = 0;
		tx_packet.mbr_dus = 0;

		xdmac_configure_transfer(XDMAC, XDMA_CH_SPI_TX, &tx_packet);
		xdmac_channel_set_descriptor_control(XDMAC, XDMA_CH_SPI_TX, 0);
		//xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_SPI_TX);
		//xdmac_channel_enable_interrupt(XDMAC, XDMA_CH_SPI_TX, XDMAC_CIE_BIE |
		//			XDMAC_CIE_RBIE  |
		//			XDMAC_CIE_WBIE  |
		//			XDMAC_CIE_ROIE);

		rx_packet.mbr_ubc = len;
		rx_packet.mbr_da = (uint32_t)data;
		rx_packet.mbr_sa = (uint32_t)&QSPI->QSPI_RDR;
		rx_packet.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN |
					XDMAC_CC_MBSIZE_SINGLE |
					XDMAC_CC_DSYNC_PER2MEM |
					XDMAC_CC_CSIZE_CHK_1 |
					XDMAC_CC_DWIDTH_BYTE|
					XDMAC_CC_SIF_AHB_IF1 |
					XDMAC_CC_DIF_AHB_IF0 |
					XDMAC_CC_SAM_FIXED_AM |
					XDMAC_CC_DAM_INCREMENTED_AM |
					XDMAC_CC_PERID(XDAMC_CHANNEL_HWID_QSPI_RX);

		rx_packet.mbr_bc = 0;
		rx_packet.mbr_ds =  0;
		rx_packet.mbr_sus = 0;
		rx_packet.mbr_dus = 0;

		xdmac_configure_transfer(XDMAC, XDMA_CH_SPI_RX, &rx_packet);
		xdmac_channel_set_descriptor_control(XDMAC, XDMA_CH_SPI_RX, 0);
		xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_SPI_RX);
		xdmac_channel_enable_interrupt(XDMAC, XDMA_CH_SPI_RX, XDMAC_CIE_BIE |
					XDMAC_CIE_RBIE  |
					XDMAC_CIE_WBIE  |
					XDMAC_CIE_ROIE);
		xdmac_enable_interrupt(XDMAC, XDMA_CH_SPI_RX);
		xdmac_channel_enable(XDMAC, XDMA_CH_SPI_RX);

		/*Start*/
		//xdmac_enable_interrupt(XDMAC, XDMA_CH_SPI_TX);
		xdmac_channel_enable(XDMAC, XDMA_CH_SPI_TX);

		__DMB();
		cpu_irq_restore(status);

		return_value = freertos_optionally_wait_transfer_completion(&rx_dma_control,
				notification_semaphore, block_time_ticks);
	} else {
		return_value = ERR_INVALID_ARG;
	}

	return return_value;
}



#if 0
/*
 * For internal use only.
 * A common SPI interrupt handler that is called for all SPI peripherals.
 */
static void local_spi_handler(const portBASE_TYPE spi_index)
{
	portBASE_TYPE higher_priority_task_woken = pdFALSE;

	uint32_t dma_status = xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_SPI_TX);

	/* Has the PDC completed a transmission? */
	if ((dma_status & XDMAC_CIS_BIS) != 0UL) {
		xdmac_channel_disable_interrupt(XDMAC, XDMA_CH_SPI_TX, MASK_ALL_INTERRUPTS);
		xdmac_channel_disable(XDMAC, XDMA_CH_SPI_TX);
		xdmac_disable_interrupt(XDMAC, (1 << XDMA_CH_SPI_TX));

		/* If the driver is supporting multi-threading, then return the access
		mutex. */
		if (tx_dma_control.peripheral_access_mutex != NULL) {
			xSemaphoreGiveFromISR(tx_dma_control.peripheral_access_mutex,
					&higher_priority_task_woken);
		}

		/* if the sending task supplied a notification semaphore, then
		 * notify the task that the transmission has completed. */
		if (tx_dma_control.transaction_complete_notification_semaphore != NULL) {
			xSemaphoreGiveFromISR(tx_dma_control.transaction_complete_notification_semaphore,
					&higher_priority_task_woken);
		}
	} else {
		__NOP();
	}

	dma_status = xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_SPI_RX);

	/* Has the PDC completed a transmission? */
	if ((dma_status & XDMAC_CIS_BIS) != 0UL) {
		xdmac_channel_disable_interrupt(XDMAC, XDMA_CH_SPI_RX, MASK_ALL_INTERRUPTS);
		xdmac_channel_disable(XDMAC, XDMA_CH_SPI_RX);
		xdmac_disable_interrupt(XDMAC, (1 << XDMA_CH_SPI_RX));

		xdmac_channel_disable(XDMAC, XDMA_CH_SPI_TX);

		/* If the driver is supporting multi-threading, then return the access
		mutex.  NOTE: As a reception is performed by first performing a
		transmission, the SPI receive function uses the tx access semaphore. */
		if (tx_dma_control.peripheral_access_mutex != NULL) {
			xSemaphoreGiveFromISR(tx_dma_control.peripheral_access_mutex,
					&higher_priority_task_woken);
		}

		/* If the receiving task supplied a notification semaphore, then
		notify the task that the transmission has completed. */
		if (rx_dma_control.transaction_complete_notification_semaphore != NULL) {
			xSemaphoreGiveFromISR(rx_dma_control.transaction_complete_notification_semaphore,
					&higher_priority_task_woken);
		}
	} else {
		__NOP();
	}

#if 0
	if ((spi_status & SR_ERROR_INTERRUPTS) != 0) {
		/* An mode error occurred in either a transmission or reception.  Abort.
		Stop the transmission, disable interrupts used by the peripheral, and
		ensure the peripheral access mutex is made available to tasks.  As this
		peripheral is half duplex, only the Tx peripheral access mutex exits. */
		//spi_disable_interrupt(spi_port, SPI_IDR_ENDTX);
		//spi_disable_interrupt(spi_port, SPI_IDR_ENDRX);

		if (tx_dma_control[spi_index].peripheral_access_mutex != NULL) {
			xSemaphoreGiveFromISR(
					tx_dma_control[spi_index].peripheral_access_mutex,
					&higher_priority_task_woken);
		}

		/* The SPI port will have been disabled, re-enable it. */
		spi_enable(spi_port);

		while (1) {;}
	}
#endif

	__DMB();

	/* If giving a semaphore caused a task to unblock, and the unblocked task
	has a priority equal to or higher than the currently running task (the task
	this ISR interrupted), then higher_priority_task_woken will have
	automatically been set to pdTRUE within the semaphore function.
	portEND_SWITCHING_ISR() will then ensure that this ISR returns directly to
	the higher priority unblocked task. */
	portEND_SWITCHING_ISR(higher_priority_task_woken);
}
#endif

static void xdma_tx_done(void)
{
	portBASE_TYPE higher_priority_task_woken = pdFALSE;

	uint32_t dma_status = xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_SPI_TX);

	/* Has the PDC completed a transmission? */
	if ((dma_status & XDMAC_CIS_BIS) != 0UL) {
		xdmac_channel_disable_interrupt(XDMAC, XDMA_CH_SPI_TX, MASK_ALL_INTERRUPTS);
		xdmac_channel_disable(XDMAC, XDMA_CH_SPI_TX);
		xdmac_disable_interrupt(XDMAC, (1 << XDMA_CH_SPI_TX));

		/* If the driver is supporting multi-threading, then return the access
		mutex. */
		if (tx_dma_control.peripheral_access_mutex != NULL) {
			xSemaphoreGiveFromISR(tx_dma_control.peripheral_access_mutex,
					&higher_priority_task_woken);
		}

		/* if the sending task supplied a notification semaphore, then
		 * notify the task that the transmission has completed. */
		if (tx_dma_control.transaction_complete_notification_semaphore != NULL) {
			xSemaphoreGiveFromISR(tx_dma_control.transaction_complete_notification_semaphore,
					&higher_priority_task_woken);
		}
	} else {
		__NOP();
	}

#if 0
	if ((spi_status & SR_ERROR_INTERRUPTS) != 0) {
		/* An mode error occurred in either a transmission or reception.  Abort.
		Stop the transmission, disable interrupts used by the peripheral, and
		ensure the peripheral access mutex is made available to tasks.  As this
		peripheral is half duplex, only the Tx peripheral access mutex exits. */
		//spi_disable_interrupt(spi_port, SPI_IDR_ENDTX);
		//spi_disable_interrupt(spi_port, SPI_IDR_ENDRX);

		if (tx_dma_control[spi_index].peripheral_access_mutex != NULL) {
			xSemaphoreGiveFromISR(
					tx_dma_control[spi_index].peripheral_access_mutex,
					&higher_priority_task_woken);
		}

		/* The SPI port will have been disabled, re-enable it. */
		spi_enable(spi_port);

		while (1) {;}
	}
#endif

	__DMB();

	/* If giving a semaphore caused a task to unblock, and the unblocked task
	has a priority equal to or higher than the currently running task (the task
	this ISR interrupted), then higher_priority_task_woken will have
	automatically been set to pdTRUE within the semaphore function.
	portEND_SWITCHING_ISR() will then ensure that this ISR returns directly to
	the higher priority unblocked task. */
	portEND_SWITCHING_ISR(higher_priority_task_woken);
}


static void xdma_rx_done(void)
{
	portBASE_TYPE higher_priority_task_woken = pdFALSE;

	uint32_t dma_status = xdmac_channel_get_interrupt_status(XDMAC, XDMA_CH_SPI_RX);

	/* Has the PDC completed a transmission? */
	if ((dma_status & XDMAC_CIS_BIS) != 0UL) {
		xdmac_channel_disable_interrupt(XDMAC, XDMA_CH_SPI_RX, MASK_ALL_INTERRUPTS);
		xdmac_channel_disable(XDMAC, XDMA_CH_SPI_RX);
		xdmac_disable_interrupt(XDMAC, (1 << XDMA_CH_SPI_RX));

		xdmac_channel_disable(XDMAC, XDMA_CH_SPI_TX);

		/* If the driver is supporting multi-threading, then return the access
		mutex.  NOTE: As a reception is performed by first performing a
		transmission, the SPI receive function uses the tx access semaphore. */
		if (tx_dma_control.peripheral_access_mutex != NULL) {
			xSemaphoreGiveFromISR(tx_dma_control.peripheral_access_mutex,
					&higher_priority_task_woken);
		}

		/* If the receiving task supplied a notification semaphore, then
		notify the task that the transmission has completed. */
		if (rx_dma_control.transaction_complete_notification_semaphore != NULL) {
			xSemaphoreGiveFromISR(rx_dma_control.transaction_complete_notification_semaphore,
					&higher_priority_task_woken);
		}
	} else {
		__NOP();
	}

#if 0
	if ((spi_status & SR_ERROR_INTERRUPTS) != 0) {
		/* An mode error occurred in either a transmission or reception.  Abort.
		Stop the transmission, disable interrupts used by the peripheral, and
		ensure the peripheral access mutex is made available to tasks.  As this
		peripheral is half duplex, only the Tx peripheral access mutex exits. */
		//spi_disable_interrupt(spi_port, SPI_IDR_ENDTX);
		//spi_disable_interrupt(spi_port, SPI_IDR_ENDRX);

		if (tx_dma_control[spi_index].peripheral_access_mutex != NULL) {
			xSemaphoreGiveFromISR(
					tx_dma_control[spi_index].peripheral_access_mutex,
					&higher_priority_task_woken);
		}

		/* The SPI port will have been disabled, re-enable it. */
		spi_enable(spi_port);

		while (1) {;}
	}
#endif

	__DMB();

	/* If giving a semaphore caused a task to unblock, and the unblocked task
	has a priority equal to or higher than the currently running task (the task
	this ISR interrupted), then higher_priority_task_woken will have
	automatically been set to pdTRUE within the semaphore function.
	portEND_SWITCHING_ISR() will then ensure that this ISR returns directly to
	the higher priority unblocked task. */
	portEND_SWITCHING_ISR(higher_priority_task_woken);
}


/*
 * Individual interrupt handlers follow from here.  Each individual interrupt
 * handler calls the common interrupt handler.
 */
//void XDMAC_Handler(void)
//{
//	local_spi_handler(0);
//}

#endif
