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
 * \brief Common API for USB Host Drivers (UHD)
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _UHD_H_
#define _UHD_H_

#include "USBDescriptors.h"
#include "USBRequests.h"
#include "board.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup usb_host_group
 * \defgroup uhd_group USB Host Driver (UHD)
 *
 * The UHD driver provides a low-level abstraction of the host
 * controller hardware. Most events coming from the hardware such as
 * interrupts may cause the UHD performing function call in UHC and UHI.
 *
 * @{
 */

//! \brief Device speed
typedef enum {
	UHD_SPEED_LOW  = 0,
	UHD_SPEED_FULL = 1,
	UHD_SPEED_HIGH = 2,
} USBH_Speed_t;

/**
 * \brief Endpoint transfer status
 * The status field is updated after each transaction attempt,
 * whether successful or not.
 */
typedef enum {
	//! Transaction is successful
	UHD_TRANS_NOERROR = 0,

	//! Device is disconnected
	UHD_TRANS_DISCONNECT,

	//! CRC error in data packet
	UHD_TRANS_CRC,

	//! Data toggle PID did not match the expected value
	UHD_TRANS_DT_MISMATCH,

	//! The endpoint returned a STALL PID
	UHD_TRANS_STALL,

	//! Device did not respond to token (IN)
	//! or did not provide a handshake (OUT)
	UHD_TRANS_NOTRESPONDING,

	//! Check bits on PID from endpoint failed
	UHD_TRANS_PIDFAILURE,

	//! Data transmission not completed before timeout
	UHD_TRANS_TIMEOUT,

	//! Data transmission has been aborted
	UHD_TRANS_ABORTED,
} USBH_XfrStatus_t;

/**
 * \brief End of reset callback function type.
 * Registered by uhd_send_reset()
 * Callback called when reset event is completed.
 */
typedef void (*uhd_callback_reset_t)(void);

/**
 * \brief Data setup transfer callback function type.
 * Registered by USBH_HAL_SetupReq()
 * Called during DATA phase when the (payload) buffer is full or empty.
 * Then the setup request is halted.
 * A new buffer can be provided to continue the DATA phase or
 * abort DATA phase.
 *
 * \param add           USB address of the setup request
 * \param payload       To return the next buffer address
 * \param payload_size  To return the size of next buffer
 *
 * \return \c true, if a new buffer is available, otherwise stop the request.
 */
typedef bool (*uhd_callback_setup_run_t)(
	uint8_t add,
	uint8_t **payload,
	uint16_t *payload_size );

/**
 * \brief End of setup callback function type.
 * Registered by USBH_HAL_SetupReq()
 * Called when the setup request is completed.
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
typedef void (*uhd_callback_setup_end_t)(
	uint8_t add,
	USBH_XfrStatus_t status,
	uint16_t payload_trans);

/**
 * \brief End of transfer callback function type.
 * Registered by USBH_HAL_RunEndpoint()
 * Callback called by USB interrupt after data transfer or abort (reset,...).
 *
 * \param add           USB address used by the transfer
 * \param status        Transfer status
 * \param nb_transfered Number of data transfered
 */
typedef void (*uhd_callback_trans_t) (
	uint8_t add,
	uint8_t ep,
	USBH_XfrStatus_t status,
	uint32_t nb_transfered);

/*
 * SCB_CleanDCache_by_Addr
 * Make the address to be aligned to 32-byte boundary
 */
void USBHS_SCB_CleanDCache_by_Addr(uint32_t *addr, int32_t dsize);
/*
 * SCB_InvalidateDCache_by_Addr
 * Make the address to be aligned to 32-byte boundary
 */
void USBHS_SCB_InvalidateDCache_by_Addr(uint32_t *addr, int32_t dsize);

/**
 * \brief Enables the USB host mode
 * Start the ID pin management if the ID pin is available.
 */
void USBH_HAL_EnableUsbHost(void);

/**
 * \brief Disables the USB host mode
 *
 * \param b_id_stop  Stop ID pin management, if true.
 */
void USBH_HAL_DisableUsb(bool b_id_stop);

/**
 * \brief Returns the speed of connected device
 *
 * \return Device speed
 */
USBH_Speed_t USBH_HAL_GetSpeed(void);

/**
 * \brief Returns the current Start Of Frame (SOF) number
 *
 * \return current start of frame number.
 */
uint16_t USBH_HAL_GetFrameNum(void);

/**
 * \brief Returns the current micro start of frame number
 *
 * \return current micro start of frame number required in high speed mode.
 */
uint16_t USBH_HAL_GetMicroFrameNum(void);

/**
 * \brief Enables the Reset state on the USB line.
 *
 * \param callback  Callback when reset sequence is finished
 */
void USBH_HAL_Reset(uhd_callback_reset_t callback );

/**
 * \brief Enables the suspend state on the USB line.
 * The SUSPEND state is enable when SOF are disabled on USB line.
 */
void USBH_HAL_Suspend(void);

/**
 * \brief Test if the suspend state is enabled on the USB line.
 * \return USB line in SUSPEND state, if true
 */
bool USBH_HAL_IsSuspended(void);

/**
 * \brief Enables the IDLE state on the USB line.
 * The IDLE state is enable when SOF are present on USB line.
 * A Downstream Resume signal can be sent.
 */
void USBH_HAL_Resume(void);

#ifdef USB_HOST_LPM_SUPPORT
/**
 * \brief Enables the suspend L1 state on the USB line.
 * The SUSPEND LPM state is enable when a LPM transaction is done.
 *
 * \param b_remotewakeup Authorize the remote wakeup features, if true
 * \param besl Best effort service latency value
 *
 * \return USB line in SUSPEND state, if true
 */
bool uhd_suspend_lpm(bool b_remotewakeup, uint8_t besl);
#endif // USB_HOST_LPM_SUPPORT

/**
 * \brief Add a setup request in the control endpoint setup queue.
 * Note: Request timeout is 5s.
 *
 * \param add           USB address of control endpoint
 * \param req           Setup request definition
 * \param payload       Buffer to use in setup DATA phase
 * \param payload_size  Size of buffer used in DATA phase
 * \param callback_run  Callback to call if buffer is empty or full
 * \param callback_end  Callback to call when request is finish
 *
 * \return \c true if the request has been accepted, otherwise \c false.
 * Note: The swap of "req.wValues" from uin16_t to le16_t is done by UHD.
 */
bool USBH_HAL_SetupReq(
	uint8_t add,
	USBGenericRequest *req,
	uint8_t *payload,
	uint16_t payload_size,
	uhd_callback_setup_run_t callback_run,
	uhd_callback_setup_end_t callback_end);



/**
 * \name Endpoint Management
 *
 * The following functions allow drivers to create and remove
 * endpoints, as well as set, clear and query their "halted" and
 * "wedged" states.
 */
//@{

/**
 * \brief Configures and enables a control endpoint 0
 *
 * \param add              USB address of endpoint
 * \param ep_size          Endpoint control maximum size
 *
 * \return \c 1 if the endpoint is enabled, otherwise \c 0.
 */
bool USBH_HAL_ConfigureControlPipe(uint8_t add, uint16_t ep_size);

/**
 * \brief Configures and enables an endpoint
 *
 * \param add              USB address of endpoint
 * \param ep_desc          Endpoint descriptor
 *
 * \return \c 1 if the endpoint is enabled, otherwise \c 0.
 */
bool USBH_HAL_ConfigurePipe(uint8_t add, USBEndpointDescriptor *ep_desc);

/**
 * \brief Disables an endpoint or all endpoint of a device
 *
 * \param add              USB address of endpoint
 * \param endp             Endpoint number and direction (USB_EP_DIR_IN/OUT).
 *                         Remove all endpoints of USB address, if 0xFF.
 */
void USBH_HAL_FreePipe(uint8_t add, uint8_t endp);

/**
 * \brief Allows to receive or send data on an endpoint
 *
 * If a USB DMA is available, the driver uses it to transfer endpoint data
 * from or to internal RAM.
 * When the transfer is finished or aborted (stall, reset, ...),
 * the \a callback is called. This callback returns the transfer status
 * and eventually the number of byte transfered.
 * Note: The control endpoint is not authorized.
 *
 * \param add           USB address of endpoint
 * \param endp          Endpoint number
 * \param b_shortpacket Enabled automatic short packet
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param timeout       Transfer timeout (ms)
 * \param callback      NULL or function to call at the end of transfer
 *
 * \warning About \a b_shortpacket, for OUT endpoint it means that
 * a short packet or a Zero Length Packet must be sent to the USB line
 * to properly close the USB transfer at the end of the data transfer.
 * For Bulk and Interrupt IN endpoint, it will automatically stop the transfer
 * at the end of the data transfer (received short packet).
 *
 * \warning About \a timeout, for BULK endpoint with \a timeout set to zero,
 * it means that the transfer will never be stopped before transfer done. Since
 * most of USB embedded peripherals do not manage the transfer bandwidth by
 * peripheral hardware, such a BULK transfer will occupy all USB non-periodic
 * transfer bandwidth. In this case, other BULK transfers started later will be
 * pending until this transfer is done and bandwidth released. So it is better
 * to use BULK transfers with none zero \a timeout.
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool USBH_HAL_RunEndpoint(
	uint8_t add,
	uint8_t endp,
	bool b_shortpacket,
	uint8_t *buf,
	uint32_t buf_size,
	uint16_t timeout,
	uhd_callback_trans_t callback);

/**
 * \brief Aborts an on-going transfer on an endpoint
 *
 * If a transfer is on going, then it is stopped and
 * the callback registered is called to signal the end of transfer.
 * Note: The control endpoint is not authorized.
 *
 * \param add           USB address of endpoint
 * \param endp          Endpoint to abort
 *
 */
void USBH_HAL_AbortEndPoint(uint8_t add, uint8_t endp);

//@}



/**
 * \name UHC callbacks to provide for UHD
 *
 * The following callbacks are used by UHD.
 */
//@{

#ifndef _UHC_H_ /* uhc.h is not included before */
/**
 * \brief Starts the host mode
 */
extern void USBH_start(void);

/**
 * \brief Stops the host mode
 */
extern void USBH_stop(bool b_id_stop);
#endif

/**
 * \brief Notify device connection or disconnection
 *
 * \param b_plug  Device connection, if true
 */
extern void USBH_notify_connection(bool b_plug);

/**
 * \brief Notify each start of frame sent by driver
 *
 * \param b_micro  It is a micro start of frame, if true
 */
extern void USBH_notify_sof(bool b_micro);

/**
 * \brief Notify that a resume bus occurs
 * A resume can occur after a downstream or an upstream resume.
 */
extern void USBH_notify_resume(void);

#ifdef USB_HOST_LPM_SUPPORT
/**
 * \brief Notify that a resume bus occurs after a L1 state
 * A resume can occur after a downstream or an upstream resume.
 */
extern void USBH_notify_resume_lpm(void);
#endif

//uint32_t* Memory_Align(uint32_t* StartAddr, uint32_t align_num);

//@}

//@}

#ifdef __cplusplus
}
#endif
#endif // _UHD_H_
