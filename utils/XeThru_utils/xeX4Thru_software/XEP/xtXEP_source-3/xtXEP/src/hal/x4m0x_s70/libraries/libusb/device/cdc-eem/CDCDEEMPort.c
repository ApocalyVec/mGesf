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
 *  Implementation of the CDCDEEMPort class methods.
 */

/** \addtogroup usbd_cdc
 *@{
 */

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

#include <CDCDEEMPort.h>
#include <CDCDescriptors.h>
#include <USBLib_Trace.h>
#include <string.h>
#include "../../../../utils/utility.h"

/*------------------------------------------------------------------------------
 *         Types
 *------------------------------------------------------------------------------*/

/** Parse data extention for descriptor parsing  */
typedef struct _CDCDParseData {
	/** Pointer to CDCDEEMPort instance */
	CDCDEEMPort *pCdcd;
	/** Pointer to found interface descriptor */
	USBInterfaceDescriptor *pIfDesc;

} CDCDParseData;

/*------------------------------------------------------------------------------
 *         Internal variables
 *------------------------------------------------------------------------------*/

/** Maximum packet size in bytes */
#define DATAPACKETSIZE (1536)

/** Size in bytes of the buffer used for reading data from USB */
#define DATABUFFERSIZE (DATAPACKETSIZE+2)

/** Number of transmit buffers */
#define USB_TX_BUFFERS 64

/** Buffer for storing incoming USB data. */
static uint8_t gRxBuffer[DATABUFFERSIZE];

/** Buffer for storing outgoing USB data. */
static uint8_t gUsbTxBuffers[USB_TX_BUFFERS * DATAPACKETSIZE];
static uint32_t gUsbTxBufferSizes[USB_TX_BUFFERS];
static int gUsbTxHeadIdx = 0;
static int gUsbTxTailIdx = 0;
static uint8_t gUsbTxMutex = 0;

// TODO temp
static TransferCallback gRxCallback;
static void *gRxCallbackArg;
static void *gRxData;
static uint32_t gRxDataSize;

/*------------------------------------------------------------------------------
 *         Internal functions
 *------------------------------------------------------------------------------*/

/**
 * Parse descriptors: Interface, Bulk IN/OUT, Interrupt IN.
 * \param desc Pointer to descriptor list.
 * \param arg  Argument, pointer to AUDDParseData instance.
 */
static uint32_t _Interfaces_Parse(USBGenericDescriptor *pDesc,
								  CDCDParseData *pArg)
{
	CDCDEEMPort *pCdcd = pArg->pCdcd;

	/* Not a valid descriptor */
	if (pDesc->bLength == 0)
		return USBRC_PARAM_ERR;

	/* Find interface descriptor */
	if (pDesc->bDescriptorType == USBGenericDescriptor_INTERFACE) {
		USBInterfaceDescriptor *pIf = (USBInterfaceDescriptor *)pDesc;

		/* Obtain interface from descriptor */
		if (pCdcd->bInterfaceNdx == 0xFF) {
			/* First interface is communication */
			if (pIf->bInterfaceClass ==
				CDCCommunicationInterfaceDescriptor_CLASS) {
				pCdcd->bInterfaceNdx = pIf->bInterfaceNumber;
				pCdcd->bNumInterface = 1;
			}

			pArg->pIfDesc = pIf;
		} else if (pCdcd->bInterfaceNdx <= pIf->bInterfaceNumber
				&& pCdcd->bInterfaceNdx + pCdcd->bNumInterface
					> pIf->bInterfaceNumber)
			pArg->pIfDesc = pIf;
	}

	/* Parse valid interfaces */
	if (pArg->pIfDesc == 0)
		return 0;

	/* Find endpoint descriptors */
	if (pDesc->bDescriptorType == USBGenericDescriptor_ENDPOINT) {
		USBEndpointDescriptor *pEp = (USBEndpointDescriptor *)pDesc;

		switch (pEp->bmAttributes & 0x3) {
		case USBEndpointDescriptor_BULK:
			if (pEp->bEndpointAddress & 0x80)
				pCdcd->bBulkInPIPE = pEp->bEndpointAddress & 0x7F;
			else {
				pCdcd->bBulkOutPIPE = pEp->bEndpointAddress;
				pCdcd->wBulkOutMaxPacketSize = pEp->wMaxPacketSize;
			}
		}
	}

	if (pCdcd->bInterfaceNdx != 0xFF
			&&  pCdcd->bBulkInPIPE != 0
			&&  pCdcd->bBulkOutPIPE != 0)
		return USBRC_FINISHED;

	return 0;
}

/*----------------------------------------------------------------------------
 * Callback invoked when data has been received on the USB.
 *----------------------------------------------------------------------------*/
static void _TransferCallback(const CDCDEEMPort *pCdcd,
							  uint8_t status,
							  uint32_t received,
							  uint32_t remaining)
{
	/* Check that data has been received successfully */
	if (status == USBD_STATUS_SUCCESS) {
		uint32_t offset = 0;

		/* Check if bytes have been discarded */
		if ((received == DATAPACKETSIZE) && (remaining > 0)) {
			TRACE_WARNING("_UsbDataReceived: %u bytes discarded\n\r",
						  (unsigned int)remaining);
		}

		while (received >= 2) {
			uint16_t header;

			header = gRxBuffer[offset] + (gRxBuffer[offset + 1] << 8);
			offset += 2;
			received -= 2;

			if (header & (1 << 15)) {
				// bit 14: reserved
				if (header & (1 << 14))
					break;

				int cmd = (header >> 11) & 7; // bits 11..13: command
				unsigned int param = header & 0x7ff;   // bits 0..10: parameter

				switch (cmd) {
				case 0:
					// Echo
					TRACE_WARNING("Received Echo EEM command\n\r");

					if (received < param)
						break;

					header = (1 << 15) + (1 << 11);

					TRACE_WARNING("Sending EchoReply EEM command\n\r");
					gRxBuffer[offset - 2] = header & 0xff;
					gRxBuffer[offset - 1] = (header >> 8) & 0xff;

					while (CDCDEEMPort_Write(pCdcd, &gRxBuffer[offset - 2],
											 param + 2, NULL, 0) != USBD_STATUS_SUCCESS);

					break;

				case 1:
					// EchoReply
					TRACE_WARNING("Unexpected EchoReply EEM command received\n\r");
					break;

				case 2:
					// SuspendHint
					TRACE_WARNING("Unexpected SuspendHint EEM command received\n\r");
					break;

				case 3:
					// ResponseHint
					TRACE_WARNING("Unexpected ResponseHint EEM command received\n\r");
					break;

				case 4:
					// ResponseCompleteHint
					TRACE_WARNING("Unexpected ResponseCompleteHint EEM command received\n\r");
					break;

				case 5:
					// Tickle
					TRACE_WARNING("Unexpected Tickle EEM command received\n\r");
					break;

				default:
					// unknown command
					TRACE_WARNING("Unexpected unknown EEM command %d received\n\r", cmd);
					break;
				}
			} else {
				uint16_t len = header & 0x3fff;

				if (received < len || len < 4)
					return;

				// TODO check CRC if present

				/* Send frame to Callback */
				if (gRxCallback) {
					if (gRxDataSize + 4 > len) {
						memcpy(gRxData, gRxBuffer + offset, len - 4);
						gRxCallback(gRxCallbackArg, USBD_STATUS_SUCCESS, len - 4, 0);
					}
				}

				offset += len;
				received -= len;
			}
		}

		// TODO: handle remaining data if any
		if (received > 0) {
			TRACE_WARNING("_UsbDataReceived: %u bytes ignored\n\r",
						  (unsigned int)remaining);
		}
	} else {
		TRACE_WARNING("_UsbDataReceived: Transfer error\n\r");
	}
}

static int _UsbTxBufferPush(void)
{
	int idx;

	if (gUsbTxHeadIdx == ((gUsbTxTailIdx - 1 + USB_TX_BUFFERS) % USB_TX_BUFFERS))
		return -1; // no buffer available

	idx = gUsbTxHeadIdx;
	gUsbTxHeadIdx = (gUsbTxHeadIdx + 1) % USB_TX_BUFFERS;
	gUsbTxBufferSizes[idx] = 0;
	return idx;
}

static int _UsbTxBufferPop(void)
{
	int idx;

	if (gUsbTxHeadIdx == gUsbTxTailIdx)
		return -1; // nothing to get

	if (gUsbTxBufferSizes[gUsbTxTailIdx] == 0)
		return -1; // still nothing to get

	idx = gUsbTxTailIdx;
	gUsbTxTailIdx = (gUsbTxTailIdx + 1) % USB_TX_BUFFERS;
	return idx;
}

static int _UsbTxBufferPeek(void)
{
	if (gUsbTxHeadIdx == gUsbTxTailIdx)
		return -1; // nothing to get

	if (gUsbTxBufferSizes[gUsbTxTailIdx] == 0)
		return -1; // still nothing to get

	return gUsbTxTailIdx;
}

static void _FlushUsbTxBuffers(const uint8_t *pipe);

static void _UsbDataSent(void *pipe,
						 uint8_t status,
						 uint32_t received,
						 uint32_t remaining)
{
	int bufIdx;

	// unused args
	(void)status;
	(void)received;
	(void)remaining;

	bufIdx = _UsbTxBufferPop();

	if (bufIdx >= 0) {
		TRACE_INFO("%u USB_TX_DEQUEUE(%d)\n\r", (unsigned int)GetTicks(), bufIdx);
	}

	// release mutex, to allow for another Flush
	__disable_irq();
	ReleaseMutex(gUsbTxMutex);
	__enable_irq();

	// try to flush more buffers
	_FlushUsbTxBuffers((const uint8_t *)pipe);
}

static void _FlushUsbTxBuffers(const uint8_t *pipe)
{
	int bufIdx;
	uint32_t timeout = 0x7ff;

	// try to lock mutex to avoid concurrent flushes
	__disable_irq();

	if (LockMutex(gUsbTxMutex, timeout) != 0) {
		__enable_irq();
		return;
	}

	__enable_irq();

	if ((bufIdx = _UsbTxBufferPeek()) >= 0) {
		TRACE_INFO("%u USB_TX_SEND(%d,%u)\n\r", (unsigned int)GetTicks(), bufIdx,
				   (unsigned int)gUsbTxBufferSizes[bufIdx]);

		/* Send buffer through the USB */
		while (USBD_Write(*pipe, &gUsbTxBuffers[DATAPACKETSIZE * bufIdx],
						  gUsbTxBufferSizes[bufIdx], _UsbDataSent,
						  (void *)pipe) != USBD_STATUS_SUCCESS) {}
	} else {
		// nothing to Flush, release mutex
		__disable_irq();
		ReleaseMutex(gUsbTxMutex);
		__enable_irq();
	}
}

/*------------------------------------------------------------------------------
 *         Exported functions
 *------------------------------------------------------------------------------*/

/**
 * Initializes the USB Device CDC serial port function.
 * \param pCdcd Pointer to CDCDEEMPort instance.
 * \param pUsbd Pointer to USBDDriver instance.
 * \param fEventHandler Pointer to event handler function.
 * \param firstInterface First interface index for the function
 *                       (0xFF to parse from descriptors).
 * \param numInterface   Number of interfaces for the function.
 */
void CDCDEEMPort_Initialize(CDCDEEMPort *pCdcd,
							USBDDriver *pUsbd,
							uint8_t firstInterface, uint8_t numInterface)
{
	TRACE_INFO("CDCDEEMPort_Initialize\n\r");

	/* Initialize USB Device Driver interface */
	pCdcd->pUsbd = pUsbd;
	pCdcd->bInterfaceNdx = firstInterface;
	pCdcd->bNumInterface = numInterface;
	pCdcd->bBulkInPIPE  = 0;
	pCdcd->bBulkOutPIPE = 0;
}

/**
 * Parse CDC EEM Port information for CDCDEEMPort instance.
 * Accepted interfaces:
 * - Communication Interface + Data Interface
 * - Data Interface ONLY
 * \param pCdcd        Pointer to CDCDEEMPort instance.
 * \param pDescriptors Pointer to descriptor list.
 * \param dwLength     Descriptor list size in bytes.
 */
USBGenericDescriptor *CDCDEEMPort_ParseInterfaces(
	CDCDEEMPort *pCdcd,
	USBGenericDescriptor *pDescriptors,
	uint32_t dwLength)
{
	CDCDParseData parseData;

	parseData.pCdcd   = pCdcd;
	parseData.pIfDesc = 0;

	return USBGenericDescriptor_Parse(
			   pDescriptors, dwLength,
			   (USBDescriptorParseFunction)_Interfaces_Parse,
			   &parseData);
}


/**
 * Handles CDC-specific SETUP requests. Should be called from a
 * re-implementation of USBDCallbacks_RequestReceived() method.
 * \param pCdcd Pointer to CDCDEEMPort instance.
 * \param request Pointer to a USBGenericRequest instance.
 * \return USBRC_SUCCESS if request handled, otherwise error.
 */
uint32_t CDCDEEMPort_RequestHandler(
	CDCDEEMPort *pCdcd,
	const USBGenericRequest *request)
{
	if (USBGenericRequest_GetType(request) != USBGenericRequest_CLASS)
		return USBRC_PARAM_ERR;

	TRACE_INFO_WP("Cdce ");

	/* Validate interface */
	if (request->wIndex >= pCdcd->bInterfaceNdx &&
		request->wIndex < pCdcd->bInterfaceNdx + pCdcd->bNumInterface) {
	} else
		return USBRC_PARAM_ERR;

	/* Handle the request */
	switch (USBGenericRequest_GetRequest(request)) {

	default:

		return USBRC_PARAM_ERR;
	}

	return USBRC_SUCCESS;
}

/**
 * Receives data from the host through the virtual COM port created by
 * the CDC device serial driver. This function behaves like USBD_Read.
 * \param pCdcd  Pointer to CDCDEEMPort instance.
 * \param pData  Pointer to the data buffer to put received data.
 * \param dwSize Size of the data buffer in bytes.
 * \param fCallback Optional callback function to invoke when the transfer
 *                  finishes.
 * \param pArg      Optional argument to the callback function.
 * \return USBD_STATUS_SUCCESS if the read operation has been started normally;
 *         otherwise, the corresponding error code.
 */
uint32_t CDCDEEMPort_Read(const CDCDEEMPort *pCdcd,
						  void *pData, uint32_t dwSize,
						  TransferCallback fCallback, void *pArg)
{
	if (pCdcd->bBulkOutPIPE == 0)
		return USBRC_PARAM_ERR;

	gRxCallback = fCallback;
	gRxCallbackArg = pArg;
	gRxData = pData;
	gRxDataSize = dwSize;
	return USBD_Read(pCdcd->bBulkOutPIPE,
					 gRxBuffer, DATABUFFERSIZE,
					 (TransferCallback)_TransferCallback, pArg);
}

/**
 * Sends a data buffer through the virtual COM port created by the CDC
 * device serial driver. This function behaves exactly like USBD_Write.
 *
 * TODO batch packets ?
 *
 * \param pCdcd  Pointer to CDCDEEMPort instance.
 * \param pData  Pointer to the data buffer to send.
 * \param dwSize Size of the data buffer in bytes.
 * \param fCallback Optional callback function to invoke when the transfer
 *                  finishes.
 * \param pArg      Optional argument to the callback function.
 * \return USBD_STATUS_SUCCESS if the read operation has been started normally;
 *         otherwise, the corresponding error code.
 */
uint32_t CDCDEEMPort_Write(const CDCDEEMPort *pCdcd,
						   void *pData, uint32_t dwSize,
						   TransferCallback fCallback, void *pArg)
{
	int bufIdx;
	uint8_t *buffer;
	uint32_t len;

	// unused args
	(void)fCallback;
	(void)pArg;

	if (pCdcd->bBulkInPIPE == 0)
		return USBRC_PARAM_ERR;

	bufIdx = _UsbTxBufferPush();

	if (bufIdx >= 0) {
		buffer = &gUsbTxBuffers[DATAPACKETSIZE * bufIdx];

		// EEM header
		len = dwSize;
		uint16_t eemHdr = (len + 4) & 0x3fff;
		buffer[0] = eemHdr & 0xff;
		buffer[1] = (eemHdr >> 8) & 0xff;
		memcpy(buffer + 2, pData, len);
		len += 2; // add EEM header length (2 bytes)
		buffer[len] = 0xde;
		buffer[len + 1] = 0xad;
		buffer[len + 2] = 0xbe;
		buffer[len + 3] = 0xef;
		len += 4; // add CRC length (4 bytes)

		// add zero EEM packet when len % maxTransferSize == 0
		if (len % pCdcd->wBulkOutMaxPacketSize == 0) {
			memset(buffer + len, 0, 2);
			len += 2;
		}

		gUsbTxBufferSizes[bufIdx] = len;

		_FlushUsbTxBuffers(&pCdcd->bBulkInPIPE);
	} else {
		// TODO
	}

	return USBRC_SUCCESS;
}

/**@}*/
