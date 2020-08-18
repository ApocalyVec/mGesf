/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/*                       SAM Software Package License                           */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2015, Atmel Corporation                                        */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */
/**
 * \file
 *
 * \brief USB host Communication Device Class interface.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "conf_usb_host.h"
#include "USBHDriver.h"
#include "USBDescriptors.h"
#include "USBH.h"
#include "CDCDescriptors.h"
#include "uhi_cdc.h"
#include <string.h>

#ifdef USB_HOST_HUB_SUPPORT
	#  error USB HUB support is not implemented on UHI CDC
#endif

#ifndef UHI_CDC_RX_NOTIFY
	#define UHI_CDC_RX_NOTIFY()
#endif

//! Define the minimum internal buffer size
#ifdef UHI_CDC_LOW_RATE
	#define UHI_CDC_BUFFER_SIZE  (1*64)
#else
	#define UHI_CDC_BUFFER_SIZE  (5*64)
#endif

/**
 * \ingroup uhi_cdc_group
 * \defgroup uhi_cdc_group_internal Implementation of UHI Communication Device Class
 *
 * Class internal implementation
 * @{
 */

/**
 * \name Internal defines and variables to manage CDC device
 */
//@{

//! Internal buffer information
typedef struct {
	//! Position of consumer in this buffer
	uint16_t pos;
	//! Number of data available in buffer
	uint16_t nb;
	//! Pointer on internal buffer
	uint8_t *ptr;
} uhi_cdc_buf_t;

//! Communication line information
typedef struct {
	//! Bulk endpoint number used to transfer data
	USBHS_Ep_t ep_data;
	//! True, if a transfer is on-going on endpoint
	bool b_trans_ongoing;
	//! Frame number of last transfer
	uint16_t sof;
	//! Current internal buffer used to store data
	volatile uint8_t buf_sel;
	//! Size of internal buffer (unit Byte)
	uint16_t buffer_size;
	//! Internal buffer information
	uhi_cdc_buf_t buffer[2];
} uhi_cdc_line_t;

//! Communication port information
typedef struct {
	//! Current port configuration (baudrate,...)
	CDCLineCoding conf;
	//! USB CDC-COMM interface number
	uint8_t  iface_comm;
	//! USB CDC-DATA interface number
	uint8_t  iface_data;
	//! Interrupt IN endpoint from CDC-COMM interface
	USBHS_Ep_t ep_comm_in;
	//! RX line information
	uhi_cdc_line_t line_rx;
	//! TX line information
	uhi_cdc_line_t line_tx;
} uhi_cdc_port_t;

//! USB CDC device information
typedef struct {
	//! Pointer on USB Device information
	USBH_device_t *dev;
	//! True, if a CDC Device has been enumerated
	bool b_enabled;
	//! Pointer on communication port(s) information
	uhi_cdc_port_t *port;
	//! Number of port available on this USB device
	uint8_t nb_port;
} uhi_cdc_dev_t;

//! Information about the enumerated USB CDC device
static uhi_cdc_dev_t uhi_cdc_dev = {
	.dev = NULL,
	.nb_port = 0,
};
//@}

/**
 * \name Internal routines
 */
//@{

/**
 * \brief Free all CDC device structures
 */
static void uhi_cdc_free_device(void);

/**
 * \brief Returns a port structure information
 *
 * \param port_num  Number of port
 *
 * \return pointer on port structure
 */
static uhi_cdc_port_t *uhi_cdc_get_port(uint8_t port_num);

/**
 * \brief Changes the port configuration
 *
 * \param port_num       Number of port
 * \param configuration  Pointer on new configuration
 *
 * \return True, if success
 */
static bool uhi_cdc_set_conf(uint8_t port, CDCLineCoding *configuration);

/**
 * \brief Sends a control line command on the port
 *
 * \param port_num       Number of port
 * \param wValue         USB CDC control line word
 *
 * \return True, if success
 */
static bool uhi_cdc_set_ctrl_line(uint8_t port, uint16_t wValue);

/**
 * \brief Update the transfer endpoint IN (RX)
 * Valid the reception of the previous transfer.
 * Start a new transfer on endpoint IN, if a RX buffer is free
 * and a new USB frame is occurred since last transfer.
 *
 * \param line        RX communication line to manage
 *
 * \return True, if a new transfer has been started
 */
static bool uhi_cdc_rx_update(uhi_cdc_line_t *line);

/**
 * \brief Manage the end of RX transfer.
 * Registered by USBH_HAL_RunEndpoint()
 * Callback called by USB interrupt after data transfer or abort (reset,...).
 *
 * \param add            USB address used by the transfer
 * \param status         Transfer status
 * \param nb_transferred Number of data transferred
 */
static void uhi_cdc_rx_received(USBHS_Add_t add, USBHS_Ep_t ep,
								USBH_XfrStatus_t status, uint32_t nb_transferred);

/**
 * \brief Update the transfer endpoint OUT (TX)
 * Start a new transfer on endpoint OUT, if a data must be send
 * and a new USB frame is occurred since last transfer.
 *
 * \param line        TX communication line to manage
 *
 * \return True, if a new transfer has been started
 */
static bool uhi_cdc_tx_update(uhi_cdc_line_t *line);

/**
 * \brief Manage the end of TX transfer.
 * Registered by USBH_HAL_RunEndpoint()
 * Callback called by USB interrupt after data transfer or abort (reset,...).
 *
 * \param add            USB address used by the transfer
 * \param status         Transfer status
 * \param nb_transferred Number of data transferred
 */
static void uhi_cdc_tx_send(USBHS_Add_t add, USBHS_Ep_t ep,
							USBH_XfrStatus_t status, uint32_t nb_transferred);
//@}

//@}


/**
 * \name Interface used by UHC module
 */
//@{

USBH_enum_status_t uhi_cdc_install(USBH_device_t *dev)
{
	bool b_iface_comm, b_iface_data;
	uint16_t conf_desc_lgt;
	uint8_t port_num, i;
	USBInterfaceDescriptor *ptr_iface;
	uhi_cdc_port_t *ptr_port = NULL;
	uhi_cdc_line_t *ptr_line;

	if (uhi_cdc_dev.dev != NULL) {
		return UHC_ENUM_SOFTWARE_LIMIT; // Device already allocated
	}

	// Compute the number of port
	conf_desc_lgt = (dev->conf_desc->wTotalLength);
	ptr_iface = (USBInterfaceDescriptor *)dev->conf_desc;
	uhi_cdc_dev.nb_port = 0;

	while (conf_desc_lgt) {
		if ((ptr_iface->bDescriptorType == USBGenericDescriptor_INTERFACE)
			&& (ptr_iface->bInterfaceClass == CDCCommunicationInterfaceDescriptor_CLASS)
			&& (ptr_iface->bInterfaceSubClass ==
				CDCCommunicationInterfaceDescriptor_ABSTRACTCONTROLMODEL)
			&& (ptr_iface->bInterfaceProtocol <=
				CDCCommunicationInterfaceDescriptor_NOPROTOCOL)) {
			// New COM port has been found
			uhi_cdc_dev.nb_port++;
		}

		assert(conf_desc_lgt >= ptr_iface->bLength);
		conf_desc_lgt -= ptr_iface->bLength;
		ptr_iface = (USBInterfaceDescriptor *)((uint8_t *)ptr_iface +
											   ptr_iface->bLength);
	}

	if (uhi_cdc_dev.nb_port == 0) {
		return UHC_ENUM_UNSUPPORTED; // No interface supported
	}

	// Alloc port structures
	uhi_cdc_dev.port = malloc(uhi_cdc_dev.nb_port * sizeof(uhi_cdc_port_t));

	if (uhi_cdc_dev.port == NULL) {
		assert(false);
		return UHC_ENUM_SOFTWARE_LIMIT;
	}

	// Initialize structure
	for (i = 0; i < uhi_cdc_dev.nb_port; i++) {
		uhi_cdc_dev.port[i].ep_comm_in = 0;
		uhi_cdc_dev.port[i].iface_data = 0xFF;
		uhi_cdc_dev.port[i].line_rx.ep_data = 0;
		uhi_cdc_dev.port[i].line_rx.buffer[0].ptr = NULL;
		uhi_cdc_dev.port[i].line_rx.buffer[1].ptr = NULL;
		uhi_cdc_dev.port[i].line_tx.ep_data = 0;
		uhi_cdc_dev.port[i].line_tx.buffer[0].ptr = NULL;
		uhi_cdc_dev.port[i].line_tx.buffer[1].ptr = NULL;
	}

	// Fill port structures
	conf_desc_lgt = (dev->conf_desc->wTotalLength);
	ptr_iface = (USBInterfaceDescriptor *)dev->conf_desc;
	b_iface_comm = false;
	b_iface_data = false;
	port_num = 0;

	while (conf_desc_lgt) {
		switch (ptr_iface->bDescriptorType) {

		case USBGenericDescriptor_INTERFACE:
			if ((ptr_iface->bInterfaceClass == CDCCommunicationInterfaceDescriptor_CLASS)
				&& (ptr_iface->bInterfaceSubClass ==
					CDCCommunicationInterfaceDescriptor_ABSTRACTCONTROLMODEL)
				&& (ptr_iface->bInterfaceProtocol <=
					CDCCommunicationInterfaceDescriptor_NOPROTOCOL)) {
				// New Communication Class COM port has been found
				b_iface_comm = true;
				ptr_port = &uhi_cdc_dev.port[port_num++];
				ptr_port->iface_comm = ptr_iface->bInterfaceNumber;
			} else {
				// Stop allocation endpoint(s)
				b_iface_comm = false;
			}

			if ((ptr_iface->bInterfaceClass == CDCDataInterfaceDescriptor_CLASS)
				&& (ptr_iface->bInterfaceSubClass == 0)
				&& (ptr_iface->bInterfaceProtocol == 0)) {
				for (i = 0; i < uhi_cdc_dev.nb_port; i++) {
					ptr_port = &uhi_cdc_dev.port[i];
					b_iface_data = true;
					break;
				}
			} else {
				// Stop allocation endpoint(s)
				b_iface_data = false;
			}

			break;

		case CDCGenericDescriptor_INTERFACE:
			if (!b_iface_comm)
				break;

			if (((CDCCallManagementDescriptor *)ptr_iface)->bDescriptorSubtype ==
				CDCGenericDescriptor_CALLMANAGEMENT)
				ptr_port->iface_data = ((CDCCallManagementDescriptor *)
										ptr_iface)->bDataInterface;

			break;

		case USBGenericDescriptor_ENDPOINT:

			//  Allocation of the endpoint
			if (b_iface_comm) {
				assert (((USBEndpointDescriptor *)ptr_iface)->bmAttributes ==
						USBEndpointDescriptor_INTERRUPT);
				assert(((USBEndpointDescriptor *)ptr_iface)->bEndpointAddress & USB_EP_DIR_IN);

				if (!USBH_HAL_ConfigurePipe(dev->address, (USBEndpointDescriptor *)ptr_iface)) {
					uhi_cdc_free_device();
					return UHC_ENUM_HARDWARE_LIMIT; // Endpoint allocation fail
				}

				ptr_port->ep_comm_in = ((USBEndpointDescriptor *)ptr_iface)->bEndpointAddress;
			}

			if (b_iface_data) {
				assert (((USBEndpointDescriptor *)ptr_iface)->bmAttributes ==
						USBEndpointDescriptor_BULK);

				if (!USBH_HAL_ConfigurePipe(dev->address, (USBEndpointDescriptor *)ptr_iface)) {
					uhi_cdc_free_device();
					return UHC_ENUM_HARDWARE_LIMIT; // Endpoint allocation fail
				}

				if (((USBEndpointDescriptor *)ptr_iface)->bEndpointAddress & USB_EP_DIR_IN)
					ptr_line = &ptr_port->line_rx;
				else
					ptr_line = &ptr_port->line_tx;

				ptr_line->ep_data = ((USBEndpointDescriptor *)ptr_iface)->bEndpointAddress;
				ptr_line->b_trans_ongoing = false;
				ptr_line->buf_sel = 0;

				// Allocate and initialize buffers
				uint16_t buf_size = Max((
											 ((USBEndpointDescriptor *)ptr_iface)->wMaxPacketSize),
										 UHI_CDC_BUFFER_SIZE);
				ptr_line->buffer_size = buf_size;
				ptr_line->buffer[0].pos = 0;
				ptr_line->buffer[0].nb = 0;
				ptr_line->buffer[0].ptr = malloc(buf_size);

				if (ptr_line->buffer[0].ptr == NULL) {
					assert(false);
					uhi_cdc_free_device();
					return UHC_ENUM_SOFTWARE_LIMIT;
				}

				ptr_line->buffer[1].pos = 0;
				ptr_line->buffer[1].nb = 0;
				ptr_line->buffer[1].ptr = malloc(buf_size);

				if (ptr_line->buffer[1].ptr == NULL) {
					assert(false);
					uhi_cdc_free_device();
					return UHC_ENUM_SOFTWARE_LIMIT;
				}

			}

			break;

		}

		assert(conf_desc_lgt >= ptr_iface->bLength);
		conf_desc_lgt -= ptr_iface->bLength;
		ptr_iface = (USBInterfaceDescriptor *)((uint8_t *)ptr_iface +
											   ptr_iface->bLength);
	}

	// Check installed ports
	for (i = 0; i < uhi_cdc_dev.nb_port; i++) {
		if ((uhi_cdc_dev.port[i].ep_comm_in == 0)
			|| (uhi_cdc_dev.port[i].line_rx.ep_data == 0)
			|| (uhi_cdc_dev.port[i].line_tx.ep_data == 0)) {
			// Install is not complete
			uhi_cdc_free_device();
			return UHC_ENUM_UNSUPPORTED;
		}
	}

	uhi_cdc_dev.b_enabled = false;
	uhi_cdc_dev.dev = dev;
	return UHC_ENUM_SUCCESS;
}

void uhi_cdc_enable(USBH_device_t *dev)
{
	if (uhi_cdc_dev.dev != dev) {
		return; // No interface to enable
	}

	uhi_cdc_dev.b_enabled = true;

	// Start all data transfers
	uhi_cdc_sof(false);
	UHI_CDC_CHANGE(dev, true);
}

void uhi_cdc_uninstall(USBH_device_t *dev)
{
	if (uhi_cdc_dev.dev != dev) {
		return; // Device not enabled in this interface
	}

	uhi_cdc_dev.dev = NULL;
	uhi_cdc_free_device();
	UHI_CDC_CHANGE(dev, false);
}

void uhi_cdc_sof(bool b_micro)
{
	uint8_t port = 0;
	uhi_cdc_port_t *ptr_port;
	UNUSED(b_micro);

	if (uhi_cdc_dev.dev == NULL) {
		return; // No interface to installed
	}

	if (!uhi_cdc_dev.b_enabled) {
		return; // Interface not enabled
	}

	// Update transfers on each port
	while (1) {
		ptr_port = uhi_cdc_get_port(port++);

		if (ptr_port == NULL)
			break;

		uhi_cdc_rx_update(&ptr_port->line_rx);
		uhi_cdc_tx_update(&ptr_port->line_tx);
	}
}
//@}


/**
 * \name Internal routines
 */
//@{

static void uhi_cdc_free_device(void)
{
	if (uhi_cdc_dev.port == NULL)
		return;

	uint8_t i;

	for (i = 0; i < uhi_cdc_dev.nb_port; i++) {
		if (uhi_cdc_dev.port[i].line_rx.buffer[0].ptr)
			free(uhi_cdc_dev.port[i].line_rx.buffer[0].ptr);

		if (uhi_cdc_dev.port[i].line_rx.buffer[1].ptr)
			free(uhi_cdc_dev.port[i].line_rx.buffer[1].ptr);

		if (uhi_cdc_dev.port[i].line_tx.buffer[0].ptr)
			free(uhi_cdc_dev.port[i].line_tx.buffer[0].ptr);

		if (uhi_cdc_dev.port[i].line_tx.buffer[1].ptr)
			free(uhi_cdc_dev.port[i].line_tx.buffer[1].ptr);
	}

	free(uhi_cdc_dev.port);
}

static uhi_cdc_port_t *uhi_cdc_get_port(uint8_t port_num)
{
	if (uhi_cdc_dev.dev == NULL)
		return NULL;

	if (port_num >= uhi_cdc_dev.nb_port)
		return NULL;

	return &uhi_cdc_dev.port[port_num];
}

static bool uhi_cdc_set_conf(uint8_t port, CDCLineCoding *configuration)
{
	uhi_cdc_port_t *ptr_port;
	USBGenericRequest req;

	// Select port
	ptr_port = uhi_cdc_get_port(port);

	if (ptr_port == NULL)
		return false;

	memcpy(&ptr_port->conf, configuration, sizeof(CDCLineCoding));

	// Enable configuration
	req.bmRequestType = USB_REQ_RECIP_INTERFACE | USB_REQ_TYPE_CLASS |
						USB_REQ_DIR_OUT;
	req.bRequest = CDCGenericRequest_SETLINECODING;
	req.wValue = 0;
	req.wIndex = ptr_port->iface_comm;
	req.wLength = sizeof(CDCLineCoding);

	if (!USBH_HAL_SetupReq(uhi_cdc_dev.dev->address,
						   &req,
						   (uint8_t *) &ptr_port->conf,
						   sizeof(CDCLineCoding),
						   NULL, NULL))
		return false;

	return true;
}

static bool uhi_cdc_set_ctrl_line(uint8_t port, uint16_t wValue)
{
	uhi_cdc_port_t *ptr_port;
	USBGenericRequest req;

	// Select port
	ptr_port = uhi_cdc_get_port(port);

	if (ptr_port == NULL)
		return false;

	// Enable configuration
	req.bmRequestType = USB_REQ_RECIP_INTERFACE | USB_REQ_TYPE_CLASS |
						USB_REQ_DIR_OUT;
	req.bRequest = CDCGenericRequest_SETCONTROLLINESTATE;
	req.wValue = wValue;
	req.wIndex = ptr_port->iface_comm;
	req.wLength = 0;

	if (!USBH_HAL_SetupReq(uhi_cdc_dev.dev->address,
						   &req,
						   NULL,
						   0,
						   NULL, NULL))
		return false;

	return true;
}

static bool uhi_cdc_rx_update(uhi_cdc_line_t *line)
{
	irqflags_t flags;
	uhi_cdc_buf_t *buf_nosel;
	uhi_cdc_buf_t *buf_sel;

	flags = cpu_irq_save();

	// Check if transfer is already on-going
	if (line->b_trans_ongoing) {
		cpu_irq_restore(flags);
		return false;
	}

	// Search a empty buffer to start a transfer
	buf_sel = &line->buffer[line->buf_sel];
	buf_nosel = &line->buffer[(line->buf_sel == 0) ? 1 : 0];

	if (buf_sel->pos >= buf_sel->nb) {
		// The current buffer has been read
		// then reset it
		buf_sel->pos = 0;
		buf_sel->nb = 0;
	}

	if (!buf_sel->nb && buf_nosel->nb) {
		// New data available then change current buffer
		line->buf_sel = (line->buf_sel == 0) ? 1 : 0;
		buf_nosel = buf_sel;
		UHI_CDC_RX_NOTIFY();
	}

	if (buf_nosel->nb) {
		// No empty buffer available to start a transfer
		cpu_irq_restore(flags);
		return false;
	}

	// Check if transfer must be delayed after the next SOF
	if (uhi_cdc_dev.dev->speed == UHD_SPEED_HIGH) {
		if (line->sof == USBH_HAL_GetMicroFrameNum()) {
			cpu_irq_restore(flags);
			return false;
		}
	} else {
		if (line->sof == USBH_HAL_GetFrameNum()) {
			cpu_irq_restore(flags);
			return false;
		}
	}

	// Start transfer on empty buffer
	line->b_trans_ongoing = true;
	cpu_irq_restore(flags);

	return USBH_HAL_RunEndpoint(
			uhi_cdc_dev.dev->address,
			line->ep_data,
			true,
			buf_nosel->ptr,
			line->buffer_size,
			10,
			uhi_cdc_rx_received);
}

static void uhi_cdc_rx_received(
	USBHS_Add_t add,
	USBHS_Ep_t ep,
	USBH_XfrStatus_t status,
	uint32_t nb_transferred)
{
	uint8_t port = 0;
	uhi_cdc_port_t *ptr_port;
	uhi_cdc_line_t *line;
	uhi_cdc_buf_t *buf;
	UNUSED(add);

	// Search port corresponding at endpoint
	while (1) {
		ptr_port = uhi_cdc_get_port(port++);

		if (ptr_port == NULL)
			return;

		line = &ptr_port->line_rx;

		if (ep == line->ep_data) {
			break; // Port found
		}
	}

	if (UHD_TRANS_NOERROR != status) {
		// Abort transfer
		line->b_trans_ongoing  = false;
		return;
	}

	// Update SOF tag, if it is a short packet
	if (nb_transferred != line->buffer_size) {
		if (uhi_cdc_dev.dev->speed == UHD_SPEED_HIGH)
			line->sof = USBH_HAL_GetMicroFrameNum();
		else
			line->sof = USBH_HAL_GetFrameNum();
	}

	USBHS_SCB_InvalidateDCache_by_Addr((uint32_t *) line->buffer[line->buf_sel].ptr,
									   nb_transferred);

	// Update buffer structure
	buf = &line->buffer[(line->buf_sel == 0) ? 1 : 0];
	buf->pos = 0;
	buf->nb = nb_transferred;
	line->b_trans_ongoing  = false;

	// Manage new transfer
	uhi_cdc_rx_update(line);
}


static bool uhi_cdc_tx_update(uhi_cdc_line_t *line)
{
	irqflags_t flags;
	uhi_cdc_buf_t *buf;

	flags = cpu_irq_save();

	// Check if transfer is already on-going
	if (line->b_trans_ongoing) {
		cpu_irq_restore(flags);
		return false;
	}

	// Check if transfer must be delayed after the next SOF
	if (uhi_cdc_dev.dev->speed == UHD_SPEED_HIGH) {
		if (line->sof == USBH_HAL_GetMicroFrameNum()) {
			cpu_irq_restore(flags);
			return false;
		}
	} else {
		if (line->sof == USBH_HAL_GetFrameNum()) {
			cpu_irq_restore(flags);
			return false;
		}
	}

	// Send the current buffer if not empty
	buf = &line->buffer[line->buf_sel];

	if (buf->nb == 0) {
		cpu_irq_restore(flags);
		return false;
	}

	// Change current buffer to next buffer
	line->buf_sel = (line->buf_sel == 0) ? 1 : 0;

	// Start transfer
	line->b_trans_ongoing = true;
	cpu_irq_restore(flags);

	return USBH_HAL_RunEndpoint(
			uhi_cdc_dev.dev->address,
			line->ep_data,
			true,
			buf->ptr,
			buf->nb,
			1000,
			uhi_cdc_tx_send);
}


static void uhi_cdc_tx_send(
	USBHS_Add_t add,
	USBHS_Ep_t ep,
	USBH_XfrStatus_t status,
	uint32_t nb_transferred)
{
	uint8_t port = 0;
	uhi_cdc_port_t *ptr_port;
	uhi_cdc_line_t *line;
	uhi_cdc_buf_t *buf;
	irqflags_t flags;
	UNUSED(add);

	flags = cpu_irq_save();

	// Search port corresponding at endpoint
	while (1) {
		ptr_port = uhi_cdc_get_port(port++);

		if (ptr_port == NULL) {
			cpu_irq_restore(flags);
			return;
		}

		line = &ptr_port->line_tx;

		if (ep == line->ep_data) {
			break; // Port found
		}
	}

	if (UHD_TRANS_NOERROR != status) {
		// Abort transfer
		line->b_trans_ongoing  = false;
		cpu_irq_restore(flags);
		return;
	}

	// Update SOF tag, if it is a short packet
	if (nb_transferred != line->buffer_size) {
		if (uhi_cdc_dev.dev->speed == UHD_SPEED_HIGH)
			line->sof = USBH_HAL_GetMicroFrameNum();
		else
			line->sof = USBH_HAL_GetFrameNum();
	}

	// Update buffer structure
	buf = &line->buffer[(line->buf_sel == 0) ? 1 : 0 ];
	buf->nb = 0;
	line->b_trans_ongoing  = false;
	cpu_irq_restore(flags);

	// Manage new transfer
	uhi_cdc_tx_update(line);
}
//@}


bool uhi_cdc_open(uint8_t port, CDCLineCoding *configuration)
{
	// Send configuration
	if (!uhi_cdc_set_conf(port, configuration))
		return false;

	// Send DTR
	if (!uhi_cdc_set_ctrl_line(port, CDCControlLineState_DTE_PRESENT))
		return false;

	return true;
}

void uhi_cdc_close(uint8_t port)
{
	// Clear DTR
	uhi_cdc_set_ctrl_line(port, 0);
}

bool uhi_cdc_is_rx_ready(uint8_t port)
{
	return (0 != uhi_cdc_get_nb_received(port));
}

uint32_t uhi_cdc_get_nb_received(uint8_t port)
{
	uhi_cdc_port_t *ptr_port;
	uhi_cdc_buf_t *buf;

	// Select port
	ptr_port = uhi_cdc_get_port(port);

	if (ptr_port == NULL)
		return false;

	// Check available data
	buf = &ptr_port->line_rx.buffer[ ptr_port->line_rx.buf_sel ];
	return (buf->nb - buf->pos);
}

int uhi_cdc_getc(uint8_t port)
{
	uhi_cdc_port_t *ptr_port;
	uhi_cdc_line_t *line;
	uhi_cdc_buf_t *buf;
	int rx_data = 0;
	bool b_databit_9;

	// Select port
	ptr_port = uhi_cdc_get_port(port);

	if (ptr_port == NULL)
		return false;

	line = &ptr_port->line_rx;

	b_databit_9 = (9 == ptr_port->conf.bDataBits);

uhi_cdc_getc_process_one_byte:
	// Check available data
	buf = &line->buffer[line->buf_sel];

	while (buf->pos >= buf->nb) {
		if (NULL == uhi_cdc_get_port(port))
			return 0;

		uhi_cdc_rx_update(line);
		goto uhi_cdc_getc_process_one_byte;
	}

	// Read data
	rx_data |= buf->ptr[buf->pos];
	buf->pos++;

	uhi_cdc_rx_update(line);

	if (b_databit_9) {
		// Receive MSB
		b_databit_9 = false;
		rx_data = rx_data << 8;
		goto uhi_cdc_getc_process_one_byte;
	}

	return rx_data;
}

uint32_t uhi_cdc_read_buf(uint8_t port, void *buf, uint32_t size)
{
	uhi_cdc_port_t *ptr_port;
	uhi_cdc_line_t *line;
	uhi_cdc_buf_t *cdc_buf;
	uint32_t copy_nb;

	// Select port
	ptr_port = uhi_cdc_get_port(port);

	if (ptr_port == NULL)
		return false;

	line = &ptr_port->line_rx;


uhi_cdc_read_buf_loop_wait:
	// Check available data
	cdc_buf = &line->buffer[line->buf_sel];

	while (cdc_buf->pos >= cdc_buf->nb) {
		if (NULL == uhi_cdc_get_port(port))
			return 0;

		uhi_cdc_rx_update(line);
		goto uhi_cdc_read_buf_loop_wait;
	}

	// Read data
	copy_nb = cdc_buf->nb - cdc_buf->pos;

	if (copy_nb > size)
		copy_nb = size;

	memcpy(buf, &cdc_buf->ptr[cdc_buf->pos], copy_nb);
	cdc_buf->pos += copy_nb;
	buf = (uint8_t *)buf + copy_nb;
	size -= copy_nb;

	uhi_cdc_rx_update(line);

	if (size)
		goto uhi_cdc_read_buf_loop_wait;

	return 0;
}


bool uhi_cdc_is_tx_ready(uint8_t port)
{
	uhi_cdc_port_t *ptr_port;
	uhi_cdc_line_t *line;

	ptr_port = uhi_cdc_get_port(port);

	if (ptr_port == NULL)
		return false;

	line = &ptr_port->line_tx;

	return (line->buffer_size != line->buffer[line->buf_sel].nb);
}


int uhi_cdc_putc(uint8_t port, int value)
{
	irqflags_t flags;
	uhi_cdc_port_t *ptr_port;
	uhi_cdc_line_t *line;
	uhi_cdc_buf_t *buf;
	bool b_databit_9;

	// Select port
	ptr_port = uhi_cdc_get_port(port);

	if (ptr_port == NULL)
		return false;

	line = &ptr_port->line_tx;

	b_databit_9 = (9 == ptr_port->conf.bDataBits);

uhi_cdc_putc_process_one_byte:
	// Check available space
	buf = &line->buffer[line->buf_sel];

	while (line->buffer_size == buf->nb) {
		if (NULL == uhi_cdc_get_port(port))
			return false;

		goto uhi_cdc_putc_process_one_byte;
	}

	// Write value
	flags = cpu_irq_save();
	buf = &line->buffer[line->buf_sel];
	buf->ptr[buf->nb++] = value;
	cpu_irq_restore(flags);

	if (b_databit_9) {
		// Send MSB
		b_databit_9 = false;
		value = value >> 8;
		goto uhi_cdc_putc_process_one_byte;
	}

	return true;
}

uint32_t uhi_cdc_write_buf(uint8_t port, const void *buf, uint32_t size)
{
	irqflags_t flags;
	uhi_cdc_port_t *ptr_port;
	uhi_cdc_line_t *line;
	uhi_cdc_buf_t *cdc_buf;
	uint32_t copy_nb;

	// Select port
	ptr_port = uhi_cdc_get_port(port);

	if (ptr_port == NULL)
		return false;

	line = &ptr_port->line_tx;

	if (9 == ptr_port->conf.bDataBits)
		size *= 2;

uhi_cdc_write_buf_loop_wait:
	// Check available space
	cdc_buf = &line->buffer[line->buf_sel];

	while (line->buffer_size == cdc_buf->nb) {
		if (NULL == uhi_cdc_get_port(port))
			return 0;

		goto uhi_cdc_write_buf_loop_wait;
	}

	// Write value
	flags = cpu_irq_save();
	cdc_buf = &line->buffer[line->buf_sel];
	copy_nb = line->buffer_size - cdc_buf->nb;

	if (copy_nb > size)
		copy_nb = size;

	memcpy(&cdc_buf->ptr[cdc_buf->nb], buf, copy_nb);
	cdc_buf->nb += copy_nb;
	cpu_irq_restore(flags);

	// Update buffer pointer
	buf = (uint8_t *)buf + copy_nb;
	size -= copy_nb;

	if (size)
		goto uhi_cdc_write_buf_loop_wait;

	return 0;
}

//@}
