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

/**\file
 * Implementation of a single CDC EEM port function for USB device.
 */

/** \addtogroup usbd_cdc
 *@{
 */

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

#include "CDCDEEM.h"

#include <USBLib_Trace.h>
#include <USBDDriver.h>
#include <USBD_HAL.h>

/*------------------------------------------------------------------------------
 *         Types
 *------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 *         Internal variables
 *------------------------------------------------------------------------------*/

/** EEM Port instance list */
static CDCDEEMPort cdcdEEM;

/*------------------------------------------------------------------------------
 *         Internal functions
 *------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 *         Exported functions
 *------------------------------------------------------------------------------*/

/**
 *  Initializes the USB Device CDC serial driver & USBD Driver.
 * \param pUsbd         Pointer to USBDDriver instance.
 * \param bInterfaceNb  Interface number for the function.
 */
void CDCDEEM_Initialize(
	USBDDriver *pUsbd, uint8_t bInterfaceNb)
{
	CDCDEEMPort *pCdcd = &cdcdEEM;

	TRACE_INFO("CDCDEEM_Initialize\n\r");

	/* Initialize serial port function */
	CDCDEEMPort_Initialize(
		pCdcd, pUsbd,
		bInterfaceNb, 2);
}

/**
 * Invoked whenever the device is changed by the
 * host.
 * \pDescriptors Pointer to the descriptors for function configure.
 * \wLength      Length of descriptors in number of bytes.
 */
void CDCDEEM_ConfigureFunction(USBGenericDescriptor *pDescriptors,
							   uint16_t wLength)
{
	CDCDEEMPort *pCdcd = &cdcdEEM;
	CDCDEEMPort_ParseInterfaces(pCdcd,
								(USBGenericDescriptor *)pDescriptors,
								wLength);
}

/**
 * Handles CDC-specific SETUP requests. Should be called from a
 * re-implementation of USBDCallbacks_RequestReceived() method.
 * \param request Pointer to a USBGenericRequest instance.
 */
uint32_t CDCDEEM_RequestHandler(const USBGenericRequest *request)
{
	CDCDEEMPort *pCdcd = &cdcdEEM;

	TRACE_INFO_WP("Cdcf ");
	return CDCDEEMPort_RequestHandler(pCdcd, request);
}

/**
 * Receives data from the host through the virtual COM port created by
 * the CDC device serial driver. This function behaves like USBD_Read.
 * \param data Pointer to the data buffer to put received data.
 * \param size Size of the data buffer in bytes.
 * \param callback Optional callback function to invoke when the transfer
 *                 finishes.
 * \param argument Optional argument to the callback function.
 * \return USBD_STATUS_SUCCESS if the read operation has been started normally;
 *         otherwise, the corresponding error code.
 */
uint32_t CDCDEEM_Read(void *data,
					  uint32_t size,
					  TransferCallback callback,
					  void *argument)
{
	CDCDEEMPort *pCdcd = &cdcdEEM;
	return CDCDEEMPort_Read(pCdcd, data, size, callback, argument);
}

/**
 * Sends a data buffer through the virtual COM port created by the CDC
 * device serial driver. This function behaves exactly like USBD_Write.
 * \param data Pointer to the data buffer to send.
 * \param size Size of the data buffer in bytes.
 * \param callback Optional callback function to invoke when the transfer
 *                 finishes.
 * \param argument Optional argument to the callback function.
 * \return USBD_STATUS_SUCCESS if the read operation has been started normally;
 *         otherwise, the corresponding error code.
 */
uint32_t CDCDEEM_Write(void *data,
					   uint32_t size,
					   TransferCallback callback,
					   void *argument)
{
	CDCDEEMPort *pCdcd = &cdcdEEM;
	return CDCDEEMPort_Write(pCdcd, data, size, callback, argument);
}

/**@}*/
