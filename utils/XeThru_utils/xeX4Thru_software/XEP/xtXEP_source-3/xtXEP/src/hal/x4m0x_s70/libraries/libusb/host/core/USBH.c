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
 * \brief USB Host Controller (UHC)
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "conf_usb_host.h"
#include "USBHDriver.h"
#include "uhi.h"
#include "USBH.h"
#include <stdlib.h>

#ifndef USB_HOST_UHI
	#  error USB_HOST_UHI must be defined with unless one UHI interface in conf_usb_host.h file.
#endif

#ifdef USB_HOST_HUB_SUPPORT
	#  error The USB HUB support is not available in this revision.
#endif

// Optional UHC callbacks
#ifndef UHC_CONNECTION_EVENT
	#define UHC_CONNECTION_EVENT(dev,b_present)
#endif
#ifndef UHC_DEVICE_CONF
	#define UHC_DEVICE_CONF(dev) 1
#endif
#ifndef UHC_ENUM_EVENT
	#define UHC_ENUM_EVENT(dev,event)
#endif
#ifndef UHC_WAKEUP_EVENT
	#define UHC_WAKEUP_EVENT()
#endif

/**
 * \ingroup USBH_group
 * \defgroup USBH_group_interne Implementation of UHC
 *
 * Internal implementation
 * @{
 */

//! \name Internal variables to manage the USB host stack
//! @{

//! Entry point of all devices connected on USB tree
static USBH_device_t USBH_device_root;

//! Number of enumeration try
static uint8_t USBH_enum_try;

//! Maximum try to enumerate a device
#define UHC_ENUM_NB_TRY 4

//! Entry point of all devices connected on USB tree
#define UHC_USB_ADD_NOT_VALID 0xFF

#ifdef USB_HOST_HUB_SUPPORT
	//! USB address of the USB device under enumeration process
	#define UHC_DEVICE_ENUM_ADD USBH_dev_enum->address

	//! Device under enumeration process
	static USBH_device_t *USBH_dev_enum;

	//! Total power of the connected devices
	static uint16_t USBH_power_running;

#else
	//! USB address of the USB device under enumeration process
	#define UHC_DEVICE_ENUM_ADD 1 // Static without USB HUB

	//! Device under enumeration process
	#define USBH_dev_enum        (&USBH_device_root) // Static without USB HUB

	//! Total power of the devices connected
	#define USBH_power_running   0 // No used without USB HUB
#endif

//! Type of callback on a SOF timeout
typedef void (*USBH_sof_timeout_callback_t) (void);

//! Callback currently registered on a SOF timeout
static USBH_sof_timeout_callback_t USBH_sof_timeout_callback;

//! Number of SOF remaining before call USBH_sof_timeout_callback callback
uint8_t USBH_sof_timeout;

//! Array of all UHI available
static uhi_api_t USBH_uhis[] = {USB_HOST_UHI};

//! Number of UHI available
#define UHC_NB_UHI  (sizeof(USBH_uhis)/sizeof(USBH_uhis[0]))

//! Volatile flag to pool the end of Get USB string setup request
static volatile bool USBH_setup_request_finish;

//! Volatile flag to know the status of Get USB string setup request
static volatile bool USBH_setup_request_finish_status;

//! @}


//! \name Internal functions to manage the USB device enumeration
//! @{
static void USBH_enable_timeout_callback(
	uint8_t timeout,
	USBH_sof_timeout_callback_t callback);
static void USBH_enumeration_suspend(void);
static void USBH_enumeration_reset(uhd_callback_reset_t callback);
static void USBH_connection_tree(bool b_plug, USBH_device_t *dev);
static void USBH_enumeration_step1(void);
static void USBH_enumeration_step2(void);
static void USBH_enumeration_step3(void);
static void USBH_enumeration_step4(void);
static void USBH_enumeration_step5(void);
static void USBH_enumeration_step6(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans);
static void USBH_enumeration_step7(void);
static void USBH_enumeration_step8(void);
static void USBH_enumeration_step9(void);
static void USBH_enumeration_step10(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans);
static void USBH_enumeration_step11(void);
static void USBH_enumeration_step12(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans);
static void USBH_enumeration_step13(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans);
static void USBH_enumeration_step14(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans);
static void USBH_enumeration_step15(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans);
#ifdef USB_HOST_LPM_SUPPORT
static void USBH_enumeration_step16_lpm(void);
static void USBH_enumeration_step17_lpm(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans);
#endif // USB_HOST_LPM_SUPPORT
static void USBH_enumeration_error(USBH_enum_status_t status);
//! @}

static void USBH_remotewakeup(bool b_enable);
static void USBH_setup_request_callback(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans);



/**
 * \brief Enable a internal timeout on SOF events
 *
 * \param timeout  value of timeout (ms)
 * \param callback Callback to call at the end of timeout
 */
static void USBH_enable_timeout_callback(uint8_t timeout,
		USBH_sof_timeout_callback_t callback)
{
	USBH_sof_timeout_callback = callback;
	USBH_sof_timeout = timeout;
}

/**
 * \brief Enters a specific device in USB suspend mode
 * Suspend the USB line or a port on USB hub.
 */
static void USBH_enumeration_suspend(void)
{
#ifdef USB_HOST_HUB_SUPPORT

	if (&USBH_device_root != USBH_dev_enum) {
		// Device connected on USB hub
		uhi_hub_suspend(USBH_dev_enum);
	} else
#endif
	{
		// Suspend USB line
		USBH_HAL_Suspend();
	}
}

/**
 * \brief Sends the USB Reset signal on the USB line of a device
 *
 * \param callback Callback to call at the end of Reset signal
 */
static void USBH_enumeration_reset(uhd_callback_reset_t callback)
{
	// Reset USB line
#ifdef USB_HOST_HUB_SUPPORT
	if (&USBH_device_root != USBH_dev_enum) {
		// Device connected on USB hub
		uhi_hub_send_reset(USBH_dev_enum, callback);
	} else
#endif
	{
		USBH_HAL_Reset(callback);
	}
}

/**
 * \brief Manage a device plug or unplug on the USB tree
 *
 * \param b_plug   true, if it is a device connection
 * \param dev      Information about device connected or disconnected
 */
static void USBH_connection_tree(bool b_plug, USBH_device_t *dev)
{
	uint8_t i;

	if (b_plug) {
		TRACE_INFO_WP("DevAdd ");
		USBH_enum_try = 1;
#ifdef USB_HOST_HUB_SUPPORT
		USBH_dev_enum = dev;
#endif
		USBH_dev_enum->conf_desc = NULL;
		USBH_dev_enum->address = 0;
		UHC_CONNECTION_EVENT(USBH_dev_enum, true);
		USBH_enumeration_step1();
	} else {
		TRACE_INFO_WP("DevRem");

		if (USBH_dev_enum == dev) {
			// Eventually stop enumeration timeout on-going on this device
			USBH_sof_timeout = 0;
		}

		// Abort all transfers (endpoint control and other) and free pipe(s)
		USBH_HAL_FreePipe(dev->address, 0xFF);

		// Disable all USB interfaces (this includes HUB interface)
		for (i = 0; i < UHC_NB_UHI; i++)
			USBH_uhis[i].uninstall(dev);

		UHC_CONNECTION_EVENT(dev, false);
		dev->address = UHC_USB_ADD_NOT_VALID;

		// Free USB configuration descriptor buffer
		if (dev->conf_desc != NULL)
			free(dev->conf_desc);

#ifdef USB_HOST_HUB_SUPPORT
		USBH_power_running -= dev->power;

		if (&USBH_device_root != dev) {
			// It is on a USB hub
			dev->prev->next = dev->next;
			dev->next->prev = dev->prev;
			free(dev);
		}

#endif
	}
}

/**
 * \brief Device enumeration step 1
 * Reset USB line.
 */
static void USBH_enumeration_step1(void)
{
	TRACE_INFO_WP("Enum1 ");
	USBH_enumeration_reset(USBH_enumeration_step2);
}

/**
 * \brief Device enumeration step 2
 * Lets USB line in IDLE state during 20ms.
 */
static void USBH_enumeration_step2(void)
{
	TRACE_INFO_WP("Enum2 ");
	USBH_enable_timeout_callback(20, USBH_enumeration_step3);
}

/**
 * \brief Device enumeration step 3
 * Reset USB line.
 */
static void USBH_enumeration_step3(void)
{
	TRACE_INFO_WP("Enum3 ");
	USBH_enumeration_reset(USBH_enumeration_step4);
}

/**
 * \brief Device enumeration step 4
 * Lets USB line in IDLE state during 100ms.
 */
static void USBH_enumeration_step4(void)
{
	TRACE_INFO_WP("Enum4 ");
	USBH_dev_enum->speed = USBH_HAL_GetSpeed();
	USBH_enable_timeout_callback(100, USBH_enumeration_step5);
}

/**
 * \brief Device enumeration step 5
 * Requests the USB device descriptor.
 * This setup request can be aborted
 * because the control endpoint size is unknown.
 */
static void USBH_enumeration_step5(void)
{
	USBGenericRequest req;
	TRACE_INFO_WP("Enum5 ");
	req.bmRequestType = USB_REQ_RECIP_DEVICE | USB_REQ_TYPE_STANDARD |
						USB_REQ_DIR_IN;
	req.bRequest = USB_REQ_GET_DESCRIPTOR;
	req.wValue = (USBGenericDescriptor_DEVICE << 8);
	req.wIndex = 0;
	req.wLength = offsetof(USBH_device_t, dev_desc.bMaxPacketSize0)
				  + sizeof(USBH_dev_enum->dev_desc.bMaxPacketSize0);

	// After a USB reset, the reallocation is required
	USBH_HAL_FreePipe(0, 0);

	if (!USBH_HAL_ConfigureControlPipe(0, 64)) {
		USBH_enumeration_error(UHC_ENUM_HARDWARE_LIMIT);
		return;
	}

	if (!USBH_HAL_SetupReq(0,
						   &req,
						   (uint8_t *)&USBH_dev_enum->dev_desc,
						   sizeof(USBDeviceDescriptor),
						   NULL,
						   USBH_enumeration_step6)) {
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}
}

/**
 * \brief Device enumeration step 6
 * End of Get device descriptor request.
 * Wait 20ms in IDLE state.
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
static void USBH_enumeration_step6(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans)
{
	UNUSED(add);
	TRACE_INFO_WP("Enum6 ");

	if ((status != UHD_TRANS_NOERROR) || (payload_trans < 8)
		|| (USBH_dev_enum->dev_desc.bDescriptorType != USBGenericDescriptor_DEVICE)) {
		USBH_enumeration_error((status == UHD_TRANS_DISCONNECT) ?
							   UHC_ENUM_DISCONNECT : UHC_ENUM_FAIL);
		return;
	}

	// Wait 20ms
	USBH_enable_timeout_callback(20, USBH_enumeration_step7);
}

/**
 * \brief Device enumeration step 7
 * Reset USB line.
 */
static void USBH_enumeration_step7(void)
{
	TRACE_INFO_WP("Enum7 ");
	USBH_enumeration_reset(USBH_enumeration_step8);
}

/**
 * \brief Device enumeration step 8
 * Lets USB line in IDLE state during 100ms.
 */
static void USBH_enumeration_step8(void)
{
	// Wait 100ms
	TRACE_INFO_WP("Enum8 ");
	USBH_enable_timeout_callback(100, USBH_enumeration_step9);
}

/**
 * \brief Device enumeration step 9
 * Send a Set address setup request.
 */
static void USBH_enumeration_step9(void)
{
	USBGenericRequest req;

	TRACE_INFO_WP("Enum9 ");
	req.bmRequestType = USB_REQ_RECIP_DEVICE
						| USB_REQ_TYPE_STANDARD | USB_REQ_DIR_OUT;
	req.bRequest = USB_REQ_SET_ADDRESS;
#ifdef USB_HOST_HUB_SUPPORT
	uint8_t usb_addr_free = 0;
	USBH_device_t *dev;

	// Search free address
	dev = &USBH_device_root;

	while (usb_addr_free++) {
		if (dev->address == usb_addr_free)
			continue;

		if (dev->next != NULL) {
			dev = dev->next;
			continue;
		}

		break;
	}

	req.wValue = usb_addr_free;
	USBH_dev_enum->address = usb_addr_free;
#else
	req.wValue = UHC_DEVICE_ENUM_ADD;
	USBH_dev_enum->address = UHC_DEVICE_ENUM_ADD;
#endif
	req.wIndex = 0;
	req.wLength = 0;

	// After a USB reset, the reallocation is required
	USBH_HAL_FreePipe(0, 0);

	if (!USBH_HAL_ConfigureControlPipe(0,
									   USBH_dev_enum->dev_desc.bMaxPacketSize0)) {
		USBH_enumeration_error(UHC_ENUM_HARDWARE_LIMIT);
		return;
	}

	if (!USBH_HAL_SetupReq(0,
						   &req,
						   (uint8_t *)&USBH_dev_enum->dev_desc,
						   sizeof(USBDeviceDescriptor),
						   NULL,
						   USBH_enumeration_step10)) {
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}
}

/**
 * \brief Device enumeration step 10
 * End of Set address request.
 * Lets USB line in IDLE state during 20ms.
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
static void USBH_enumeration_step10(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans)
{
	UNUSED(add);
	UNUSED(payload_trans);
	TRACE_INFO_WP("Enum10 ");

	if (status != UHD_TRANS_NOERROR) {
		USBH_enumeration_error((status == UHD_TRANS_DISCONNECT) ?
							   UHC_ENUM_DISCONNECT : UHC_ENUM_FAIL);
		return;
	}

	// Wait 20ms
	USBH_enable_timeout_callback(20, USBH_enumeration_step11);
}

/**
 * \brief Device enumeration step 11
 * Updates USB host pipe with the new USB address.
 * Requests a complete USB device descriptor.
 */
static void USBH_enumeration_step11(void)
{
	USBGenericRequest req;

	TRACE_INFO_WP("Enum11 ");
	// Free address 0 used to start enumeration
	USBH_HAL_FreePipe(0, 0);

	// Alloc control endpoint with the new USB address
	if (!USBH_HAL_ConfigureControlPipe(UHC_DEVICE_ENUM_ADD,
									   USBH_dev_enum->dev_desc.bMaxPacketSize0)) {
		USBH_enumeration_error(UHC_ENUM_HARDWARE_LIMIT);
		return;
	}

	// Send USB device descriptor request
	req.bmRequestType = USB_REQ_RECIP_DEVICE | USB_REQ_TYPE_STANDARD |
						USB_REQ_DIR_IN;
	req.bRequest = USB_REQ_GET_DESCRIPTOR;
	req.wValue = (USBGenericDescriptor_DEVICE << 8);
	req.wIndex = 0;
	req.wLength = sizeof(USBDeviceDescriptor);

	if (!USBH_HAL_SetupReq(UHC_DEVICE_ENUM_ADD,
						   &req,
						   (uint8_t *) & USBH_dev_enum->dev_desc,
						   sizeof(USBDeviceDescriptor),
						   NULL, USBH_enumeration_step12)) {
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}
}

/**
 * \brief Device enumeration step 12
 * Requests the first USB structure of the USB configuration descriptor.
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
static void USBH_enumeration_step12(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans)
{
	USBGenericRequest req;
	uint8_t conf_num;
	UNUSED(add);

	TRACE_INFO_WP("Enum12 ");

	if ((status != UHD_TRANS_NOERROR)
		|| (payload_trans != sizeof(USBDeviceDescriptor))
		|| (USBH_dev_enum->dev_desc.bDescriptorType != USBGenericDescriptor_DEVICE)) {
		USBH_enumeration_error((status == UHD_TRANS_DISCONNECT) ?
							   UHC_ENUM_DISCONNECT : UHC_ENUM_FAIL);
		return;
	}

	// Choose USB device configuration
	if (USBH_dev_enum->dev_desc.bNumConfigurations > 1)
		conf_num = UHC_DEVICE_CONF(USBH_dev_enum);
	else
		conf_num = 1;

	USBH_dev_enum->conf_desc = malloc(sizeof(USBConfigurationDescriptor));

	if (USBH_dev_enum->conf_desc == NULL) {
		assert(false);
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}

	// Send USB device descriptor request
	req.bmRequestType = USB_REQ_RECIP_DEVICE | USB_REQ_TYPE_STANDARD |
						USB_REQ_DIR_IN;
	req.bRequest = USB_REQ_GET_DESCRIPTOR;
	req.wValue = (USBGenericDescriptor_CONFIGURATION << 8) | (conf_num - 1);
	req.wIndex = 0;
	req.wLength = sizeof(USBConfigurationDescriptor);

	if (!USBH_HAL_SetupReq(UHC_DEVICE_ENUM_ADD,
						   &req,
						   (uint8_t *) USBH_dev_enum->conf_desc,
						   sizeof(USBConfigurationDescriptor),
						   NULL, USBH_enumeration_step13)) {
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}
}

/**
 * \brief Device enumeration step 13
 * Requests a complete Get configuration descriptor.
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
static void USBH_enumeration_step13(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans)
{
	uint8_t conf_num;
	uint16_t conf_size;
	uint16_t bus_power = 0;
	USBGenericRequest req;
	UNUSED(add);

	TRACE_INFO_WP("Enum13 ");

	if ((status != UHD_TRANS_NOERROR)
		|| (payload_trans != sizeof(USBConfigurationDescriptor))
		|| (USBH_dev_enum->conf_desc->bDescriptorType !=
			USBGenericDescriptor_CONFIGURATION)) {
		USBH_enumeration_error((status == UHD_TRANS_DISCONNECT) ?
							   UHC_ENUM_DISCONNECT : UHC_ENUM_FAIL);
		return;
	}

#ifdef USB_HOST_HUB_SUPPORT
	USBH_device_t *dev;
	dev = USBH_dev_enum;

	while (1) {
		if (dev->conf_desc->bmAttributes & USB_CONFIG_ATTR_SELF_POWERED) {
			// The device or a parent HUB is SELF power, then no power on root
			break;
		}

		if (dev == (&USBH_device_root)) {
			bus_power = USBH_dev_enum->conf_desc->bMaxPower * 2;
			break; // End of USB tree
		}

		// Go to USB HUB parent
		dev = dev->hub;
	}

#else

	if (!(USBH_dev_enum->conf_desc->bmAttributes
		  &USBConfigurationDescriptor_SELFPOWERED_NORWAKEUP))
		bus_power = USBH_dev_enum->conf_desc->bMaxPower * 2;

#endif

	if ((bus_power + USBH_power_running) > USB_HOST_POWER_MAX) {
		// USB interfaces consumption too high
		UHC_ENUM_EVENT(USBH_dev_enum, UHC_ENUM_OVERCURRENT);

		// Abort enumeration, set line in suspend mode
		USBH_enumeration_suspend();
		return;
	}

#ifdef USB_HOST_HUB_SUPPORT
	USBH_dev_enum->power = bus_power;
	USBH_power_running += bus_power;
#endif

	// Save information about USB configuration descriptor size
	conf_size = (USBH_dev_enum->conf_desc->wTotalLength);
	conf_num = USBH_dev_enum->conf_desc->bConfigurationValue;
	assert(conf_num);
	// Re alloc USB configuration descriptor
	free(USBH_dev_enum->conf_desc);
	USBH_dev_enum->conf_desc = malloc(conf_size);

	if (USBH_dev_enum->conf_desc == NULL) {
		assert(false);
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}

	// Send USB device descriptor request
	req.bmRequestType =
		USB_REQ_RECIP_DEVICE | USB_REQ_TYPE_STANDARD |
		USB_REQ_DIR_IN;
	req.bRequest = USB_REQ_GET_DESCRIPTOR;
	req.wValue = (USBGenericDescriptor_CONFIGURATION << 8) | (conf_num - 1);
	req.wIndex = 0;
	req.wLength = conf_size;

	if (!USBH_HAL_SetupReq(UHC_DEVICE_ENUM_ADD,
						   &req,
						   (uint8_t *) USBH_dev_enum->conf_desc,
						   conf_size,
						   NULL, USBH_enumeration_step14)) {
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}
}

/**
 * \brief Device enumeration step 14
 * Enable USB configuration, if unless one USB interface is supported by UHIs.
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
static void USBH_enumeration_step14(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans)
{
	USBGenericRequest req;
	uint8_t i;
	bool b_conf_supported = false;
	UNUSED(add);

	TRACE_INFO_WP("Enum14 ");

	if ((status != UHD_TRANS_NOERROR)
		|| (payload_trans < sizeof(USBConfigurationDescriptor))
		|| (USBH_dev_enum->conf_desc->bDescriptorType !=
			USBGenericDescriptor_CONFIGURATION)
		|| (payload_trans != (USBH_dev_enum->conf_desc->wTotalLength))) {
		USBH_enumeration_error((status == UHD_TRANS_DISCONNECT) ?
							   UHC_ENUM_DISCONNECT : UHC_ENUM_FAIL);
		return;
	}

	// Check if unless one USB interface is supported by UHIs
	for (i = 0; i < UHC_NB_UHI; i++) {
		switch (USBH_uhis[i].install(USBH_dev_enum)) {
		case UHC_ENUM_SUCCESS:
			TRACE_INFO("Device Driver installed");
			b_conf_supported = true;
			break;

		case UHC_ENUM_UNSUPPORTED:
			TRACE_INFO(" \n\rUnsupported Device");
			TRACE_INFO(" \n\rRestarting enumeration");
			break;

		default:
			// USB host hardware limitation
			// Free all endpoints
			USBH_HAL_FreePipe(UHC_DEVICE_ENUM_ADD, 0xFF);
			UHC_ENUM_EVENT(USBH_dev_enum, UHC_ENUM_HARDWARE_LIMIT);

			// Abort enumeration, set line in suspend mode
			USBH_enumeration_suspend();
			return;
		}
	}

	if (!b_conf_supported) {
		// No USB interface supported
		UHC_ENUM_EVENT(USBH_dev_enum, UHC_ENUM_UNSUPPORTED);

		// Abort enumeration, set line in suspend mode
		USBH_enumeration_suspend();
		return;
	}

	// Enable device configuration
	req.bmRequestType = USB_REQ_RECIP_DEVICE
						| USB_REQ_TYPE_STANDARD | USB_REQ_DIR_OUT;
	req.bRequest = USB_REQ_SET_CONFIGURATION;
	req.wValue = USBH_dev_enum->conf_desc->bConfigurationValue;
	req.wIndex = 0;
	req.wLength = 0;

	if (!USBH_HAL_SetupReq(UHC_DEVICE_ENUM_ADD,
						   &req,
						   NULL,
						   0,
						   NULL, USBH_enumeration_step15)) {
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}
}

/**
 * \brief Device enumeration step 15
 * Enables UHI interfaces
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
static void USBH_enumeration_step15(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans)
{
	UNUSED(add);
	uint8_t i;
	TRACE_INFO_WP("Enum15 ");

	if ((status != UHD_TRANS_NOERROR) || (payload_trans != 0)) {
		for (i = 0; i < UHC_NB_UHI; i++)
			USBH_uhis[i].uninstall(USBH_dev_enum);

		USBH_enumeration_error((status == UHD_TRANS_DISCONNECT) ?
							   UHC_ENUM_DISCONNECT : UHC_ENUM_FAIL);
		return;
	}

	// Enable all UHIs supported
	for (i = 0; i < UHC_NB_UHI; i++)
		USBH_uhis[i].enable(USBH_dev_enum);

#ifdef USB_HOST_LPM_SUPPORT

	// Check LPM support
	if (USBH_device_root.dev_desc.bcdUSB >= (USBDeviceDescriptor_USB2_00)) {
		// Device can support LPM
		// Start LPM support check
		USBH_enumeration_step16_lpm();
		return;
	}

	USBH_dev_enum->lpm_desc = NULL;
#endif

	USBH_enum_try = 0;

	UHC_ENUM_EVENT(USBH_dev_enum, UHC_ENUM_SUCCESS);
}

#ifdef USB_HOST_LPM_SUPPORT
/**
 * \brief Device enumeration step 16 (LPM only)
 * Requests the USB structure of the USB BOS / LPM descriptor.
 */
static void USBH_enumeration_step16_lpm(void)
{
	USBGenericRequest req;

	TRACE_INFO_WP("Enum16_lpm");
	USBH_dev_enum->lpm_desc = malloc(sizeof(USB_DeviceLPMDescriptor));

	if (USBH_dev_enum->lpm_desc == NULL) {
		assert(false);
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}

	// Send USB device descriptor request
	req.bmRequestType = USB_REQ_RECIP_DEVICE | USB_REQ_TYPE_STANDARD |
						USB_REQ_DIR_IN;
	req.bRequest = USB_REQ_GET_DESCRIPTOR;
	req.wValue = USB_DT_BOS << 8;
	req.wIndex = 0;
	req.wLength = sizeof(USB_DeviceLPMDescriptor);

	if (!USBH_HAL_SetupReq(UHC_DEVICE_ENUM_ADD,
						   &req,
						   (uint8_t *) USBH_dev_enum->lpm_desc,
						   sizeof(USB_DeviceLPMDescriptor),
						   NULL, USBH_enumeration_step17_lpm)) {
		USBH_enumeration_error(UHC_ENUM_MEMORY_LIMIT);
		return;
	}
}

/**
 * \brief Device enumeration step 17 (LPM only)
 * Check LPM support through the BOS descriptor received
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
static void USBH_enumeration_step17_lpm(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans)
{
	UNUSED(add);

	TRACE_INFO_WP("Enum17_lpm ");

	if (status == UHD_TRANS_STALL) {
		free(USBH_dev_enum->lpm_desc);
		USBH_dev_enum->lpm_desc = NULL;
	} else if ((status != UHD_TRANS_NOERROR)
			   || (payload_trans < sizeof(USB_DeviceLPMDescriptor))
			   || (USBH_dev_enum->lpm_desc->bos.bDescriptorType != USB_DT_BOS)
			   || (payload_trans != (USBH_dev_enum->lpm_desc->bos.wTotalLength))) {
		USBH_enumeration_error((status == UHD_TRANS_DISCONNECT) ?
							   UHC_ENUM_DISCONNECT : UHC_ENUM_FAIL);
		return;
	} else if (status == UHD_TRANS_NOERROR) {
		// Check LPM support
		if ((USBH_dev_enum->lpm_desc->capa_ext.bDescriptorType !=
			 USBGenericDescriptor_DEVICE_CAPABILITY)
			|| (USBH_dev_enum->lpm_desc->capa_ext.bDevCapabilityType !=
				USB_DC_USB20_EXTENSION)
			|| (!(USBH_dev_enum->lpm_desc->capa_ext.bmAttributes & (USB_DC_EXT_LPM)))) {
			free(USBH_dev_enum->lpm_desc);
			USBH_dev_enum->lpm_desc = NULL;
		}
	}

	USBH_enum_try = 0;
	UHC_ENUM_EVENT(USBH_dev_enum, UHC_ENUM_SUCCESS);
}
#endif // USB_HOST_LPM_SUPPORT

/**
 * \brief Manage error during device enumeration
 *
 * \param status        Enumeration error occurred
 */
static void USBH_enumeration_error(USBH_enum_status_t status)
{
	if (status == UHC_ENUM_DISCONNECT) {
		USBH_enum_try = 0;
		TRACE_INFO_WP("Enum_Abort ");
		return; // Abort enumeration process
	}

	USBH_HAL_FreePipe(USBH_dev_enum->address, 0xFF);

	// Free USB configuration descriptor buffer
	if (USBH_dev_enum->conf_desc != NULL) {
		free(USBH_dev_enum->conf_desc);
		USBH_dev_enum->conf_desc = NULL;
	}

	USBH_dev_enum->address = 0;

	if (USBH_enum_try++ < UHC_ENUM_NB_TRY) {
		// Restart enumeration at beginning
		USBH_enumeration_step1();
		return;
	}

	// Abort enumeration, set line in suspend mode
	USBH_enumeration_suspend();
	UHC_ENUM_EVENT(USBH_dev_enum, status);
	USBH_enum_try = 0;
}

/**
 * \brief Enables or disables the remote wakeup feature
 * of all devices connected
 *
 * \param b_enable   true to enable remote wakeup feature, else disable.
 */
static void USBH_remotewakeup(bool b_enable)
{
	USBGenericRequest req;
	USBH_device_t *dev;

	dev = &USBH_device_root;

	while (1) {
		if (dev->conf_desc->bmAttributes & USBConfigurationDescriptor_REMOTE_WAKEUP) {
			if (b_enable)
				req.bRequest = USB_REQ_SET_FEATURE;
			else
				req.bRequest = USB_REQ_CLEAR_FEATURE;

			req.bmRequestType = USB_REQ_RECIP_DEVICE
								| USB_REQ_TYPE_STANDARD | USB_REQ_DIR_OUT;
			req.wValue = USB_DEV_FEATURE_REMOTE_WAKEUP;
			req.wIndex = 0;
			req.wLength = 0;
			USBH_HAL_SetupReq(dev->address, &req, NULL, 0, NULL, NULL);
		}

#ifdef USB_HOST_HUB_SUPPORT

		if (dev->next == NULL)
			break;

		dev = dev->next;
#else
		break;
#endif
	}
}

/**
 * \brief Callback used to signal the end of a setup request
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
static void USBH_setup_request_callback(
	USBHS_Add_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans)
{
	UNUSED(add);
	UNUSED(payload_trans);
	USBH_setup_request_finish_status = (status == UHD_TRANS_NOERROR);
	USBH_setup_request_finish = true;
}


/**
 * \name Callbacks used by USB Host Driver (UHD) to notify events
 * @{
 */
void USBH_notify_connection(bool b_plug)
{
	if (b_plug) {
		assert(USBH_device_root.address == UHC_USB_ADD_NOT_VALID);

#ifdef USB_HOST_HUB_SUPPORT
		USBH_power_running = 0;
#endif
	} else {
		if (USBH_device_root.address == UHC_USB_ADD_NOT_VALID) {
			// Already disconnected
			// Ignore the noise from host stop process
			return;
		}
	}

	// Device connection on root
	USBH_connection_tree(b_plug, &USBH_device_root);
}

void USBH_notify_sof(bool b_micro)
{
	// Call all UHIs
	uint8_t i;

	for (i = 0; i < UHC_NB_UHI; i++) {
		if (USBH_uhis[i].sof_notify != NULL)
			USBH_uhis[i].sof_notify(b_micro);
	}

	if (!b_micro) {
		// Manage SOF timeout
		if (USBH_sof_timeout) {
			if (--USBH_sof_timeout == 0)
				USBH_sof_timeout_callback();
		}
	}
}

void USBH_notify_resume(void)
{
	USBH_remotewakeup(false);
	UHC_WAKEUP_EVENT();
}

#ifdef USB_HOST_LPM_SUPPORT
void USBH_notify_resume_lpm(void)
{
	UHC_WAKEUP_EVENT();
}
#endif

//! @}


/**
 * \name Functions to control the USB host stack
 *
 * @{
 */
void USBH_start(void)
{
	USBH_device_root.address = UHC_USB_ADD_NOT_VALID;
	USBH_sof_timeout = 0; // No callback registered on a SOF timeout
	USBH_HAL_EnableUsbHost();
}

void USBH_stop(bool b_id_stop)
{
	// Stop UHD
	USBH_HAL_DisableUsb(b_id_stop);
}

void USBH_suspend(bool b_remotewakeup)
{
	if (USBH_enum_try) {
		// enumeration on-going, the USB suspend can't be done
		return;
	}

	if (b_remotewakeup)
		USBH_remotewakeup(true);

	// Suspend all USB devices
	USBH_HAL_Suspend();
}

bool USBH_is_suspend(void)
{
	if (USBH_device_root.address == UHC_USB_ADD_NOT_VALID)
		return true;

	return USBH_HAL_IsSuspended();
}

void USBH_resume(void)
{
	if (!USBH_is_suspend())
		return;

	// Resume all USB devices
	USBH_HAL_Resume();
}

#ifdef USB_HOST_LPM_SUPPORT
bool USBH_suspend_lpm(bool b_remotewakeup, uint8_t besl)
{
	if (USBH_enum_try) {
		// enumeration on-going, the USB suspend can't be done
		return false;
	}

	// Check LPM support
	if (USBH_dev_enum->lpm_desc == NULL) {
		// Device cannot support LPM
		return false;
	}

	// Suspend all USB devices
	return uhd_suspend_lpm(b_remotewakeup, besl);
}
#endif // USB_HOST_LPM_SUPPORT

//! @}


/**
 * \name User functions to manage the devices
 *
 * @{
 */
uint8_t USBH_get_device_number(void)
{
#ifdef USB_HOST_HUB_SUPPORT
	uint8_t nb_dev = 0;
	USBH_device_t *dev;

	if (USBH_device_root.address != UHC_USB_ADD_NOT_VALID) {
		dev = &USBH_device_root;

		while (nb_dev++) {
			if (dev->next == NULL)
				break;

			dev = dev->next;
		}
	}

	return nb_dev;
#else
	return (USBH_device_root.address != UHC_USB_ADD_NOT_VALID) ? 1 : 0;
#endif
}

char *USBH_dev_get_string_manufacturer(USBH_device_t *dev)
{
	if (!dev->dev_desc.iManufacturer) {
		return NULL; // No manufacturer string available
	}

	return USBH_dev_get_string(dev, dev->dev_desc.iManufacturer);
}

char *USBH_dev_get_string_product(USBH_device_t *dev)
{
	if (!dev->dev_desc.iProduct) {
		return NULL; // No product string available
	}

	return USBH_dev_get_string(dev, dev->dev_desc.iProduct);
}

char *USBH_dev_get_string_serial(USBH_device_t *dev)
{
	if (!dev->dev_desc.iSerialNumber) {
		return NULL; // No serial string available
	}

	return USBH_dev_get_string(dev, dev->dev_desc.iSerialNumber);
}

char *USBH_dev_get_string(USBH_device_t *dev, uint8_t str_id)
{
	USBGenericRequest req;
	USBStringDescriptor str_header;
	USBStringLangIdDescriptor *str_desc;
	char *string;
	uint8_t i;
	UNUSED(dev);

	req.bmRequestType = USB_REQ_RECIP_DEVICE | USB_REQ_TYPE_STANDARD |
						USB_REQ_DIR_IN;
	req.bRequest = USB_REQ_GET_DESCRIPTOR;
	req.wValue = (USBGenericDescriptor_STRING << 8) | str_id;
	req.wIndex = 0;
	req.wLength = sizeof(USBStringDescriptor);

	// Get the size of string
	USBH_setup_request_finish = false;

	if (!USBH_HAL_SetupReq(0,
						   &req,
						   (uint8_t *)&str_header,
						   sizeof(USBStringDescriptor),
						   NULL,
						   USBH_setup_request_callback))
		return NULL;

	while (!USBH_setup_request_finish);

	if (!USBH_setup_request_finish_status)
		return NULL;

	// Get the size of string
	str_desc = malloc(str_header.bLength);

	if (str_desc == NULL)
		return NULL;

	req.wLength = str_header.bLength;
	USBH_setup_request_finish = false;

	if (!USBH_HAL_SetupReq(0,
						   &req,
						   (uint8_t *)str_desc,
						   str_header.bLength,
						   NULL,
						   USBH_setup_request_callback))
		return NULL;

	while (!USBH_setup_request_finish);

	if (!USBH_setup_request_finish_status) {
		free(str_desc);
		return NULL;
	}

	// The USB strings are "always" in ASCII format, then translate it.
	str_header.bLength = (str_header.bLength - 2) / 2; // Number of character
	string = malloc(str_header.bLength + 1); // +1 for NULL terminal

	if (string == NULL) {
		free(str_desc);
		return NULL;
	}

	for (i = 0; i < str_header.bLength; i++)
		string[i] = (str_desc->string[i]) & 0xFF;

	string[i] = 0;
	free(str_desc);

	return string;
}

uint16_t USBH_dev_get_power(USBH_device_t *dev)
{
	return dev->conf_desc->bMaxPower * 2;
}

USBH_Speed_t USBH_dev_get_speed(USBH_device_t *dev)
{
	return dev->speed;
}

bool USBH_dev_is_high_speed_support(USBH_device_t *dev)
{
	USBGenericRequest req;
	USBDeviceQualifierDescriptor qualifier;

	if (dev->speed == UHD_SPEED_HIGH)
		return true;

	if (dev->speed == UHD_SPEED_FULL) {
		req.bmRequestType = USB_REQ_RECIP_DEVICE
							| USB_REQ_TYPE_STANDARD | USB_REQ_DIR_IN;
		req.bRequest = USB_REQ_GET_DESCRIPTOR;
		req.wValue = (USBGenericDescriptor_DEVICEQUALIFIER << 8);
		req.wIndex = 0;
		req.wLength = sizeof(qualifier);

		// Get the size of string
		USBH_setup_request_finish = false;

		if (!USBH_HAL_SetupReq(0,
							   &req,
							   (uint8_t *)&qualifier,
							   sizeof(qualifier),
							   NULL,
							   USBH_setup_request_callback))
			return NULL;

		while (!USBH_setup_request_finish);

		return USBH_setup_request_finish_status;
	}

	return false; // Low speed device
}

/*
 * SCB_CleanDCache_by_Addr
 * Make the address to be aligned to 32-byte boundary
 */
void USBHS_SCB_CleanDCache_by_Addr(uint32_t *addr, int32_t dsize)
{
	uint32_t *pTemp;

	if ((uint32_t)addr % 32 == 0)
		pTemp = addr;
	else
		pTemp = (uint32_t *)(((uint32_t)addr) & (uint32_t)(~(32 - 1)));

	SCB_CleanDCache_by_Addr(pTemp, dsize);
}
/*
 * SCB_InvalidateDCache_by_Addr
 * Make the address to be aligned to 32-byte boundary
 */
void USBHS_SCB_InvalidateDCache_by_Addr(uint32_t *addr, int32_t dsize)
{
	uint32_t *pTemp;

	if ((uint32_t)addr % 32 == 0)
		pTemp = addr;
	else
		pTemp = (uint32_t *)(((uint32_t)addr) & (uint32_t)(~(32 - 1)));

	SCB_InvalidateDCache_by_Addr(pTemp, dsize);
}

//! @}

//! @}
