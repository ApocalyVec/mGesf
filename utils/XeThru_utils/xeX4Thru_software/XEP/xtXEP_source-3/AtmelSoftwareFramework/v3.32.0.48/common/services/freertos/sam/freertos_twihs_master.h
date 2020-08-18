/**
 * \file
 *
 * \brief FreeRTOS Peripheral Control API For the TWIHS
 *
 * Copyright (c) 2014-2016 Atmel Corporation. All rights reserved.
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

#ifndef FREERTOS_TWIHS_MASTER_INCLUDED
#define FREERTOS_TWIHS_MASTER_INCLUDED

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* ASF includes. */
#include "twihs_master.h"
#include "freertos_peripheral_control.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

#if XMEGA
# error Unsupported chip type
#elif UC3
# error Unsupported chip type
#elif SAM
	/* SAM is supported. */
#else
# error Unsupported chip type
#endif

/**
 * \defgroup freertos_twihs_peripheral_control_group FreeRTOS TWIHS peripheral
 * control
 * \brief FreeRTOS peripheral control functions for the TWIHS peripheral
 * \ingroup freertos_service_group
 */

/**
 * \ingroup freertos_twihs_peripheral_control_group
 * \typedef freertos_twihs_if
 * \brief Type returned from a call to freertos_twihs_master_init(), and then used
 * to reference a TWIHS port in calls to FreeRTOS peripheral control functions.
 */
typedef void *freertos_twihs_if;

freertos_twihs_if freertos_twihs_master_init(Twihs *p_twihs,
		const freertos_peripheral_options_t *const freertos_driver_parameters);

status_code_t freertos_twihs_write_packet_async(freertos_twihs_if p_twihs,
		twihs_packet_t *p_packet, portTickType block_time_ticks,
		xSemaphoreHandle notification_semaphore);

status_code_t freertos_twihs_read_packet_async(freertos_twihs_if p_twihs,
		twihs_packet_t *p_packet, portTickType block_time_ticks,
		xSemaphoreHandle notification_semaphore);

/**
 * \ingroup freertos_twihs_peripheral_control_group
 * \brief Initiate a multi-byte write operation on an TWIHS peripheral.
 *
 * freertos_twihs_write_packet() is an ASF specific FreeRTOS driver function.  It
 * configures the TWIHS peripheral DMA controller (PDC) to transmit data on the
 * TWIHS port, then waits until the transmission is complete.  Other RTOS tasks
 * execute while the transmission is in progress.
 *
 * freertos_twihs_write_packet_async() is a version that does not wait for the
 * transmission to complete before returning.
 *
 * The FreeRTOS ASF TWIHS driver is initialized using a call to
 * freertos_twihs_master_init().  The freertos_driver_parameters.options_flags
 * parameter passed into the initialization function defines the driver behavior.
 * freertos_twihs_write_packet() can only be used if the
 * freertos_driver_parameters.options_flags parameter passed to the initialization
 * function had the WAIT_TX_COMPLETE bit set.
 *
 * Readers are recommended to also reference the application note and examples
 * that accompany the FreeRTOS ASF drivers.
 *
 * The FreeRTOS ASF driver both installs and handles the TWIHS PDC interrupts.
 * Users do not need to concern themselves with interrupt handling, and must
 * not install their own interrupt handler.
 *
 * \param p_twihs    The handle to the TWIHS port returned by the
 *     freertos_twihs_master_init() call used to initialise the port.
 * \param p_packet    Structure that defines the TWIHS transfer parameters, such
 *     as the I2C chip being addressed, the source data location, and the number
 *     of bytes to transmit.  twihs_packet_t is a standard ASF type (it is not
 *     FreeRTOS specific).
 * \param block_time_ticks    The FreeRTOS ASF TWIHS driver is initialized using a
 *     call to freertos_twihs_master_init().  The
 *     freertos_driver_parameters.options_flags parameter passed to the
 *     initialization function defines the driver behavior.  If
 *     freertos_driver_parameters.options_flags had the USE_TX_ACCESS_SEM bit
 *     set, then the driver will only write to the TWIHS peripheral if it has
 *     first gained exclusive access to it.  block_time_ticks specifies the
 *     maximum amount of time the driver will wait to get exclusive access before
 *     aborting the write operation.  Other tasks will execute during any
 *     waiting time.  block_time_ticks is specified in RTOS tick periods.  To
 *     specify a block time in milliseconds, divide the milliseconds value by
 *     portTICK_RATE_MS, and pass the result in block_time_ticks.
 *     portTICK_RATE_MS is defined by FreeRTOS.
 *
 * \return     ERR_INVALID_ARG is returned if an input parameter is invalid.
 *     ERR_TIMEOUT is returned if block_time_ticks passed before exclusive access
 *     to the TWIHS peripheral could be obtained.  STATUS_OK is returned if the
 *     PDC was successfully configured to perform the TWIHS write operation.
 */
#define freertos_twihs_write_packet(p_twihs, p_packet, block_time_ticks) freertos_twihs_write_packet_async((p_twihs), (p_packet), (block_time_ticks), (NULL))

/**
 * \brief Initiate a multi-byte read operation on an TWIHS peripheral.
 *
 * freertos_twihs_read_packet() is an ASF specific FreeRTOS driver function.  It
 * configures the TWIHS peripheral DMA controller (PDC) to read data from the
 * TWIHS port, then waits until the requested number of bytes have been received.
 * Other RTOS tasks execute while the DMA transfer is in progress.
 *
 * freertos_twihs_read_packet_async() is a version that does not wait for the
 * reception to complete before returning.
 *
 * The FreeRTOS ASF TWIHS driver is initialized using a call to
 * freertos_twihs_master_init().  The freertos_driver_parameters.options_flags
 * parameter passed into the initialization function defines the driver behavior.
 * freertos_twihs_read_packet() can only be used if the
 * freertos_driver_parameters.options_flags parameter passed to the initialization
 * function had the WAIT_RX_COMPLETE bit set.
 *
 * Readers are recommended to also reference the application note and examples
 * that accompany the FreeRTOS ASF drivers.
 *
 * The FreeRTOS ASF driver both installs and handles the TWIHS PDC interrupts.
 * Users do not need to concern themselves with interrupt handling, and must
 * not install their own interrupt handler.
 *
 * \param p_twihs    The handle to the TWIHS port returned by the
 *     freertos_twihs_master_init() call used to initialise the port.
 * \param p_packet    Structure that defines the TWIHS transfer parameters, such
 *     as the I2C chip being addressed, the destination for the data being read,
 *     and the number of bytes to read.  twihs_packet_t is a standard ASF type (it
 *     is not FreeRTOS specific).
 * \param block_time_ticks    The FreeRTOS ASF TWIHS driver is initialized using a
 *     call to freertos_twihs_master_init().  The
 *     freertos_driver_parameters.options_flags parameter passed to the
 *     initialization function defines the driver behavior.  If
 *     freertos_driver_parameters.options_flags had the
 *     USE_RX_ACCESS_MUTEX bit set, then the driver will only read from the TWIHS
 *     peripheral if it has first gained exclusive access to it.  block_time_ticks
 *     specifies the maximum amount of time the driver will wait to get
 *     exclusive access before aborting the read operation.  Other tasks will
 *     execute during any waiting time.  block_time_ticks is specified in RTOS
 *     tick periods.  To specify a block time in milliseconds, divide the
 *     milliseconds value by portTICK_RATE_MS, and pass the result in
 *     block_time_ticks.  portTICK_RATE_MS is defined by FreeRTOS.
 *
 * \return     ERR_INVALID_ARG is returned if an input parameter is invalid.
 *     ERR_TIMEOUT is returned if block_time_ticks passed before exclusive access
 *     to the TWIHS peripheral could be obtained.  STATUS_OK is returned if the
 *     PDC was successfully configured to perform the TWIHS read operation.
 */
#define freertos_twihs_read_packet(p_twihs, p_packet, block_time_ticks) freertos_twihs_read_packet_async((p_twihs), (p_packet), (block_time_ticks), (NULL))

/**
 * \page freertos_twihs_peripheral_control_quick_start Quick start guide for
 * FreeRTOS TWIHS peripheral control functions
 *
 * This is the quick start guide for the
 * \ref freertos_twihs_peripheral_control_group, with
 * step-by-step instructions on how to configure and use the service.
 *
 * The service can be initialized to operate in one of two different modes -
 * standard mode, and fully asynchronous mode.
 *
 * freertos_twihs_write_packet() is used to transmit data in standard mode.
 * freertos_twihs_write_packet() does not return until all the data has been
 * fully transmitted.  Other FreeRTOS tasks will execute while the data
 * transmission is in progress.
 *
 * freertos_twihs_read_packet() is used to receive data in standard mode.
 * freertos_twihs_read_packet() does not return until all the requested data has
 * been received.  This is safe because, in master mode, the amount of data
 * being received is controlled by the receiving device, and therefore known in
 * advance.  Other FreeRTOS tasks will execute while the data is being received.
 *
 * freertos_twihs_write_packet_async() is used to transmit data in the fully
 * asynchronous mode.  freertos_twihs_write_packet_async() returns as soon as
 * the data transmission has started (not when it has finished), and can opt to
 * be notified by FreeRTOS when the transmission is complete.  The buffer being
 * transmitted must not be altered until the transmission is complete.
 *
 * freertos_twihs_write_packet_async() and freertos_twihs_read_packet_async() must
 * not be used when the peripheral is initialized to use standard mode.
 * Likewise, freertos_twihs_write_packet() and freertos_twihs_read_packet()
 * must not be used when the peripheral is configured to use fully asynchronous
 * mode.
 *
 * Refer to the FreeRTOS peripheral control projects in the Atmel ASF
 * distribution for complete working examples, and the
 * <a href="http://www.FreeRTOS.org" target="_blank">FreeRTOS web site</a> for
 * information on getting started with FreeRTOS.
 *
 * \section freertos_twihs_sync_use_case Standard Mode Functions
 * - \subpage freertos_twihs_initialization_sync
 * - \subpage freertos_twihs_tx_sync
 * - \subpage freertos_twihs_rx_sync
 *
 * \section freertos_twihs_async_use_case Fully Asynchronous Mode Functions
 * - \subpage freertos_twihs_initialization_async
 * - \subpage freertos_twihs_tx_async
 * - \subpage freertos_twihs_rx_async
 */

/**
 * \page freertos_twihs_initialization_sync Initializing the FreeRTOS TWIHS driver
 * for standard operation
 *
 * This example configures the FreeRTOS TWIHS driver to:
 * - Support simultaneous access from multiple RTOS tasks.
 * - Wait in the FreeRTOS transmit function until all the data has been
 * completely sent
 * - Wait in the FreeRTOS receive function until all the requested data has been
 * received
 *
 * Other FreeRTOS tasks will execute during any wait periods.
 *
 * If the driver is initialized with this configuration then only the
 * freertos_twihs_write_packet() and freertos_twihs_read_packet() API functions can
 * be used to transmit and receive data respectively.
 * freertos_twihs_write_packet_async() and freertos_twihs_read_packet_async() must
 * not be used.
 *
 * Refer to the FreeRTOS peripheral control projects in the Atmel ASF
 * distribution for complete working examples, and the
 * <a href="http://www.FreeRTOS.org" target="_blank">FreeRTOS web site</a> for
 * information on getting started with FreeRTOS.
 *
 *
 * \code
 *
 * /////////////////////////////////////////////////////////////////////////////
 * // Declare the variables used as parameters to the
 * // freertos_twihs_master_init() function.
 *
 * // Handle used to access the initialized port by other FreeRTOS ASF functions.
 * freertos_twihs_if freertos_twihs;
 *
 * // Configuration structure.
 * freertos_peripheral_options_t driver_options = {
 *
 *     // This peripheral is synchronous and so does not need a receive buffer.
 *     // The receive_buffer value is just set to NULL.
 *     NULL,
 *
 *     // There isn't a receive buffer, so the receive_buffer_size value can
 *     // take any value.
 *     0,
 *
 *     // The interrupt priority.  The FreeRTOS driver provides the interrupt
 *     // service routine, and handles all interrupt interactions.  The
 *     // application writer does not need to provide any interrupt handling
 *     // code, but does need to specify the priority of the DMA interrupt here.
 *     // IMPORTANT!!!  see <a href="http://www.freertos.org/RTOS-Cortex-M3-M4.html">how to set interrupt priorities to work with FreeRTOS</a>
 *     0x0e,
 *
 *     // The operation_mode value.
 *     TWIHS_I2C_MASTER,
 *
 *     // Flags set to allow access from multiple tasks, wait in the transmit
 *     // function until the transmit is complete, and wait in the receive
 *     // function until reception is complete.  Note that other FreeRTOS tasks
 *     // will execute during the wait period.
 *     (USE_TX_ACCESS_SEM | USE_RX_ACCESS_MUTEX | WAIT_TX_COMPLETE | WAIT_RX_COMPLETE)
 *  };
 *
 *  /////////////////////////////////////////////////////////////////////////////
 *  // Call the TWIHS specific FreeRTOS ASF driver initialization function,
 *  // storing the return value as the driver handle.
 *  freertos_twihs = freertos_twihs_master_init(twihs_base, &driver_options);
 *
 *  if (freertos_twihs != NULL) {
 *     // Calling freertos_twihs_master_init() will enable the peripheral
 *     // clock, and set the TWIHS into I2C master mode.  Other ASF
 *     // configuration functions, such as twihs_set_speed(), can then be
 *     // called here.
 *  }
 *
 * \endcode
 */

/**
 * \page freertos_twihs_initialization_async Initializing the FreeRTOS TWIHS driver
 * for asynchronous operation
 *
 * This example configures the FreeRTOS TWIHS driver to:
 * - Support simultaneous access from multiple RTOS tasks.
 * - Exit the FreeRTOS transmit function as soon as the transmission is started.
 * - Exit the receive function as soon as the receive has been requested.
 *
 * If the driver is initialized with this configuration then only the
 * freertos_twihs_write_packet_async() and freertos_twihs_read_packet_async() API
 * functions can be used to transmit and receive data respectively.
 * freertos_twihs_write_packet() and freertos_twihs_read_packet() must not be used.
 *
 * Refer to the FreeRTOS peripheral control projects in the Atmel ASF
 * distribution for complete working examples, and the
 * <a href="http://www.FreeRTOS.org" target="_blank">FreeRTOS web site</a> for
 * information on getting started with FreeRTOS.
 *
 *
 * \code
 *
 * /////////////////////////////////////////////////////////////////////////////
 * // Declare the variables used as parameters to the
 * // freertos_twihs_master_init() function.
 *
 * // Handle used to access the initialized port by other FreeRTOS ASF functions.
 * freertos_twihs_if freertos_twihs;
 *
 * // Configuration structure.
 * freertos_peripheral_options_t driver_options = {
 *
 *     // This peripheral is synchronous and so does not need a receive buffer.
 *     // The receive_buffer value is just set to NULL.
 *     NULL,
 *
 *     // There isn't a receive buffer, so the receive_buffer_size value can
 *     // take any value.
 *     0,
 *
 *     // The interrupt priority.  The FreeRTOS driver provides the interrupt
 *     // service routine, and handles all interrupt interactions.  The
 *     // application writer does not need to provide any interrupt handling
 *     // code, but does need to specify the priority of the DMA interrupt here.
 *     // IMPORTANT!!!  see <a href="http://www.freertos.org/RTOS-Cortex-M3-M4.html">how to set interrupt priorities to work with FreeRTOS</a>
 *     0x0e,
 *
 *     // The operation_mode value.
 *     TWIHS_I2C_MASTER,
 *
 *     // Flags set to allow access from multiple tasks.  Note the
 *     // WAIT_TX_COMPLETE and WAIT_RX_COMPLETE bits are *not* set.
 *     (USE_TX_ACCESS_SEM | USE_RX_ACCESS_MUTEX)
 *  };
 *
 * /////////////////////////////////////////////////////////////////////////////
 * // Call the TWIHS specific FreeRTOS ASF driver initialization function,
 * // storing the return value as the driver handle.
 *  freertos_twihs = freertos_twihs_master_init(twihs_base, &driver_options);
 *
 *  if (freertos_twihs != NULL) {
 *     // Calling freertos_twihs_master_init() will enable the peripheral
 *     // clock, and set the TWIHS into I2C master mode.  Other ASF
 *     // configuration functions, such as twihs_set_speed(), can then be
 *     // called here.
 *  }
 *
 * \endcode
 */

/**
 * \page freertos_twihs_tx_sync Transmitting using the FreeRTOS TWIHS driver in
 * standard mode
 *
 * This example demonstrates using the FreeRTOS TWIHS driver to send data using
 * the standard operation mode.  See \ref freertos_twihs_initialization_sync
 *
 * The example below implements a function that simply writes a block of data to
 * a TWIHS port.
 *
 * Refer to the FreeRTOS peripheral control projects in the Atmel ASF
 * distribution for complete working examples, and the
 * <a href="http://www.FreeRTOS.org" target="_blank">FreeRTOS web site</a> for
 * information on getting started with FreeRTOS.
 *
 * \code
 *
 *  // Write number_of_bytes from data_buffer to freertos_twihs.
 *  //
 *  // This examples assumes freertos_twihs has already been set by a successful
 *  // call to freertos_twihs_master_init(), and that freertos_twihs_master_init()
 *  // configured the FreeRTOS TWIHS driver to use the standard operation mode.
 *
 *  static void write_page_to_eeprom(freertos_twihs_if freertos_twihs, uint16_t page)
 *  {
 *  twihs_packet_t write_parameters;
 *  uint16_t calculated_address;
 *  const portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;
 *
 *      // Calculate the address being written to
 *      calculated_address = page * PAGE_SIZE;
 *
 *      // Configure the twihs_packet_t structure to define the write operation
 *      write_parameters.chip = BOARD_AT24C_ADDRESS;
 *      write_parameters.buffer = data_buffer;
 *      write_parameters.length = PAGE_SIZE;
 *      write_parameters.addr[0] = (uint8_t) ((calculated_address >> 8) & 0xff);
 *      write_parameters.addr[1] = (uint8_t) (calculated_address & 0xff);
 *      write_parameters.addr_length = 2;
 *
 *      // Attempt to write the data to the port referenced by the freertos_twihs
 *      // handle.  Wait a maximum of 200ms to get exclusive access to the port
 *      // (other FreeRTOS tasks will execute during any waiting time).
 *      if(freertos_twihs_write_packet(freertos_twihs, &write_parameters, max_block_time_ticks) != STATUS_OK)
 *      {
 *          // The data was not written successfully, either because there was
 *          // an error on the TWIHS bus, or because exclusive access to the TWIHS
 *          // port was not obtained within 200ms.
 *      }
 * }
 * \endcode
 */

/**
 * \page freertos_twihs_tx_async Transmitting using the FreeRTOS TWIHS driver in
 * fully asynchronous mode
 *
 * This example demonstrates using the FreeRTOS TWIHS driver to send data using
 * the fully asynchronous operation mode.  See \ref
 * freertos_twihs_initialization_async
 *
 * The example below implements a function that simply writes a block of data to
 * a TWIHS port.
 *
 * Refer to the FreeRTOS peripheral control projects in the Atmel ASF
 * distribution for complete working examples, and the
 * <a href="http://www.FreeRTOS.org" target="_blank">FreeRTOS web site</a> for
 * information on getting started with FreeRTOS.
 *
 * \code
 *
 *  // Write number_of_bytes from data_buffer to freertos_twihs.
 *  //
 *  // This examples assumes freertos_twihs has already been set by a successful
 *  // call to freertos_twihs_master_init(), and that freertos_twihs_master_init()
 *  // configured the FreeRTOS TWIHS driver to use the fully asynchronous operation
 *  // mode.
 *  //
 *  // The example further assumes that notification_semaphore has already been
 *  // initialised by a call to the vSemaphoreCreateBinary() FreeRTOS API
 *  // function.
 *
 *  static void write_page_to_eeprom(freertos_twihs_if freertos_twihs,
 *                      uint16_t page, xSemaphoreHandle notification_semaphore)
 *  {
 *  twihs_packet_t write_parameters;
 *  uint16_t calculated_address;
 *  const portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;
 *
 *      // Calculate the address being written to
 *      calculated_address = page * PAGE_SIZE;
 *
 *      // Configure the twihs_packet_t structure to define the write operation
 *      write_parameters.chip = BOARD_AT24C_ADDRESS;
 *      write_parameters.buffer = data_buffer;
 *      write_parameters.length = PAGE_SIZE;
 *      write_parameters.addr[0] = (uint8_t) ((calculated_address >> 8) & 0xff);
 *      write_parameters.addr[1] = (uint8_t) (calculated_address & 0xff);
 *      write_parameters.addr_length = 2;
 *
 *      // Attempt to initiate a DMA transfer to write the data from data_buffer
 *      // to the port referenced by the freertos_twihs handle.  Wait a maximum of
 *      // 200ms to get exclusive access to the port (other FreeRTOS tasks will
 *      // execute during any waiting time).
 *      if(freertos_twihs_write_packet(freertos_twihs, &write_parameters,
 *                 max_block_time_ticks, notification_semaphore) != STATUS_OK)
 *      {
 *          // The DMA transfer was not started because exclusive access to the
 *          // port was not obtained within 200ms.
 *      }
 *      else
 *      {
 *          // The DMA transfer was started successfully, but has not
 *          // necessarily completed yet.  Other processing can be performed
 *          // here, but THE DATA IN data_buffer MUST NOT BE ALTERED UNTIL THE
 *          // TRANSMISSION HAS COMPLETED.
 *
 *          // ...
 *
 *          // Ensure the transaction is complete before proceeding further by
 *          // waiting for the notification_semaphore.  Don't wait longer than
 *          // 200ms.  Other FreeRTOS tasks will execute during any waiting
 *          // time.
 *          if(xSemaphoreTake(notification_semaphore,
 *                                            max_block_time_ticks) != pdPASS)
 *          {
 *              // The semaphore could not be obtained within 200ms.  Either the
 *              // data is still being transmitted, or an error occurred.
 *          }
 *      }
 * }
 * \endcode
 */

/**
 * \page freertos_twihs_rx_sync Receiving using the FreeRTOS TWIHS driver in
 * standard mode
 *
 * This example demonstrates using the FreeRTOS TWIHS driver to receive data using
 * the standard operation mode.  See \ref freertos_twihs_initialization_sync
 *
 * The example below implements a function that simply reads a block of data
 * from a TWIHS port.
 *
 * Refer to the FreeRTOS peripheral control projects in the Atmel ASF
 * distribution for complete working examples, and the
 * <a href="http://www.FreeRTOS.org" target="_blank">FreeRTOS web site</a> for
 * information on getting started with FreeRTOS.
 *
 * \code
 *
 * // This examples assumes freertos_twihs has already been set by a successful
 * // call to freertos_twihs_master_init(), and that freertos_twihs_master_init()
 * // configured the FreeRTOS TWIHS driver to use the standard operation mode.
 *
 *  static void read_page_from_eeprom(freertos_twihs_if freertos_twihs,
 *                                     uint16_t page, uint8_t receive_buffer)
 *  {
 *  twihs_packet_t read_parameters;
 *  uint16_t calculated_address;
 *  const portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;
 *
 *      // Calculate the address being read from.
 *      calculated_address = page * PAGE_SIZE;
 *
 *      // Configure the read_parameters structure to define the read operation.
 *      read_parameters.chip = BOARD_AT24C_ADDRESS;
 *      read_parameters.buffer = receive_buffer;
 *      read_parameters.length = PAGE_SIZE;
 *      read_parameters.addr[0] = (uint8_t) ((calculated_address >> 8) & 0xff);
 *      read_parameters.addr[1] = (uint8_t) (calculated_address & 0xff);
 *      read_parameters.addr_length = 2;
 *
 *      // Receive the data into receive_buffer.  Don't wait any longer than 200ms
 *      // to get exclusive access to the port (other FreeRTOS tasks will
 *      // execute during any wait time.
 *      if(freertos_twihs_read_packet(freertos_twihs, &read_parameters,
 *                                      max_block_time_ticks) != STATUS_OK)
 *      {
 *          // Either an error occurred on the bus or exclusive access
 *          // to the port was not obtained within 200ms.
 *      }
 *      else
 *      {
 *          // freertos_twihs_read_packet() does not return until all the
 *          // requested bytes have been received, so it is known that the
 *          // data in receive_buffer is already complete, and can be
 *          // processed immediately.
 *
 *          // ... Process received data here ...
 *          do_something(receive_buffer);
 *      }
 *  }
 *
 * \endcode
 */

/**
 * \page freertos_twihs_rx_async Receiving using the FreeRTOS TWIHS driver in fully
 * asynchronous mode
 *
 * This example demonstrates using the FreeRTOS TWIHS driver to receive data using
 * the fully asynchronous operation mode.  See \ref
 * freertos_twihs_initialization_sync
 *
 * The example below implements a function that receives a block of data, then
 * processes the received data while another bock is being received.
 *
 * Refer to the FreeRTOS peripheral control projects in the Atmel ASF
 * distribution for complete working examples, and the
 * <a href="http://www.FreeRTOS.org" target="_blank">FreeRTOS web site</a> for
 * information on getting started with FreeRTOS.
 *
 * \code
 *
 * // This examples assumes freertos_twihs has already been set by a successful
 * // call to freertos_twihs_master_init(), and that freertos_twihs_master_init()
 * // configured the FreeRTOS TWIHS driver to use the fully asynchronous operation
 * // mode.
 *
 * // This example demonstrates how a single task can process data while
 * // additional data is being received on the TWIHS bus.  Error checking is
 * // omitted to simplify the example.
 *  void a_function(freertos_twihs_if freertos_twihs)
 *  {
 *  // The buffers into which the data is placed are too large to be declared on
 *  // the task stack, so are instead declared static (making this function
 *  // non-reentrant ?meaning it can only be called by a single task at a time,
 *  // otherwise multiple tasks would use the same buffers).
 *  static uint8_t first_receive_buffer[BUFFER_SIZE], second_receive_buffer[BUFFER_SIZE];
 *  xSemaphoreHandle notification_semaphore = NULL;
 *  const max_block_time_500ms = 500 / portTICK_RATE_MS;
 *	twihs_packet_t read_parameters;
 *	uint16_t calculated_address;
 *
 *      // Configure the read_parameters structure to define a read operation
 *      // that fills the first receive buffer.
 *      read_parameters.chip = BOARD_AT24C_ADDRESS;
 *      read_parameters.buffer = first_receive_buffer;
 *      read_parameters.length = BUFFER_SIZE;
 *      read_parameters.addr_length = 2;
 *
 *      // The first read is from address 0.
 *      calculated_address = 0;
 *      read_parameters.addr[0] = 0;
 *      read_parameters.addr[1] = 0;
 *
 *      // Create the notification semaphore that will be used by FreeRTOS to
 *      // notify the task that the receive operation is complete.
 *      vSemaphoreCreateBinary(notification_semaphore);
 *
 *      // Nothing has been read over the TWIHS bus yet, so make sure the
 *      // semaphore starts in the expected empty state.
 *      xSemaphoreTake(notification_semaphore, 0);
 *
 *      // Start an asynchronous read to fill the first buffer.  The function
 *      // will be able to access the port immediately because nothing else has
 *      // accessed it yet - allowing the block_time_ticks value to be set to 0.
 *      freertos_twihs_read_packet_async(freertos_twihs, &read_parameters, 0
 *                                                     notification_semaphore);
 *
 *      // Wait until the first buffer is full.  Other tasks will run during
 *      // the wait.  FreeRTOS will give notification_semaphore when
 *      // the receive is complete.
 *      xSemaphoreTake(notification_semaphore, max_block_time_500ms);
 *
 *      // Move the calculated address by the amount of bytes read so far.
 *      calculated_address += BUFFER_SIZE
 *      read_parameters.addr[0] = (uint8_t) ((calculated_address >> 8) & 0xff);
 *      read_parameters.addr[1] = (uint8_t) (calculated_address & 0xff);
 *
 *      // The first buffer is full.  The next read will fill the second buffer.
 *      read_parameters.buffer = second_receive_buffer;
 *
 *      // Start an asynchronous read to fill the second buffer.  Assuming no
 *      // other tasks are using the same TWIHS port, it is known that the port is
 *      // already available because the first read has completed.  This means
 *      // the block time can be 0.
 *      freertos_twihs_read_packet_async(freertos_twihs, &read_parameters, 0
 *                                                     notification_semaphore);
 *
 *      // Process the data in the first receive buffer while the second
 *      // receive buffer is being filled.
 *      process_received_data(first_receive_buffer);
 *
 *      // Wait until the second buffer is full.  Other tasks will run
 *      // during the wait.  FreeRTOS will give notification_semaphore when
 *      // the receive is complete.
 *      xSemaphoreTake(notification_semaphore, max_block_time_500ms);
 *
 *      // Process the data in the second receive buffer.
 *      process_received_data(second_receive_buffer);
 *  }
 * \endcode
 */

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* FREERTOS_TWIHS_MASTER_INCLUDED */
