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
 * \brief Common API for USB Host Interface
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _UHI_H_
#define _UHI_H_

#include "USBH.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup usb_host_group
 * \defgroup uhi_group USB Host Interface (UHI)
 * The UHI provides a common API for all classes,
 * and this is used by UHC for the main control of USB host interface.
 * @{
 */

/**
 * \brief UHI API.
 *
 * The callbacks within this structure are called only by
 * USB Host Controller (UHC)
 */
typedef struct {
	/**
	 * \brief Install interface
	 * Allocate interface endpoints if supported.
	 *
	 * \param USBH_device_t    Device to request
	 *
	 * \return status of the install
	 */
	USBH_enum_status_t (*install)(USBH_device_t *);

	/**
	 * \brief Enable the interface.
	 *
	 * Enable a USB interface corresponding to UHI.
	 *
	 * \param USBH_device_t    Device to request
	 */
	void (*enable)(USBH_device_t *);

	/**
	 * \brief Uninstall the interface (if installed)
	 *
	 * \param USBH_device_t    Device to request
	 */
	void (*uninstall)(USBH_device_t *);

	/**
	 * \brief Signal that a SOF has occurred
	 */
	void (*sof_notify)(bool b_micro);
} uhi_api_t;

//@}

#ifdef __cplusplus
}
#endif
#endif // _UHI_H_
