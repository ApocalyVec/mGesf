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
 * \brief USB host Human Interface Device (HID) mouse driver.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "conf_usb_host.h"
#include "USBHDriver.h"
#include "USBH.h"
#include "uhi_hid_mouse.h"
#include <string.h>

#ifdef USB_HOST_HUB_SUPPORT
	# error USB HUB support is not implemented on UHI mouse
#endif

/**
 * \ingroup uhi_hid_mouse_group
 * \defgroup uhi_hid_mouse_group_internal Implementation of UHI HID Mouse
 *
 * Class internal implementation
 * @{
 */

/**
 * \name Index in HID report for usual HID mouse events
 * @{
 */
#define UHI_HID_MOUSE_BTN        0
#define UHI_HID_MOUSE_MOV_X      1
#define UHI_HID_MOUSE_MOV_Y      2
#define UHI_HID_MOUSE_MOV_SCROLL 3
//@}

/**
 * \name Structure to store information about USB Device HID mouse
 */
//@{
typedef struct {
	USBH_device_t *dev;
	uint8_t ep_in;
	uint8_t report_size;
	uint8_t *report;
	uint8_t report_btn_prev;
} uhi_hid_mouse_dev_t;

static uhi_hid_mouse_dev_t uhi_hid_mouse_dev = {
	.dev = NULL,
	.report = NULL,
};

//@}


/**
 * \name Internal routines
 */
//@{
static void uhi_hid_mouse_start_trans_report(USBHS_Add_t add);
static void uhi_hid_mouse_report_reception(
	USBHS_Add_t add,
	USBHS_Ep_t ep,
	USBH_XfrStatus_t status,
	uint32_t nb_transfered);
//@}


/**
 * \name Functions required by UHC
 * @{
 */

USBH_enum_status_t uhi_hid_mouse_install(USBH_device_t *dev)
{
	bool b_iface_supported;
	uint16_t conf_desc_lgt;
	USBInterfaceDescriptor *ptr_iface;

	if (uhi_hid_mouse_dev.dev != NULL) {
		return UHC_ENUM_SOFTWARE_LIMIT; // Device already allocated
	}

	conf_desc_lgt = (dev->conf_desc->wTotalLength);
	ptr_iface = (USBInterfaceDescriptor *)dev->conf_desc;
	b_iface_supported = false;

	while (conf_desc_lgt) {
		switch (ptr_iface->bDescriptorType) {

		case USBGenericDescriptor_INTERFACE:
			if ((ptr_iface->bInterfaceClass   == HIDInterfaceDescriptor_CLASS)
				&& (ptr_iface->bInterfaceProtocol == HIDInterfaceDescriptor_PROTOCOL_MOUSE)) {
				// USB HID Mouse interface found
				// Start allocation endpoint(s)
				b_iface_supported = true;
			} else {
				// Stop allocation endpoint(s)
				b_iface_supported = false;
			}

			break;

		case USBGenericDescriptor_ENDPOINT:

			//  Allocation of the endpoint
			if (!b_iface_supported)
				break;

			if (!USBH_HAL_ConfigurePipe(dev->address, (USBEndpointDescriptor *)ptr_iface)) {
				return UHC_ENUM_HARDWARE_LIMIT; // Endpoint allocation fail
			}

			assert(((USBEndpointDescriptor *)ptr_iface)->bEndpointAddress & USB_EP_DIR_IN);
			uhi_hid_mouse_dev.ep_in = ((USBEndpointDescriptor *)
									   ptr_iface)->bEndpointAddress;
			uhi_hid_mouse_dev.report_size =
				(((USBEndpointDescriptor *)ptr_iface)->wMaxPacketSize);
			uhi_hid_mouse_dev.report = malloc(uhi_hid_mouse_dev.report_size);

			if (uhi_hid_mouse_dev.report == NULL) {
				assert(false);
				return UHC_ENUM_MEMORY_LIMIT; // Internal RAM allocation fail
			}

			uhi_hid_mouse_dev.dev = dev;
			// All endpoints of all interfaces supported allocated
			return UHC_ENUM_SUCCESS;

		default:
			// Ignore descriptor
			break;
		}

		assert(conf_desc_lgt >= ptr_iface->bLength);
		conf_desc_lgt -= ptr_iface->bLength;
		ptr_iface = (USBInterfaceDescriptor *)((uint8_t *)ptr_iface +
											   ptr_iface->bLength);
	}

	return UHC_ENUM_UNSUPPORTED; // No interface supported
}

void uhi_hid_mouse_enable(USBH_device_t *dev)
{
	if (uhi_hid_mouse_dev.dev != dev) {
		return;  // No interface to enable
	}

	// Init value
	uhi_hid_mouse_dev.report_btn_prev = 0;
	uhi_hid_mouse_start_trans_report(dev->address);
	UHI_HID_MOUSE_CHANGE(dev, true);
}

void uhi_hid_mouse_uninstall(USBH_device_t *dev)
{
	if (uhi_hid_mouse_dev.dev != dev) {
		return; // Device not enabled in this interface
	}

	uhi_hid_mouse_dev.dev = NULL;
	assert(uhi_hid_mouse_dev.report != NULL);
	free(uhi_hid_mouse_dev.report);
	UHI_HID_MOUSE_CHANGE(dev, false);
}
//@}

/**
 * \name Internal routines
 */
//@{

/**
 * \brief Starts the reception of the HID mouse report
 *
 * \param add   USB address to use
 */
static void uhi_hid_mouse_start_trans_report(USBHS_Add_t add)
{
	// Start transfer on interrupt endpoint IN
	USBH_HAL_RunEndpoint(add, uhi_hid_mouse_dev.ep_in, true,
						 uhi_hid_mouse_dev.report,
						 uhi_hid_mouse_dev.report_size, 0, uhi_hid_mouse_report_reception);
}

/**
 * \brief Decodes the HID mouse report received
 *
 * \param add           USB address used by the transfer
 * \param status        Transfer status
 * \param nb_transfered Number of data transferred
 */
static void uhi_hid_mouse_report_reception(
	USBHS_Add_t add,
	USBHS_Ep_t ep,
	USBH_XfrStatus_t status,
	uint32_t nb_transfered)
{
	uint8_t state_prev;
	uint8_t state_new;
	UNUSED(ep);

	if ((status == UHD_TRANS_NOTRESPONDING) || (status == UHD_TRANS_TIMEOUT)) {
		uhi_hid_mouse_start_trans_report(add);
		return; // HID mouse transfer restart
	}

	if ((status != UHD_TRANS_NOERROR) || (nb_transfered < 4)) {
		return; // HID mouse transfer aborted
	}

	SCB_InvalidateDCache_by_Addr((uint32_t *)uhi_hid_mouse_dev.report,
								 nb_transfered);
	// Decode buttons
	state_prev = uhi_hid_mouse_dev.report_btn_prev;
	state_new = uhi_hid_mouse_dev.report[UHI_HID_MOUSE_BTN];

	if ((state_prev & 0x01) != (state_new & 0x01))
		UHI_HID_MOUSE_EVENT_BTN_LEFT((state_new & 0x01) ? true : false);

	if ((state_prev & 0x02) != (state_new & 0x02))
		UHI_HID_MOUSE_EVENT_BTN_RIGHT((state_new & 0x02) ? true : false);

	if ((state_prev & 0x04) != (state_new & 0x04))
		UHI_HID_MOUSE_EVENT_BTN_MIDDLE((state_new & 0x04) ? true : false);

	uhi_hid_mouse_dev.report_btn_prev = state_new;

	// Decode moves
	if ((uhi_hid_mouse_dev.report[UHI_HID_MOUSE_MOV_X] != 0)
		|| (uhi_hid_mouse_dev.report[UHI_HID_MOUSE_MOV_Y] != 0)
		|| (uhi_hid_mouse_dev.report[UHI_HID_MOUSE_MOV_SCROLL] != 0)) {
		UHI_HID_MOUSE_EVENT_MOUVE(
			(int8_t)uhi_hid_mouse_dev.report[UHI_HID_MOUSE_MOV_X],
			(int8_t)uhi_hid_mouse_dev.report[UHI_HID_MOUSE_MOV_Y],
			(int8_t)uhi_hid_mouse_dev.report[UHI_HID_MOUSE_MOV_SCROLL]);
	}

	uhi_hid_mouse_start_trans_report(add);
}
//@}

//@}
