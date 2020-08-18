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

/** \file
 *  Definition of a class for implementing a USB device
 *  CDC EEM function.
 */

#ifndef _CDCDEEMPORT_H_
#define _CDCDEEMPORT_H_

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/
 
/* These headers were introduced in C99
   by working group ISO/IEC JTC1/SC22/WG14. */
#include <stdint.h>

#include <USBRequests.h>
#include <CDCRequests.h>
#include <CDCNotifications.h>
#include "USBD.h"
#include <USBDDriver.h>

/** \addtogroup usbd_cdc
 *@{
 */

/*------------------------------------------------------------------------------
 *         Defines
 *------------------------------------------------------------------------------*/

/** \addtogroup usbd_cdc_eem_desc USB Device EEM Port Descriptor Values
 *      @{
 */
/** Default CDC interrupt endpoints max packat size (8). */
#define CDCDEEMPort_INTERRUPT_MAXPACKETSIZE          8
/** Default CDC interrupt endpoint polling rate of High Speed (16ms). */
#define CDCDEEMPort_INTERRUPT_INTERVAL_HS            8
/** Default CDC interrupt endpoint polling rate of Full Speed (16ms). */
#define CDCDEEMPort_INTERRUPT_INTERVAL_FS            16
/** Default CDC bulk endpoints max packat size (512, for HS actually). */
#define CDCDEEMPort_BULK_MAXPACKETSIZE_HS            512
/** Default CDC bulk endpoints max packat size (64, for FS actually). */
#define CDCDEEMPort_BULK_MAXPACKETSIZE_FS            64
/**     @}*/

/*------------------------------------------------------------------------------
 *         Types
 *------------------------------------------------------------------------------*/

/**
 * Struct for USB CDC EEM port function.
 */
typedef struct _CDCDEEMPort {
    /** USB Driver for the %device */
    USBDDriver *pUsbd;
    /** USB starting interface index */
    uint8_t bInterfaceNdx;
    /** USB number of interfaces */
    uint8_t bNumInterface;
    /** USB bulk IN endpoint address */
    uint8_t bBulkInPIPE;
    /** USB bulk OUT endpoint address */
    uint8_t bBulkOutPIPE;
    /** Max packet size for OUT endpoint */
    uint32_t wBulkOutMaxPacketSize;
} CDCDEEMPort;

/*------------------------------------------------------------------------------
 *         Functions
 *------------------------------------------------------------------------------*/

extern void CDCDEEMPort_Initialize(CDCDEEMPort *pCdcd,
                                      USBDDriver *pUsbd,
                                      uint8_t firstInterface,
                                      uint8_t numInterface);

extern USBGenericDescriptor * CDCDEEMPort_ParseInterfaces(
    CDCDEEMPort * pCdcd,
    USBGenericDescriptor * pDescriptors, uint32_t dwLength);

extern uint32_t CDCDEEMPort_RequestHandler(
    CDCDEEMPort *pCdcd,
    const USBGenericRequest *pRequest);

extern uint32_t CDCDEEMPort_Write(
    const CDCDEEMPort *pCdcd,
    void *pData, uint32_t dwSize,
    TransferCallback fCallback, void* pArg);

extern uint32_t CDCDEEMPort_Read(
    const CDCDEEMPort *pCdcd,
    void *pData, uint32_t dwSize,
    TransferCallback fCallback, void* pArg);

/**@}*/
#endif /* #ifndef _CDCDEEMPORT_H_ */
