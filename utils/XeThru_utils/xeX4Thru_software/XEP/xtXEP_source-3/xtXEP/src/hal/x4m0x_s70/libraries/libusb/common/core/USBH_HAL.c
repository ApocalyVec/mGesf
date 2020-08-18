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
 * \brief USB host driver
 * Compliance with common driver UHD
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "conf_usb_host.h"
#include "board.h"
#include "USBHDriver.h"
#include "USBH.h"

#include <string.h>
#include <stdlib.h>



#ifndef UHD_USB_INT_LEVEL
	#define UHD_USB_INT_LEVEL 5 // By default USB interrupt have low priority
#endif

#define USB_HOST_MAX_EP  9
#define UHD_PIPE_USED(pipe)      (USB_HOST_MAX_EP >= pipe)

#if ((UHD_PIPE_USED( 1) && Is_uhd_pipe_dma_supported( 1)) \
    ||(UHD_PIPE_USED( 2) && Is_uhd_pipe_dma_supported( 2)) \
    ||(UHD_PIPE_USED( 3) && Is_uhd_pipe_dma_supported( 3)) \
    ||(UHD_PIPE_USED( 4) && Is_uhd_pipe_dma_supported( 4)) \
    ||(UHD_PIPE_USED( 5) && Is_uhd_pipe_dma_supported( 5)) \
    ||(UHD_PIPE_USED( 6) && Is_uhd_pipe_dma_supported( 6)) \
    ||(UHD_PIPE_USED( 7) && Is_uhd_pipe_dma_supported( 7)) \
    ||(UHD_PIPE_USED( 8) && Is_uhd_pipe_dma_supported( 8)) \
    ||(UHD_PIPE_USED( 9) && Is_uhd_pipe_dma_supported( 9)) \
    ||(UHD_PIPE_USED(10) && Is_uhd_pipe_dma_supported(10)) \
    ||(UHD_PIPE_USED(11) && Is_uhd_pipe_dma_supported(11)) \
    ||(UHD_PIPE_USED(12) && Is_uhd_pipe_dma_supported(12)) \
    ||(UHD_PIPE_USED(13) && Is_uhd_pipe_dma_supported(13)) \
    ||(UHD_PIPE_USED(14) && Is_uhd_pipe_dma_supported(14)) \
    ||(UHD_PIPE_USED(15) && Is_uhd_pipe_dma_supported(15)) \
    )
# define UHD_PIPE_DMA_SUPPORTED
#endif

#if ((UHD_PIPE_USED( 1) && !Is_uhd_pipe_dma_supported( 1)) \
    ||(UHD_PIPE_USED( 2) && !Is_uhd_pipe_dma_supported( 2)) \
    ||(UHD_PIPE_USED( 3) && !Is_uhd_pipe_dma_supported( 3)) \
    ||(UHD_PIPE_USED( 4) && !Is_uhd_pipe_dma_supported( 4)) \
    ||(UHD_PIPE_USED( 5) && !Is_uhd_pipe_dma_supported( 5)) \
    ||(UHD_PIPE_USED( 6) && !Is_uhd_pipe_dma_supported( 6)) \
    ||(UHD_PIPE_USED( 7) && !Is_uhd_pipe_dma_supported( 7)) \
    ||(UHD_PIPE_USED( 8) && !Is_uhd_pipe_dma_supported( 8)) \
    ||(UHD_PIPE_USED( 9) && !Is_uhd_pipe_dma_supported( 9)) \
    ||(UHD_PIPE_USED(10) && !Is_uhd_pipe_dma_supported(10)) \
    ||(UHD_PIPE_USED(11) && !Is_uhd_pipe_dma_supported(11)) \
    ||(UHD_PIPE_USED(12) && !Is_uhd_pipe_dma_supported(12)) \
    ||(UHD_PIPE_USED(13) && !Is_uhd_pipe_dma_supported(13)) \
    ||(UHD_PIPE_USED(14) && !Is_uhd_pipe_dma_supported(14)) \
    ||(UHD_PIPE_USED(15) && !Is_uhd_pipe_dma_supported(15)) \
    )
# define UHD_PIPE_FIFO_SUPPORTED
#endif



// Optional UHC callbacks
#ifndef UHC_MODE_CHANGE
	#define UHC_MODE_CHANGE(arg)
#endif
#ifndef UHC_SOF_EVENT
	#define UHC_SOF_EVENT()
#endif
#ifndef UHC_VBUS_CHANGE
	#define UHC_VBUS_CHANGE(b_present)
#endif
#ifndef UHC_VBUS_ERROR
	#define UHC_VBUS_ERROR()
#endif

/**
 * \ingroup usb_host_group
 * \defgroup uhd_group USB Host Driver (UHD)
 *
 * \section USB_CONF USB dual role configuration
 * The defines UHD_ENABLE and UDD_ENABLE must be added in project to allow
 * the USB low level (UHD) to manage or not the dual role (device and host).
 *
 * \section USBHS_CONF USBHS Custom configuration
 * The following USBHS driver configuration must be defined in conf_usb_host.h
 * file of the application.
 *
 * UHD_USB_INT_LEVEL<br>
 * Option to change the interrupt priority (0 to 15) by default 5 (recommended).
 *
 * UHD_USB_INT_FUN<br>
 * Option to fit interrupt function to what defined in exception table.
 *
 * UHD_ISOCHRONOUS_NB_BANK<br>
 * Feature to reduce or increase isochronous endpoints buffering (1 to 2).
 * Default value 2.
 *
 * UHD_BULK_NB_BANK<br>
 * Feature to reduce or increase bulk endpoints buffering (1 to 2).
 * Default value 2.
 *
 * UHD_INTERRUPT_NB_BANK<br>
 * Feature to reduce or increase interrupt endpoints buffering (1 to 2).
 * Default value 1.
 *
 * \section Callbacks management
 * The USB driver is fully managed by interrupt and does not request periodic
 * task. Thereby, the USB events use callbacks to transfer the information.
 * The callbacks can be declared in static during compilation
 * or dynamically during code execution.
 *
 * \section Power mode management
 * The driver uses the sleepmgr service to manage the different sleep modes.
 * The sleep mode depends on USB driver state (uhd_uotghs_state_enum).
 * @{
 */


#ifndef UHD_ISOCHRONOUS_NB_BANK
	#define UHD_ISOCHRONOUS_NB_BANK 2
#else
	#if (UHD_ISOCHRONOUS_NB_BANK < 1) || (UHD_ISOCHRONOUS_NB_BANK > 2)
		#    error UHD_ISOCHRONOUS_NB_BANK must be define with 1 or 2.
	#endif
#endif

#ifndef UHD_BULK_NB_BANK
	#define UHD_BULK_NB_BANK        2
#else
	#if (UHD_BULK_NB_BANK < 1) || (UHD_BULK_NB_BANK > 2)
		#    error UHD_BULK_NB_BANK must be define with 1 or 2.
	#endif
#endif

#ifndef UHD_INTERRUPT_NB_BANK
	#define UHD_INTERRUPT_NB_BANK   1
#else
	#if (UHD_INTERRUPT_NB_BANK < 1) || (UHD_INTERRUPT_NB_BANK > 2)
		#    error UHD_INTERRUPT_NB_BANK must be define with 1 or 2.
	#endif
#endif

/**
 * \name Power management
 */
//@{
#ifndef UHD_NO_SLEEP_MGR

#include "sleepmgr.h"
//! States of USBHS interface
enum uhd_uotghs_state_enum {
	UHD_STATE_OFF = 0,
	UHD_STATE_WAIT_ID_HOST = 1,
	UHD_STATE_NO_VBUS = 2,
	UHD_STATE_DISCONNECT = 3,
	UHD_STATE_SUSPEND = 4,
	UHD_STATE_IDLE = 5,
};

/*! \brief Manages the sleep mode following the USBHS state
 *
 * \param new_state  New USBHS state
 */
static void uhd_sleep_mode(enum uhd_uotghs_state_enum new_state)
{
	enum sleepmgr_mode sleep_mode[] = {
		SLEEPMGR_BACKUP,    // UHD_STATE_OFF (not used)
		SLEEPMGR_WAIT_FAST, // UHD_STATE_WAIT_ID_HOST
		SLEEPMGR_SLEEP_WFI, // UHD_STATE_NO_VBUS
		SLEEPMGR_SLEEP_WFI, // UHD_STATE_DISCONNECT
		SLEEPMGR_WAIT_FAST, // UHD_STATE_SUSPEND
		SLEEPMGR_SLEEP_WFI, // UHD_STATE_IDLE
	};

	static enum uhd_uotghs_state_enum uhd_state = UHD_STATE_OFF;

	if (uhd_state == new_state) {
		return; // No change
	}

	if (new_state != UHD_STATE_OFF) {
		// Lock new limit
		sleepmgr_lock_mode(sleep_mode[new_state]);
	}

	if (uhd_state != UHD_STATE_OFF) {
		// Unlock old limit
		sleepmgr_unlock_mode(sleep_mode[uhd_state]);
	}

	uhd_state = new_state;
}

#else
#  define uhd_sleep_mode(arg)
#endif // UHD_NO_SLEEP_MGR
//@}


//! Store the callback to be call at the end of reset signal
static uhd_callback_reset_t uhd_reset_callback = NULL;

/**
 * \name Control endpoint low level management routine.
 *
 * This function performs control endpoint management.
 * It handles the SETUP/DATA/HANDSHAKE phases of a control transaction.
 */
//@{

/**
 * \brief Structure to store the high level setup request
 */

struct uhd_ctrl_request_t {
	//! Next setup request to process
	struct uhd_ctrl_request_t *next_request;

	//! Callback called when buffer is empty or full
	uhd_callback_setup_run_t callback_run;
	//! Callback called when request is completed
	uhd_callback_setup_end_t callback_end;

	//! Buffer to store the setup DATA phase
	uint8_t *payload;

	//! USB address of control endpoint
	uint8_t add;

	//! Setup request definition
	USBGenericRequest req;

	//! Size of buffer used in DATA phase
	uint16_t payload_size;
};

//! Entry points of setup request list
struct uhd_ctrl_request_t *uhd_ctrl_request_first;
struct uhd_ctrl_request_t *uhd_ctrl_request_last;

//! Remaining time for on-going setup request (No request on-going if equal 0)
volatile uint16_t uhd_ctrl_request_timeout;

//! Number of transfered byte on DATA phase of current setup request
uint16_t uhd_ctrl_nb_trans;

//! Flag to delay a suspend request after all on-going setup request
static bool uhd_b_suspend_requested;

//! Bit definitions to store setup request state machine
typedef enum {
	//! Wait a SETUP packet
	UHD_CTRL_REQ_PHASE_SETUP = 0,
	//! Wait a OUT data packet
	UHD_CTRL_REQ_PHASE_DATA_OUT = 1,
	//! Wait a IN data packet
	UHD_CTRL_REQ_PHASE_DATA_IN = 2,
	//! Wait a IN ZLP packet
	UHD_CTRL_REQ_PHASE_ZLP_IN = 3,
	//! Wait a OUT ZLP packet
	UHD_CTRL_REQ_PHASE_ZLP_OUT = 4,
} uhd_ctrl_request_phase_t;
uhd_ctrl_request_phase_t uhd_ctrl_request_phase;

//@}


/**
 * \name Management of bulk/interrupt/isochronous endpoints
 *
 * The UHD manages the data transfer on endpoints:
 * - Start data transfer on endpoint with USB DMA
 * - Send a ZLP packet if requested
 * - Call registered callback to signal end of transfer
 * The transfer abort and stall feature are supported.
 */
//@{

//! Structure definition to store registered jobs on a pipe
typedef struct {
	//! Callback to call at the end of transfer
	uhd_callback_trans_t call_end;

	//! Buffer located in internal RAM to send or fill during job
	uint8_t *buf;
	//! Size of buffer to send or fill
	uint32_t buf_size;
	//! Total number of transfered data on endpoint
	uint32_t nb_trans;

	//! timeout on this request (ms)
	uint16_t timeout;
	//! A job is registered on this pipe
	uint16_t busy: 1;
	//! A short packet is requested for this job on endpoint IN
	uint16_t b_shortpacket: 1;
	//! Periodic packet start for this job
	uint16_t b_periodic_start: 1;
} USBH_PipeJob_t;

//! Array to register a job on bulk/interrupt/isochronous endpoint
static USBH_PipeJob_t uhd_pipe_job[USBHS_EPT_NUM - 1];

//! Variables to manage the suspend/resume sequence
static uint8_t uhd_suspend_start;
static uint8_t uhd_resume_start;
static uint16_t uhd_pipes_unfreeze;

//@}
static const Pin USB_HOST[] = {PINS_VBUS_EN};

static void USBH_HAL_ManageSof(void);
static void USBH_HAL_ControlInterrupt(void);
static void USBH_HAL_PhaseControlSetup(void);
static void USBH_HAL_PhaseDataInStart(void);
static void USBH_HAL_PhaseDataIn(void);
static void USBH_HAL_InZLP(void);
static void USBH_HAL_PhaseDataOut(void);
static void USBH_HAL_OutZLP(void);
static void USBH_HAL_ControlReqEnd(USBH_XfrStatus_t status);
static USBH_XfrStatus_t USBH_HAL_GetPipeError(uint8_t pipe);
static uint8_t USBH_HAL_GetPipe(uint8_t Addr, uint8_t bEndpoint);

#ifdef UHD_PIPE_FIFO_SUPPORTED
	static void USBH_HAL_PipeOutReady(uint8_t pipe);
	static void USBH_HAL_PipeInReceived(uint8_t pipe);
#endif
#ifdef UHD_PIPE_DMA_SUPPORTED
	static void USBH_HAL_PipeXfrCmplt(uint8_t pipe);
	static void USBH_HAL_PipeDmaInterrupt(uint8_t pipe);
#endif

static void USBH_HAL_PipeInterrupt(uint8_t pipe);
static void USBH_HAL_PipeAbort(uint8_t pipe, USBH_XfrStatus_t status);
static void USBH_HAL_PipeXfrEnd(uint8_t pipe, USBH_XfrStatus_t status);


//--------------------------------------------------------
//--- INTERNAL ROUTINES TO MANAGED GLOBAL EVENTS

/**
 * \internal
 * \brief Function called by USBHS interrupt handler to manage USB interrupts
 *
 * USB host interrupt events are split into four sections:
 * - USB line events
 *   (VBus error, device dis/connection, SOF, reset, suspend, resume, wakeup)
 * - control endpoint events
 *   (setup reception, end of data transfer, underflow, overflow, stall, error)
 * - bulk/interrupt/isochronous endpoints events
 *   (end of data transfer, stall, error)
 *
 * Note:
 * Here, the global interrupt mask is not cleared when an USB interrupt
 * is enabled because this one can not occurred during the USB ISR
 * (=during INTX is masked).
 * See Technical reference $3.8.3 Masking interrupt requests
 * in peripheral modules.
 */
void USBHS_Handler(void)
{
	uint8_t pipe_int, pipe_dma_int;
	uint32_t status = USBHS_GetHostStatus(USBHS, 0xFF);
	uint32_t statusInt = ( status & USBHS_IsHostIntEnable(USBHS, 0xFF));

	memory_sync();
	TRACE_DEBUG("%c ", USBHS_IsUsbLowSpeed(USBHS) ? 'L' :
				(USBHS_IsUsbHighSpeed(USBHS) ? 'H' : 'F'));

	if (USBHS_IsUsbLowSpeed(USBHS)
		&& (!(USBHS->USBHS_HSTCTRL & USBHS_HSTCTRL_SPDCONF_Msk)))
		USBHS->USBHS_HSTCTRL |= USBHS_HSTCTRL_SPDCONF_LOW_POWER;

	// Manage SOF interrupt
	if (status & USBHS_HSTISR_HSOFI) {
		//Acknowledge SOF
		USBHS_ClearHostStatus(USBHS, USBHS_HSTICR_HSOFIC);
		USBH_HAL_ManageSof();
	} else {
		pipe_int = USBHS_GetInterruptPipeNum();
		pipe_dma_int = USBHS_GetInterruptPipeDmaNum();

		if (pipe_int == 0) {
			TRACE_DEBUG("Pipe0: ");
			// Interrupt acked by control endpoint managed
			USBH_HAL_ControlInterrupt();
		} else if (pipe_int != USBHS_EPT_NUM) {
			TRACE_DEBUG("Pipe%x: ", pipe_int);
			// Interrupt acked by bulk/interrupt/isochronous endpoint
			USBH_HAL_PipeInterrupt(pipe_int);
		}

#ifdef UHD_PIPE_DMA_SUPPORTED
		else if (pipe_dma_int != USBHS_EPT_NUM) {
			TRACE_DEBUG("Pipe%x ", pipe_dma_int);
			// Interrupt DMA acked by bulk/interrupt/isochronous endpoint
			USBH_HAL_PipeDmaInterrupt(pipe_dma_int);
		}

#endif
		// USB bus reset detection
		else if (status & USBHS_HSTISR_RSTI) {
			TRACE_INFO_WP("RST ");
			USBHS_ClearHostStatus(USBHS, USBHS_HSTICR_RSTIC);

			if (uhd_reset_callback != NULL)
				uhd_reset_callback();
		}

		// Check USB clock ready after asynchronous interrupt

		// Manage dis/connection event
		else if (statusInt & USBHS_HSTIMR_DDISCIE) {
			USBHS_ClearHostStatus(USBHS, USBHS_HSTICR_DDISCIC);
			USBHS_HostIntDisable(USBHS, USBHS_HSTIDR_DDISCIEC);
			TRACE_INFO("Disconnect--\n\r");
			// Stop reset signal, in case of disconnection during reset
			USBHS_StopReset();
			// Disable wakeup/resumes interrupts,
			// in case of disconnection during suspend mode
			USBHS_HostIntDisable(USBHS, (USBHS_HSTIDR_HWUPIEC
										 | USBHS_HSTIDR_RSMEDIEC
										 | USBHS_HSTIDR_RXRSMIEC));
			uhd_sleep_mode(UHD_STATE_DISCONNECT);
			USBHS_ClearHostStatus(USBHS, USBHS_HSTICR_DCONNIC);
			USBHS_HostIntEnable(USBHS, USBHS_HSTIER_DCONNIES);
			USBHS_ClearHostStatus(USBHS, USBHS_HSTICR_HWUPIC);
			USBHS_HostIntEnable(USBHS, USBHS_HSTIER_HWUPIES);
#ifdef USB_HOST_HS_SUPPORT
			USBHS_SetHostHighSpeed(USBHS);
#endif
			//otg_freeze_clock();
			uhd_suspend_start = 0;
			uhd_resume_start = 0;
			USBH_notify_connection(false);
		} else if (statusInt & USBHS_HSTIMR_DCONNIE) {
			TRACE_INFO("Connect-- ");
			USBHS_ClearHostStatus(USBHS, USBHS_HSTICR_DCONNIC);
			USBHS_HostIntDisable(USBHS, USBHS_HSTIDR_DCONNIEC);
			USBHS_ClearHostStatus(USBHS, USBHS_HSTICR_DDISCIC);
			USBHS_HostIntEnable(USBHS, USBHS_HSTIER_DDISCIES);
			USBHS_EnableSOF(USBHS);
			uhd_sleep_mode(UHD_STATE_IDLE);
			uhd_suspend_start = 0;
			uhd_resume_start = 0;
			USBH_notify_connection(true);
		}

		/* If Wakeup interrupt is enabled and triggered and connection interrupt is enabled  */
		else if ((statusInt & USBHS_HSTISR_HWUPI )
				 && USBHS_IsHostIntEnable(USBHS, USBHS_HSTIMR_DCONNIE)) {
			while (!USBHS_ISUsableClock(USBHS));

			USBHS_UnFreezeClock(USBHS);
			// Here the wakeup interrupt has been used to detect connection
			// with an asynchrone interrupt
			USBHS_HostIntDisable(USBHS, USBHS_HSTIDR_HWUPIEC);

			USBHS_Set(USBHS, USBHS_SR_VBUSRQ);// enable VBUS
			uhd_sleep_mode(UHD_STATE_DISCONNECT);
			UHC_VBUS_CHANGE(true);
		}


		else if (USBHS_IsHostIntEnable(USBHS, USBHS_HSTIMR_HWUPIE)
				 && (USBHS_GetHostStatus(USBHS,
										 (USBHS_HSTISR_HWUPI | USBHS_HSTISR_RSMEDI | USBHS_HSTISR_RXRSMI)))) {

			TRACE_INFO_WP("\n\rWKP ");

			while (!USBHS_ISUsableClock(USBHS));

			USBHS_UnFreezeClock(USBHS);
			// Disable wakeup/resumes interrupts
			USBHS_HostIntDisable(USBHS, (USBHS_HSTIDR_HWUPIEC
										 | USBHS_HSTIDR_RSMEDIEC
										 | USBHS_HSTIDR_RXRSMIEC));
			USBHS_EnableSOF(USBHS);

			if ((!(USBHS_HSTISR_RSMEDI & status))
				&& (!(USBHS_HSTISR_DDISCI & status))) {
				// It is a upstream resume
				// Note: When the CPU exits from a deep sleep mode, the event
				// Is_uhd_upstream_resume() can be not detected
				// because the USB clock are not available.

				// In High speed mode a downstream resume must be sent
				// after a upstream to avoid a disconnection.
				if (USBHS_IsUsbHighSpeed(USBHS))
					USBHS_Resume();
			}

			// Wait 50ms before restarting transfer
			uhd_resume_start = 50;
			uhd_sleep_mode(UHD_STATE_IDLE);
		} else {
			assert(false); // Interrupt event no managed
		}

		//TRACE_INFO_WP("\n\r");
	}
}


void USBH_HAL_EnableUsbHost(void)
{
	irqflags_t flags;

	// To avoid USB interrupt before end of initialization
	flags = cpu_irq_save();

	//USB_Initialized = true;

	//* Enable USB hardware clock
	sysclk_enable_usb();
	PMC_EnablePeripheral(ID_USBHS);

	// Always authorize asynchronous USB interrupts to exit of sleep mode
	// For SAM3 USB wake up device except BACKUP mode
	NVIC_SetPriority((IRQn_Type) ID_USBHS, UHD_USB_INT_LEVEL);
	NVIC_EnableIRQ((IRQn_Type) ID_USBHS);

	uhd_sleep_mode(UHD_STATE_OFF);

	PIO_Configure(USB_HOST, PIO_LISTSIZE(USB_HOST));

	PIO_Set(USB_HOST);// Power off USB devices

	USBHS_UsbMode(USBHS, HOST_MODE);

	USBHS_UsbEnable(USBHS, true);

#ifndef USB_HOST_HS_SUPPORT
	USBHS->USBHS_HSTCTRL |= USBHS_HSTCTRL_SPDCONF_LOW_POWER;
#endif

	uhd_ctrl_request_first = NULL;
	uhd_ctrl_request_last = NULL;
	uhd_ctrl_request_timeout = 0;
	uhd_suspend_start = 0;
	uhd_resume_start = 0;
	uhd_b_suspend_requested = false;

	// Check USB clock
	USBHS_UnFreezeClock(USBHS);

	while (!USBHS_ISUsableClock(USBHS));

	// Clear all interrupts that may have been set by a previous host mode
	USBHS_ClearHostStatus(USBHS, (USBHS_HSTICR_DCONNIC | USBHS_HSTICR_DDISCIC
								  | USBHS_HSTICR_HSOFIC | USBHS_HSTICR_HWUPIC
								  | USBHS_HSTICR_RSMEDIC | USBHS_HSTICR_RSTIC
								  | USBHS_HSTICR_RXRSMIC));
	memory_sync();

	USBHS_VBusHWC(USBHS, false);
	uhd_sleep_mode(UHD_STATE_NO_VBUS);

	/** Freeze USB clock to use wakeup interrupt
	* to detect connection.
	* After detection of wakeup interrupt,
	* the clock is unfreeze to have the true
	* connection interrupt.
	*/
	// wakeup
	USBHS_HostIntEnable(USBHS, USBHS_HSTIER_HWUPIES);
	uhd_sleep_mode(UHD_STATE_DISCONNECT);
	UHC_VBUS_CHANGE(true);/* Changed to HIGH */

	PIO_Clear(USB_HOST);// power on USB device

	// Enable main control interrupt
	// Connection, SOF and reset
	USBHS_HostIntEnable(USBHS,
						(USBHS_HSTIER_RSTIES | USBHS_HSTIER_HSOFIES | USBHS_HSTIER_DCONNIES));

	USBHS_FreezeClock(USBHS);

	cpu_irq_restore(flags);

}

void USBH_HAL_DisableUsb(bool b_id_stop)
{
	irqflags_t flags;

	b_id_stop = b_id_stop;
	// Check USB clock ready after a potential sleep mode < IDLE
	while (!USBHS_ISUsableClock(USBHS));

	USBHS_UnFreezeClock(USBHS);
	printf("USB disabled \n\r");

	// (Connection, disconnection, SOF and reset)
	USBHS_HostIntDisable(USBHS, (USBHS_HSTICR_DCONNIC | USBHS_HSTICR_DDISCIC
								 | USBHS_HSTICR_HSOFIC | USBHS_HSTICR_HWUPIC
								 | USBHS_HSTICR_RSMEDIC | USBHS_HSTICR_RSTIC
								 | USBHS_HSTICR_RXRSMIC));

	USBHS_DisableSOF();
	USBHS_Ack(USBHS, USBHS_SR_VBUSRQ);
	USBH_notify_connection(false);

	flags = cpu_irq_save();
	USBHS_FreezeClock(USBHS);
	USBHS_UsbEnable(USBHS, false);
	sysclk_disable_usb();
	PMC_DisablePeripheral(ID_USBHS);
	uhd_sleep_mode(UHD_STATE_OFF);
	cpu_irq_restore(flags);
}

USBH_Speed_t USBH_HAL_GetSpeed(void)
{
	switch (USBHS_GetUsbSpeed(USBHS)) {

	case USBHS_SR_SPEED_HIGH_SPEED:
		return UHD_SPEED_HIGH;

	case USBHS_SR_SPEED_FULL_SPEED:
		return UHD_SPEED_FULL;

	case USBHS_SR_SPEED_LOW_SPEED:
		return UHD_SPEED_LOW;

	default:
		assert(false);
		return UHD_SPEED_LOW;
	}
}

uint16_t USBH_HAL_GetFrameNum(void)
{
	return USBHS_HostGetSOF();
}

uint16_t USBH_HAL_GetMicroFrameNum(void)
{
	return USBHS_HostGetMSOF();
}

void USBH_HAL_Reset(uhd_callback_reset_t callback)
{
	uhd_reset_callback = callback;
	USBHS_Reset();
}

void USBH_HAL_Suspend(void)
{
	uint8_t pipe;

	if (uhd_ctrl_request_timeout) {
		// Delay suspend after setup requests
		uhd_b_suspend_requested = true;
		return;
	}

	// Save pipe freeze states and freeze pipes
	uhd_pipes_unfreeze = 0;

	for (pipe = 1; pipe < USBHS_EPT_NUM; pipe++) {
		uhd_pipes_unfreeze |= (!USBHS_IsHostPipeIntTypeEnable(USBHS, pipe,
							   USBHS_HSTPIPIMR_PFREEZE)) << pipe;
		USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_PFREEZES);
	}

	// Wait three SOFs before entering in suspend state
	uhd_suspend_start = 3;
}

bool USBH_HAL_IsSuspended(void)
{
	return !USBHS_IsEnableSOF(USBHS);
}

void USBH_HAL_Resume(void)
{
	if (USBHS_IsEnableSOF(USBHS)) {
		// Currently in IDLE mode (!=Suspend)
		if (uhd_suspend_start) {
			// Suspend mode on going
			// then stop it and start resume event
			uhd_suspend_start = 0;
			uhd_resume_start = 1;
		}

		return;
	}

	// Check USB clock ready after a potential sleep mode < IDLE
	while (!USBHS_ISUsableClock(USBHS));

	USBHS_UnFreezeClock(USBHS);
	USBHS_EnableSOF(USBHS);
	USBHS_Resume();
	uhd_sleep_mode(UHD_STATE_IDLE);
}

bool USBH_HAL_ConfigureControlPipe(uint8_t Addr, uint16_t ep_size)
{
	uint8_t bSizeEpt;

	if (ep_size < 8)
		return false;

#ifdef USB_HOST_HUB_SUPPORT

	if (USBHS_IsHostPipeEnable(USBHS, 0)) {
		// Already allocated
#error TODO Add USB address in a list
		return true;
	}

#endif

	USBHS_HostPipeEnable(USBHS, 0);

	/* Configure endpoint size */
	if (ep_size <= 8)
		bSizeEpt = 0;
	else if (ep_size <= 16)
		bSizeEpt = 1;
	else if (ep_size <= 32)
		bSizeEpt = 2;
	else if (ep_size <= 64)
		bSizeEpt = 3;
	else if (ep_size <= 128)
		bSizeEpt = 4;
	else if (ep_size <= 256)
		bSizeEpt = 5;
	else if (ep_size <= 512)
		bSizeEpt = 6;
	else if (ep_size <= 1024)
		bSizeEpt = 7;

#ifdef USB_HOST_HUB_SUPPORT
	bSizeEpt = 3; //64 bytes
#endif
	USBHS_HostConfigure(USBHS, 0, USBHS_HSTPIPCFG_PBK_1_BANK, bSizeEpt,
						USBHS_HSTPIPCFG_PTYPE_CTRL, USBHS_HSTPIPCFG_PTOKEN_SETUP, 0, 0);
	USBHS_HostDisableAutoSw(USBHS, 0);
	USBHS_HostAllocMem(USBHS, 0);

	if (!USBHS_IsHostConfigOk(USBHS, 0)) {
		USBHS_HostPipeDisable(USBHS, 0);
		return false;
	}

	USBHS_HostSetAddr(USBHS, 0, Addr);

	// Always enable stall and error interrupts of control endpoint
	USBHS_HostEnablePipeIntType(USBHS, 0,
								(USBHS_HSTPIPIER_RXSTALLDES | USBHS_HSTPIPIER_PERRES));
	USBHS_HostPipeIntEnable(USBHS, 0);
	return true;
}

bool USBH_HAL_ConfigurePipe(uint8_t Addr, USBEndpointDescriptor *ep_desc)
{
	uint32_t ep_type, ep_dir;
	uint8_t ep_interval;
	uint8_t pipe;
	uint8_t bank, ep_addr, bSizeEpt;

	for (pipe = 1; pipe < USBHS_EPT_NUM; pipe++) {
		if (USBHS_IsHostPipeEnable(USBHS, pipe))
			continue;

		USBHS_HostPipeEnable(USBHS, pipe);
		ep_addr = ep_desc->bEndpointAddress & USB_EP_ADDR_MASK;
		ep_dir = (ep_desc->bEndpointAddress & USB_EP_DIR_IN) ?
				 (USBHS_HSTPIPCFG_PTOKEN_IN) :
				 (USBHS_HSTPIPCFG_PTOKEN_OUT),
				 ep_type = ep_desc->bmAttributes & USB_EP_TYPE_MASK;

		// Bank choice
		switch (ep_type) {
		case USBEndpointDescriptor_ISOCHRONOUS:
			bank = UHD_ISOCHRONOUS_NB_BANK;
			ep_interval = ep_desc->bInterval;
			ep_type = USBHS_HSTPIPCFG_PTYPE_ISO;
			break;

		case USBEndpointDescriptor_INTERRUPT:
			bank = UHD_INTERRUPT_NB_BANK;
			ep_interval = ep_desc->bInterval;
			ep_type = USBHS_HSTPIPCFG_PTYPE_INTRPT;
			break;

		case USBEndpointDescriptor_BULK:
			bank = UHD_BULK_NB_BANK;
			// 0 is required by USBHS hardware for bulk
			ep_interval = 0;
			ep_type = USBHS_HSTPIPCFG_PTYPE_BLK;
			break;

		default:
			assert(false);
			return false;
		}

		switch (bank) {
		case 1:
			bank = USBHS_HSTPIPCFG_PBK_1_BANK;
			break;

		case 2:
			bank = USBHS_HSTPIPCFG_PBK_2_BANK;
			break;

		case 3:
			bank = USBHS_HSTPIPCFG_PBK_3_BANK;
			break;

		default:
			assert(false);
			return false;
		}

		/* Configure endpoint size */
		if (ep_desc->wMaxPacketSize <= 8)
			bSizeEpt = 0;
		else if (ep_desc->wMaxPacketSize <= 16)
			bSizeEpt = 1;
		else if (ep_desc->wMaxPacketSize <= 32)
			bSizeEpt = 2;
		else if (ep_desc->wMaxPacketSize <= 64)
			bSizeEpt = 3;
		else if (ep_desc->wMaxPacketSize <= 128)
			bSizeEpt = 4;
		else if (ep_desc->wMaxPacketSize <= 256)
			bSizeEpt = 5;
		else if (ep_desc->wMaxPacketSize <= 512)
			bSizeEpt = 6;
		else if (ep_desc->wMaxPacketSize <= 1024)
			bSizeEpt = 7;

		USBHS_HostConfigure(USBHS, pipe, bank, bSizeEpt, ep_type, ep_dir,
							ep_addr, ep_interval);
		USBHS_HostEnableAutoSw(USBHS, pipe);

		USBHS_HostAllocMem(USBHS, pipe);

		if (!USBHS_IsHostConfigOk(USBHS, pipe)) {
			USBHS_HostPipeDisable(USBHS, pipe);
			return false;
		}

		USBHS_HostSetAddr(USBHS, pipe, Addr);

		USBHS_HostPipeEnable(USBHS, pipe);

		// Enable endpoint interrupts
		USBHS_HostDmaIntEnable(USBHS, pipe - 1);
		USBHS_HostEnablePipeIntType(USBHS, pipe,
									(USBHS_HSTPIPIER_RXSTALLDES | USBHS_HSTPIPIER_PERRES) );

		USBHS_HostPipeIntEnable(USBHS, pipe);

		return true;
	}

	return false;
}


void USBH_HAL_FreePipe(uint8_t Addr, uint8_t bEndpoint)
{
	uint8_t pipe;
#ifdef USB_HOST_HUB_SUPPORT

	if (bEndpoint == 0) {
		// Control endpoint does not be unallocated
#error TODO the list address must be updated
		if (uhd_ctrl_request_timeout
			&& (uhd_ctrl_request_first->add == Addr)) {
			// Disable setup request if on this device
			USBH_HAL_ControlReqEnd(UHD_TRANS_DISCONNECT);
		}

		return;
	}

#endif

	// Search endpoint(s) in all pipes
	for (pipe = 0; pipe < USBHS_EPT_NUM; pipe++) {
		if (!USBHS_IsHostPipeEnable(USBHS, pipe))
			continue;

		if (Addr != USBHS_HostGetAddr(USBHS, pipe))
			continue;

		if (bEndpoint != 0xFF) {
			// Disable specific endpoint number
			if (bEndpoint != USBHS_GetPipeEpAddr(USBHS, pipe)) {
				continue; // Mismatch
			}
		}

		// Unalloc pipe
		USBHS_HostPipeDisable(USBHS, pipe);
		USBHS_HostFreeMem(USBHS, pipe);

		// Stop transfer on this pipe
#ifndef USB_HOST_HUB_SUPPORT

		if (pipe == 0) {
			// Endpoint control
			if (uhd_ctrl_request_timeout)
				USBH_HAL_ControlReqEnd(UHD_TRANS_DISCONNECT);

			continue;
		}

#endif
		// Endpoint interrupt, bulk or isochronous
		USBH_HAL_PipeAbort(pipe, UHD_TRANS_DISCONNECT);
	}
}

bool USBH_HAL_SetupReq(
	uint8_t Addr,
	USBGenericRequest *req,
	uint8_t *payload,
	uint16_t payload_size,
	uhd_callback_setup_run_t callback_run,
	uhd_callback_setup_end_t callback_end)
{
	irqflags_t flags;
	struct uhd_ctrl_request_t *request;
	bool b_start_request = false;

	request = malloc(sizeof(struct uhd_ctrl_request_t));

	if (request == NULL) {
		assert(false);
		return false;
	}

	// Fill structure
	request->add = (uint8_t) Addr;
	memcpy(&request->req, req, sizeof(USBGenericRequest));
	request->payload = payload;
	request->payload_size = payload_size;
	request->callback_run = callback_run;
	request->callback_end = callback_end;
	request->next_request = NULL;

	// Add this request in the queue
	flags = cpu_irq_save();

	if (uhd_ctrl_request_first == NULL) {
		uhd_ctrl_request_first = request;
		b_start_request = true;
	} else
		uhd_ctrl_request_last->next_request = request;

	uhd_ctrl_request_last = request;
	cpu_irq_restore(flags);

	if (b_start_request) {
		// Start immediately request
		USBH_HAL_PhaseControlSetup();
	}

	return true;
}

bool USBH_HAL_RunEndpoint(uint8_t Addr,
						  uint8_t bEndpoint,
						  bool b_shortpacket,
						  uint8_t *buf,
						  uint32_t buf_size,
						  uint16_t timeout,
						  uhd_callback_trans_t callback)
{
	irqflags_t flags;
	uint8_t pipe;
	USBH_PipeJob_t *pJob;

	pipe = USBH_HAL_GetPipe(Addr, bEndpoint);

	if (pipe == USBHS_EPT_NUM) {
		return false; // pipe not found
	}

#ifdef UHD_PIPE_FIFO_SUPPORTED
	bool b_pipe_in = (USBHS_HostGetToken(USBHS,
										 pipe) == USBHS_HSTPIPCFG_PTOKEN_IN) ? true : false;
#endif

	// Get job about pipe
	pJob = &uhd_pipe_job[pipe - 1];
	flags = cpu_irq_save();

	if (pJob->busy == true) {
		cpu_irq_restore(flags);
		return false;// Job already on going
	}

	pJob->busy = true;

	// No job running. Let's setup a new one.
	pJob->buf = buf;
	pJob->buf_size = buf_size;
	pJob->nb_trans = 0;
	pJob->timeout = timeout;
	pJob->b_shortpacket = b_shortpacket;
	pJob->call_end = callback;

	if ((USBHS_HostGetPipeType(USBHS, pipe) & (USBHS_HSTPIPCFG_PTYPE_ISO
			| USBHS_HSTPIPCFG_PTYPE_INTRPT))
		&& (USBHS_HostGetToken(USBHS, pipe) == USBHS_HSTPIPCFG_PTOKEN_OUT))
		/*if ((Is_uhd_pipe_int(pipe) || Is_uhd_pipe_iso(pipe))
		        && (Is_uhd_pipe_out(pipe))) */
		pJob->b_periodic_start = true;

	cpu_irq_restore(flags);

#ifdef UHD_PIPE_FIFO_SUPPORTED

	// No DMA support
	if (!Is_uhd_pipe_dma_supported(pipe)) {
		flags = cpu_irq_save();
		USBHS_HostDisableAutoSw(USBHS, pipe);
		USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_PFREEZEC);

		if (b_pipe_in) {
			USBHS_HostEnableInReq(USBHS, pipe);
			USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_RXINES);

			if (b_shortpacket)
				USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_SHORTPACKETIES);
		} else {
			USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_NBUSYBKEC);
			USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_TXOUTES);
		}

		USBHS_HostPipeIntEnable(USBHS, pipe);
		cpu_irq_restore(flags);
		return true;
	}

#endif // UHD_PIPE_FIFO_SUPPORTED

#ifdef UHD_PIPE_DMA_SUPPORTED
	// Request first transfer
	USBH_HAL_PipeXfrCmplt(pipe);
#endif
	return true;
}

void USBH_HAL_AbortEndPoint(uint8_t Addr, uint8_t bEndpoint)
{
	uint8_t pipe;

	pipe = USBH_HAL_GetPipe(Addr, bEndpoint);

	if (pipe == USBHS_EPT_NUM) {
		return; // pipe not found
	}

	USBH_HAL_PipeAbort(pipe, UHD_TRANS_ABORTED);
}


static void USBH_HAL_DelayedSuspend(void)
{
	volatile uint8_t AsyncInt = (USBHS_HSTIER_HWUPIES | USBHS_HSTIER_RSMEDIES
								 | USBHS_HSTIER_RXRSMIES);

	if (--uhd_suspend_start == 0) {
		// In case of high CPU frequency,
		// the current Keep-Alive/SOF can be always on-going
		// then wait end of SOF generation
		// to be sure that disable SOF has been accepted
		uint8_t pos =
			(USBHS_GetUsbSpeed(USBHS) == USBHS_SR_SPEED_HIGH_SPEED) ?
			13 : 114;

		while (pos < USBHS_HostGetFramePos()) {
			if (USBHS_GetHostStatus(USBHS, USBHS_HSTISR_DDISCI))
				break;
		}

		USBHS_DisableSOF();

		// When SOF is disabled, the current transmitted packet may
		// cause a resume.
		// Wait for a while to check this resume status and clear it.
		for (pos = 0; pos < 15; pos ++) {
			memory_barrier();

			if (USBHS_GetHostStatus(USBHS, USBHS_HSTICR_HWUPIC
									| USBHS_HSTICR_RSMEDIC
									| USBHS_HSTICR_RXRSMIC))
				break;
		}


		// Ack previous wakeup and resumes interrupts
		USBHS_ClearHostStatus(USBHS, (USBHS_HSTICR_HWUPIC
									  | USBHS_HSTICR_RSMEDIC
									  | USBHS_HSTICR_RXRSMIC));

		memory_sync();
		// Enable wakeup/resumes interrupts
		USBHS_HostIntEnable(USBHS, AsyncInt);


		USBHS_FreezeClock(USBHS);
		uhd_sleep_mode(UHD_STATE_SUSPEND);
	}
}
static void USBH_HAL_DelayedResume(void)
{
	uint8_t pipe;

	if (--uhd_resume_start == 0) {
		// Restore pipes unfreezed
		for (pipe = 1; pipe < USBHS_EPT_NUM; pipe++) {
			if ((uhd_pipes_unfreeze >> pipe) & 0x01)
				USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_PFREEZEC);
		}

		USBH_notify_resume();
	}
}
static void USBH_HAL_ControlTimeout(void)
{
	if (uhd_ctrl_request_timeout) {
		// Setup request on-going
		if (--uhd_ctrl_request_timeout == 0) {
			TRACE_WARNING_WP("Timeout");
			// Stop request by freezing pipe
			USBHS_HostEnablePipeIntType(USBHS, 0, USBHS_HSTPIPIER_PFREEZES);
			USBH_HAL_ControlReqEnd(UHD_TRANS_TIMEOUT);
		}
	}
}

/**
 * \internal
 * \brief Manages timeouts and actions based on SOF events
 * - Suspend delay
 * - Resume delay
 * - Setup packet delay
 * - Timeout on endpoint control transfer
 * - Timeouts on bulk/interrupt/isochronous endpoint transfers
 * - UHC user notification
 * - SOF user notification
 */
static void USBH_HAL_ManageSof(void)
{
	//  USBHS_HostAckPipeIntType(USBHS, USBHS_HSTICR_HSOFIC);
	uint8_t pipe;

	// Manage the micro SOF
	if (USBHS_IsUsbHighSpeed(USBHS)) {
		static uint8_t msof_cpt = 0;

		if (++msof_cpt % 8) {
			// It is a micro SOF
			if (!uhd_suspend_start && !uhd_resume_start) {
				// If no resume and no suspend on going
				// then send Micro start of frame event (each 125?)
				USBH_notify_sof(true);
			}

			return;
		}
	}

	// Manage a delay to enter in suspend
	if (uhd_suspend_start) {
		USBH_HAL_DelayedSuspend();
		return; // Abort SOF events
	}

	// Manage a delay to exit of suspend
	if (uhd_resume_start) {
		USBH_HAL_DelayedResume();
		return; // Abort SOF events
	}

	// Manage the timeout on endpoint control transfer
	USBH_HAL_ControlTimeout();

	// Manage the timeouts on endpoint transfer
	USBH_PipeJob_t *pJob;

	for (pipe = 1; pipe < USBHS_EPT_NUM; pipe++) {
		pJob = &uhd_pipe_job[pipe - 1];

		if (pJob->busy == true) {
			if (pJob->timeout) {
				// Timeout enabled on this job
				if (--(pJob->timeout) == 0) {
					// Abort job
					USBH_HAL_PipeAbort(pipe, UHD_TRANS_TIMEOUT);
				}
			}

			if (pJob->b_periodic_start) {
				pJob->b_periodic_start = false;
				USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_PFREEZEC);
			}
		}
	}

	// Notify the UHC
	USBH_notify_sof(false);

	// Notify the user application
	UHC_SOF_EVENT();
	memory_sync();
}

/**
 * \internal
 * \brief Manages the events related to control endpoint
 */
static void USBH_HAL_ControlInterrupt(void)
{
	// A setup request is on-going
	assert(uhd_ctrl_request_timeout != 0);

	// Disable setup, IN and OUT interrupts of control endpoint
	USBHS_HostDisablePipeIntType(USBHS, 0, (USBHS_HSTPIPIDR_TXSTPEC
											| USBHS_HSTPIPIDR_RXINEC
											| USBHS_HSTPIPIDR_TXOUTEC) );

	// Search event on control endpoint
	if (USBHS_HostGetIntTypeStatus(USBHS, 0, USBHS_HSTPIPISR_TXSTPI) ) {
		// SETUP packet sent
		USBHS_HostEnablePipeIntType(USBHS, 0, USBHS_HSTPIPIER_PFREEZES);
		USBHS_HostAckPipeIntType(USBHS, 0, USBHS_HSTPIPICR_TXSTPIC);
		assert(uhd_ctrl_request_phase == UHD_CTRL_REQ_PHASE_SETUP);

		// Start DATA phase
		if ((uhd_ctrl_request_first->req.bmRequestType & USB_REQ_DIR_MASK)
			== USB_REQ_DIR_IN)
			USBH_HAL_PhaseDataInStart();
		else {
			if (uhd_ctrl_request_first->req.wLength)
				USBH_HAL_PhaseDataOut();
			else {
				// No DATA phase
				USBH_HAL_InZLP();
			}
		}

		return;
	}

	if (USBHS_HostGetIntTypeStatus(USBHS, 0, USBHS_HSTPIPISR_RXINI)) {
		// In case of low USB speed and with a high CPU frequency,
		// a ACK from host can be always running on USB line
		// then wait end of ACK on IN pipe.
		while (!USBHS_IsHostPipeIntTypeEnable(USBHS, 0, USBHS_HSTPIPIMR_PFREEZE));

		// IN packet received, Acknowledge
		USBHS_HostAckPipeIntType(USBHS, 0, USBHS_HSTPIPICR_RXINIC);

		switch (uhd_ctrl_request_phase) {
		case UHD_CTRL_REQ_PHASE_DATA_IN:
			USBH_HAL_PhaseDataIn();
			break;

		case UHD_CTRL_REQ_PHASE_ZLP_IN:
			USBH_HAL_ControlReqEnd(UHD_TRANS_NOERROR);
			break;

		default:
			assert(false);
			break;
		}

		return;
	}

	if (USBHS_HostGetIntTypeStatus(USBHS, 0, USBHS_HSTPIPISR_TXOUTI)) {
		// OUT packet sent
		USBHS_HostEnablePipeIntType(USBHS, 0, USBHS_HSTPIPIER_PFREEZES);
		USBHS_HostAckPipeIntType(USBHS, 0, USBHS_HSTPIPICR_TXOUTIC);

		switch (uhd_ctrl_request_phase) {
		case UHD_CTRL_REQ_PHASE_DATA_OUT:
			USBH_HAL_PhaseDataOut();
			break;

		case UHD_CTRL_REQ_PHASE_ZLP_OUT:
			USBH_HAL_ControlReqEnd(UHD_TRANS_NOERROR);
			break;

		default:
			assert(false);
			break;
		}

		return;
	}

	if (USBHS_HostGetIntTypeStatus(USBHS, 0, USBHS_HSTPIPISR_RXSTALLDI)) {
		// Stall Handshake received
		USBHS_HostAckPipeIntType(USBHS, 0, USBHS_HSTPIPICR_RXSTALLDIC);
		USBH_HAL_ControlReqEnd(UHD_TRANS_STALL);
		return;
	}

	if (USBHS_HostGetIntTypeStatus(USBHS, 0, USBHS_HSTPIPISR_PERRI)) {
		// Get and ack error
		USBH_HAL_ControlReqEnd(USBH_HAL_GetPipeError(0));
		printf("Control pipe error \n\r");
		return;
	}

	memory_sync();
	assert(false); // Error system
}

/**
 * \internal
 * \brief Sends a USB setup packet to start a control request sequence
 */
static void USBH_HAL_PhaseControlSetup(void)
{
	union {
		volatile uint64_t value64;
		USBGenericRequest req;
	} setup;
	volatile uint64_t *pEpData;

	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_SETUP;
	memcpy(&setup.req, &uhd_ctrl_request_first->req, sizeof(USBGenericRequest));
	memory_sync();

	uhd_ctrl_nb_trans = 0;

	// Check pipe
#ifdef USB_HOST_HUB_SUPPORT

	if (!USBHS_IsHostPipeEnable(USBHS, 0)) {
		USBH_HAL_ControlReqEnd(UHD_TRANS_DISCONNECT);
		return; // Endpoint not valid
	}

#error TODO check address in list
	// Reconfigure USB address of pipe 0 used for all control endpoints
	uhd_configure_address(0, uhd_ctrl_request_first->add);
#else

	if (!USBHS_IsHostPipeEnable(USBHS, 0) ||
		(uhd_ctrl_request_first->add != USBHS_HostGetAddr(USBHS, 0))) {
		USBH_HAL_ControlReqEnd(UHD_TRANS_DISCONNECT);
		return; // Endpoint not valid
	}

#endif

	// Fill pipe
	USBHS_HostSetToken(USBHS, 0, USBHS_HSTPIPCFG_PTOKEN_SETUP);
	USBHS_HostAckPipeIntType(USBHS, 0, USBHS_HSTPIPICR_TXSTPIC);
	assert(sizeof(setup) == sizeof(uint64_t));

	pEpData = (volatile uint64_t *)USBHS_RAM_ADDR;
	memory_sync();
	*pEpData = setup.value64;
	memory_barrier();

	uhd_ctrl_request_timeout = 5000;
	USBHS_HostEnablePipeIntType(USBHS, 0, USBHS_HSTPIPIER_TXSTPES);
	// Acknowledge FIFO control and unfreeze Pipe
	USBHS_HostDisablePipeIntType(USBHS, 0,
								 (USBHS_HSTPIPIDR_FIFOCONC | USBHS_HSTPIPIDR_PFREEZEC));

}

/**
 * \internal
 * \brief Starts the DATA IN phase on control endpoint
 */
static void USBH_HAL_PhaseDataInStart(void)
{
	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_DATA_IN;
	// set pipe token
	USBHS_HostSetToken(USBHS, 0, USBHS_HSTPIPCFG_PTOKEN_IN);
	// Ack In and Short packet
	USBHS_HostAckPipeIntType(USBHS, 0,
							 (USBHS_HSTPIPICR_RXINIC | USBHS_HSTPIPICR_SHORTPACKETIC) );

	USBHS_HostEnablePipeIntType(USBHS, 0, USBHS_HSTPIPIER_RXINES);
	// Acknowledge FIFO control and unfreeze Pipe
	USBHS_HostDisablePipeIntType(USBHS, 0,
								 (USBHS_HSTPIPIDR_FIFOCONC | USBHS_HSTPIPIDR_PFREEZEC) );

	memory_sync();
}

/**
 * \internal
 * \brief Manages the DATA IN phase on control endpoint
 */
static void USBH_HAL_PhaseDataIn(void)
{
	bool b_short_packet;
	uint8_t *pEpData;
	uint8_t bPipe = 0;
	uint8_t nb_byte_received;

	// Get information to read data
	nb_byte_received = USBHS_HostGetPipeByteCount(USBHS, bPipe);
#ifdef USB_HOST_HUB_SUPPORT
	//! In HUB mode, the control pipe is always configured to 64B
	//! thus the short packet flag must be computed
	b_short_packet = (nb_byte_received != USBHS_HostGetSize(USBHS, 0));
	USBHS_HostAckPipeIntType(USBHS, 0, USBHS_HSTPIPICR_SHORTPACKETIC);
#else
	b_short_packet = (USBHS_HostGetIntTypeStatus(USBHS, 0,
					  USBHS_HSTPIPISR_SHORTPACKETI) == USBHS_HSTPIPISR_SHORTPACKETI) ? true : false;
#endif

	pEpData = (uint8_t *)((uint32_t *)USBHS_RAM_ADDR
						  + (EPT_VIRTUAL_SIZE * bPipe));
uhd_ctrl_receiv_in_read_data:
	memory_sync();

	// Copy data from pipe to payload buffer
	while (uhd_ctrl_request_first->payload_size && nb_byte_received) {
		*uhd_ctrl_request_first->payload++ = *pEpData++;
		memory_sync();
		uhd_ctrl_nb_trans++;
		uhd_ctrl_request_first->payload_size--;
		nb_byte_received--;
		memory_barrier();
	}

	if (!uhd_ctrl_request_first->payload_size && nb_byte_received) {
		// payload buffer is full to store data remaining
		if (uhd_ctrl_request_first->callback_run == NULL
			|| !uhd_ctrl_request_first->callback_run(
				USBHS_HostGetAddr(USBHS, bPipe),
				&uhd_ctrl_request_first->payload,
				&uhd_ctrl_request_first->payload_size)) {
			// DATA phase aborted by host
			goto uhd_ctrl_phase_data_in_end;
		}

		// The payload buffer has been updated by the callback
		// thus the data load can restart.
		goto uhd_ctrl_receiv_in_read_data;
	}

	// Test short packet
	if ((uhd_ctrl_nb_trans == uhd_ctrl_request_first->req.wLength)
		|| b_short_packet) {
		// End of DATA phase or DATA phase abort from device
uhd_ctrl_phase_data_in_end:
		USBH_HAL_OutZLP();
		return;
	}

	// Send a new IN packet request
	USBHS_HostEnablePipeIntType(USBHS, bPipe, USBHS_HSTPIPIER_RXINES);
	// ack Fifocon and unfreeze pipe
	USBHS_HostDisablePipeIntType(USBHS, bPipe,
								 (USBHS_HSTPIPIDR_FIFOCONC | USBHS_HSTPIPIDR_PFREEZEC));
}

/**
 * \internal
 * \brief Starts the ZLP IN phase on control endpoint
 */
static void USBH_HAL_InZLP(void)
{
	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_ZLP_IN;
	USBHS_HostSetToken(USBHS, 0, USBHS_HSTPIPCFG_PTOKEN_IN);
	USBHS_HostAckPipeIntType(USBHS, 0,
							 (USBHS_HSTPIPICR_RXINIC | USBHS_HSTPIPICR_SHORTPACKETIC));
	/*uhd_ack_in_received(0);
	uhd_ack_short_packet(0);*/
	USBHS_HostEnablePipeIntType(USBHS, 0, USBHS_HSTPIPIER_RXINES);
	// ack Fifocon and unfreeze pipe
	USBHS_HostDisablePipeIntType(USBHS, 0,
								 (USBHS_HSTPIPIDR_FIFOCONC | USBHS_HSTPIPIDR_PFREEZEC));
	memory_sync();
}

/**
 * \internal
 * \brief Manages the DATA OUT phase on control endpoint
 */
static void USBH_HAL_PhaseDataOut(void)
{
	uint8_t *pEpData;
	uint8_t bPipe = 0;
	uint8_t ep_ctrl_size;

	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_DATA_OUT;

	if (uhd_ctrl_nb_trans == uhd_ctrl_request_first->req.wLength) {
		// End of DATA phase
		USBH_HAL_InZLP();
		return;
	}

	if (!uhd_ctrl_request_first->payload_size) {
		// Buffer empty, then request a new buffer
		if (uhd_ctrl_request_first->callback_run == NULL
			|| !uhd_ctrl_request_first->callback_run(
				USBHS_HostGetAddr(USBHS, bPipe),
				&uhd_ctrl_request_first->payload,
				&uhd_ctrl_request_first->payload_size)) {
			// DATA phase aborted by host
			USBH_HAL_InZLP();
			return;
		}
	}

#ifdef USB_HOST_HUB_SUPPORT
	// TODO
#else
	ep_ctrl_size = USBHS_HostGetSize(USBHS, bPipe);
#endif

	// Fill pipe
	USBHS_HostSetToken(USBHS, bPipe, USBHS_HSTPIPCFG_PTOKEN_OUT);
	//uhd_configure_pipe_token(0, USBHS_HSTPIPCFG_PTOKEN_OUT);
	//uhd_ack_out_ready(0);
	USBHS_HostAckPipeIntType(USBHS, bPipe, USBHS_HSTPIPICR_TXOUTIC);
	pEpData = (uint8_t *)((uint32_t *)USBHS_RAM_ADDR
						  + (EPT_VIRTUAL_SIZE * bPipe));
	memory_sync();

	while ((uhd_ctrl_nb_trans < uhd_ctrl_request_first->req.wLength)
		   && ep_ctrl_size && uhd_ctrl_request_first->payload_size) {
		*pEpData++ = *uhd_ctrl_request_first->payload++;
		memory_sync();
		uhd_ctrl_nb_trans++;
		ep_ctrl_size--;
		uhd_ctrl_request_first->payload_size--;
		memory_barrier();
	}

	//uhd_enable_out_ready_interrupt(0);
	USBHS_HostEnablePipeIntType(USBHS, bPipe, USBHS_HSTPIPIER_TXOUTES);
	USBHS_HostDisablePipeIntType(USBHS, bPipe,
								 (USBHS_HSTPIPIDR_FIFOCONC | USBHS_HSTPIPIDR_PFREEZEC ) );
	//uhd_unfreeze_pipe(0);
}

/**
 * \internal
 * \brief Starts the ZLP OUT phase on control endpoint
 */
static void USBH_HAL_OutZLP(void)
{
	uint8_t bPipe = 0;
	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_ZLP_OUT;
	USBHS_HostSetToken(USBHS, bPipe, USBHS_HSTPIPCFG_PTOKEN_OUT);
	//uhd_configure_pipe_token(0, USBHS_HSTPIPCFG_PTOKEN_OUT);
	USBHS_HostAckPipeIntType(USBHS, bPipe, USBHS_HSTPIPICR_TXOUTIC);
	//uhd_ack_out_ready(0);
	USBHS_HostEnablePipeIntType(USBHS, bPipe, USBHS_HSTPIPIER_TXOUTES);
	//uhd_enable_out_ready_interrupt(0);
	USBHS_HostDisablePipeIntType(USBHS, bPipe,
								 (USBHS_HSTPIPIDR_FIFOCONC | USBHS_HSTPIPIDR_PFREEZEC));
	//uhd_unfreeze_pipe(0);
}

/**
 * \internal
 * \brief Call the callback linked to control request
 * and start the next request from the queue.
*/
static void USBH_HAL_ControlReqEnd(USBH_XfrStatus_t status)
{
	uint8_t bPipe = 0;
	irqflags_t flags;
	uhd_callback_setup_end_t callback_end;
	struct uhd_ctrl_request_t *request_to_free;
	bool b_new_request;

	uhd_ctrl_request_timeout = 0;

	// Remove request from the control request list
	callback_end = uhd_ctrl_request_first->callback_end;
	request_to_free = uhd_ctrl_request_first;
	flags = cpu_irq_save();
	uhd_ctrl_request_first = uhd_ctrl_request_first->next_request;
	b_new_request = (uhd_ctrl_request_first != NULL);
	cpu_irq_restore(flags);
	free(request_to_free);

	// Call callback
	if (callback_end != NULL)
		callback_end(USBHS_HostGetAddr(USBHS, bPipe), status, uhd_ctrl_nb_trans);

	// If a setup request is pending and no started by previous callback
	if (b_new_request)
		USBH_HAL_PhaseControlSetup();

	if (uhd_b_suspend_requested) {
		// A suspend request has been delay after all setup request
		uhd_b_suspend_requested = false;
		USBH_HAL_Suspend();
	}
}

/**
 * \internal
 * \brief Translates the USBHS pipe error to UHD error
 *
 * \param pipe Pipe number to use
 *
 * \return UHD transfer error
 */
static USBH_XfrStatus_t USBH_HAL_GetPipeError(uint8_t pipe)
{
	uint32_t error = USBHS_HostGetErr(USBHS, pipe, (USBHS_HSTPIPERR_DATATGL |
									  USBHS_HSTPIPERR_TIMEOUT |
									  USBHS_HSTPIPERR_PID |
									  USBHS_HSTPIPERR_DATAPID));
	USBHS_HostClearErr(USBHS, pipe, 0xFF);

	switch (error) {
	case USBHS_HSTPIPERR_DATATGL:
		return UHD_TRANS_DT_MISMATCH;

	case USBHS_HSTPIPERR_TIMEOUT:
		return UHD_TRANS_NOTRESPONDING;

	case USBHS_HSTPIPERR_DATAPID:
	case USBHS_HSTPIPERR_PID:
	default:
		return UHD_TRANS_PIDFAILURE;
	}
}

/**
 * \internal
 * \brief Returns the pipe number matching a USB endpoint
 *
 * \param Addr USB address
 * \param bEndpoint  Endpoint number
 *
 * \return Pipe number
 */
static uint8_t USBH_HAL_GetPipe(uint8_t Addr, uint8_t bEndpoint)
{
	uint8_t pipe;

	// Search pipe
	for (pipe = 0; pipe < USBHS_EPT_NUM; pipe++) {
		if (!USBHS_IsHostPipeEnable(USBHS, pipe))
			continue;

		if (Addr != USBHS_HostGetAddr(USBHS, pipe))
			continue;

		if (bEndpoint != USBHS_GetPipeEpAddr(USBHS, pipe))
			continue;

		break;
	}

	return pipe;
}

#ifdef UHD_PIPE_FIFO_SUPPORTED
/**
 * \internal
 */
static void USBH_HAL_PipeInReceived(uint8_t pipe)
{
	USBH_PipeJob_t *pJob = &uhd_pipe_job[pipe - 1];
	uint32_t nb_data = 0, i;
	uint32_t nb_remain = pJob->buf_size - pJob->nb_trans;
	uint32_t pkt_size = USBHS_HostGetSize(USBHS, pipe);
	uint8_t *ptr_src = (uint8_t *)((uint32_t *)USBHS_RAM_ADDR
								   + (EPT_VIRTUAL_SIZE * pipe));
	uint8_t *ptr_dst = &pJob->buf[pJob->nb_trans];
	bool b_full = false, b_short = false;

	if (!pJob->busy) {
		return; // No job is running, then ignore it (system error)
	}

	// Read byte count
	nb_data = USBHS_HostGetPipeByteCount(USBHS, pipe);

	if (nb_data < pkt_size)
		b_short = true;

	// Copy data if there is
	if (nb_data > 0) {
		if (nb_data >= nb_remain) {
			nb_data = nb_remain;
			b_full = true;
		}

		// Modify job information
		pJob->nb_trans += nb_data;

		// Copy FIFO to buffer
		for (i = 0; i < nb_data; i++) {
			*ptr_dst++ = *ptr_src++;
			memory_sync();
		}
	}

	// Clear FIFO Status
	USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_FIFOCONC);

	// Finish job on error or short packet
	if (b_full || b_short) {
		USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_PFREEZES);
		USBHS_HostDisablePipeIntType(USBHS, pipe,
									 (USBHS_HSTPIPIDR_SHORTPACKETIEC | USBHS_HSTPIPIDR_RXINEC));
		//uhd_disable_short_packet_interrupt(pipe);
		//uhd_disable_in_received_interrupt(pipe);
		USBHS_HostPipeIntDisable(USBHS, pipe);
		//uhd_disable_pipe_interrupt(pipe);
		USBHS_HostDisableInReq(USBHS, pipe);
		//uhd_disable_continuous_in_mode(pipe);
		USBH_HAL_PipeXfrEnd(pipe, UHD_TRANS_NOERROR);
	}
}

/**
 * \internal
 */
static void USBH_HAL_PipeOutReady(uint8_t pipe)
{
	USBH_PipeJob_t *pJob = &uhd_pipe_job[pipe - 1];
	uint32_t pkt_size = USBHS_HostGetSize(USBHS, pipe);
	uint32_t nb_data = 0, i;
	uint32_t nb_remain;
	uint8_t *ptr_src;
	uint8_t *ptr_dst;

	if (!pJob->busy) {
		return; // No job is running, then ignore it (system error)
	}

	// Transfer data
	USBHS_HostAckPipeIntType(USBHS, pipe, USBHS_HSTPIPICR_TXOUTIC);

	nb_remain = pJob->buf_size - pJob->nb_trans;
	nb_data = min(nb_remain, pkt_size);

	// If not ZLP, fill FIFO
	if (nb_data) {
		// Fill FIFO
		ptr_dst = (uint8_t *)((uint32_t *)USBHS_RAM_ADDR
							  + (EPT_VIRTUAL_SIZE * pipe));
		ptr_src = &pJob->buf[pJob->nb_trans];
		// Modify job information
		pJob->nb_trans += nb_data;

		memory_sync();

		// Copy buffer to FIFO
		for (i = 0; i < nb_data; i++) {
			*ptr_dst++ = *ptr_src++;
			memory_sync();
		}
	}

	// Switch to next bank
	USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_FIFOCONC);

	// ZLP is cleared if last packet is short
	if (nb_data < pkt_size)
		pJob->b_shortpacket = false;

	// All transfer done, including ZLP, Finish Job
	if (pJob->nb_trans >= pJob->buf_size && !pJob->b_shortpacket) {
		// At least one bank there, wait to freeze pipe
		USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_TXOUTEC);
		//uhd_disable_out_ready_interrupt(pipe);
		USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_NBUSYBKES);
		return;
	}
}

#endif // #ifdef UHD_PIPE_FIFO_SUPPORTED

#ifdef UHD_PIPE_DMA_SUPPORTED
/**
 * \internal
 * \brief Computes and starts the next transfer on a pipe
 *
 * \param pipe  Pipe number
 */
static void USBH_HAL_PipeXfrCmplt(uint8_t pipe)
{
	uint32_t uhd_dma_ctrl = 0;
	USBH_PipeJob_t *pJob;
	uint32_t max_trans;
	uint32_t next_trans;
	irqflags_t flags;
	Usbhs  *pUdp  = USBHS;
	UsbhsHstdma *pUsbDma = &pUdp->USBHS_HSTDMA[pipe - 1];

	// Get job corresponding at endpoint
	pJob = &uhd_pipe_job[pipe - 1];

	if (!pJob->busy) {
		return; // No job is running, then ignore it (system error)
	}

	if (pJob->nb_trans != pJob->buf_size) {
		// Need to send or receive other data
		next_trans = pJob->buf_size - pJob->nb_trans;
		max_trans = UHD_PIPE_MAX_TRANS;

		if (USBHS_HostGetToken(USBHS, pipe) == USBHS_HSTPIPCFG_PTOKEN_IN) {
			// 256 is the maximum of IN requests via UPINRQ
			if ((256L * USBHS_HostGetSize(USBHS, pipe)) < UHD_PIPE_MAX_TRANS)
				max_trans = 256L * USBHS_HostGetSize(USBHS, pipe);
		}

		if (max_trans < next_trans) {
			// The USB hardware supports a maximum
			// transfer size of UHD_PIPE_MAX_TRANS Bytes
			next_trans = max_trans;
		}

		if (next_trans == UHD_PIPE_MAX_TRANS) {
			// Set 0 to transfer the maximum
			uhd_dma_ctrl = USBHS_HSTDMACONTROL_BUFF_LENGTH(0);
		} else
			uhd_dma_ctrl = USBHS_HSTDMACONTROL_BUFF_LENGTH(next_trans);

		if (USBHS_HSTPIPCFG_PTOKEN_OUT == USBHS_HostGetToken(USBHS, pipe)) {
			if (0 != (next_trans % USBHS_HostGetSize(USBHS, pipe))) {
				// Enable short packet option
				// else the DMA transfer is accepted
				// and interrupt DMA valid but nothing is sent.
				uhd_dma_ctrl |= USBHS_HSTDMACONTROL_END_B_EN;
				// No need to request another ZLP
				pJob->b_shortpacket = false;
			}
		} else {
			if ((USBEndpointDescriptor_ISOCHRONOUS != USBHS_HostGetPipeType(USBHS, pipe))
				|| ((int)next_trans <= USBHS_HostGetSize(USBHS, pipe))) {
				// Enable short packet reception
				uhd_dma_ctrl |= USBHS_HSTDMACONTROL_END_TR_IT
								| USBHS_HSTDMACONTROL_END_TR_EN;
			}
		}

		// Start USB DMA to fill or read fifo of the selected endpoint
		USBHS_SetHostDmaBuffAdd(pUsbDma, (uint32_t) &pJob->buf[pJob->nb_trans]);
		uhd_dma_ctrl |= USBHS_HSTDMACONTROL_END_BUFFIT |
						USBHS_HSTDMACONTROL_CHANN_ENB;

		// Disable IRQs to have a short sequence
		// between read of EOT_STA and DMA enable
		flags = cpu_irq_save();

		if (!(USBHS_GetHostPipeDmaStatus(pUsbDma)
			  & USBHS_HSTDMASTATUS_END_TR_ST)) {
			if (USBHS_HSTPIPCFG_PTOKEN_IN == USBHS_HostGetToken(USBHS, pipe)) {
				uint32_t pipe_size = USBHS_HostGetSize(USBHS, pipe);
				USBHS_HostInReq(USBHS, pipe, (next_trans + pipe_size - 1) / pipe_size);
			}

			if (!pJob->b_periodic_start) {
				USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_NBUSYBKEC);
				USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_PFREEZEC);
			} else {
				// Last bank not sent, just start
				if (USBHS_IsHostPipeIntTypeEnable(USBHS, pipe, USBHS_HSTPIPIMR_NBUSYBKE))
					USBHS_HostDisablePipeIntType(USBHS, pipe,
												 (USBHS_HSTPIPIDR_NBUSYBKEC | USBHS_HSTPIPIDR_PFREEZEC));
				else {
					// Wait SOF to start
				}
			}

			SCB_CleanInvalidateDCache();
			USBHS_HostConfigureDma(pUsbDma, uhd_dma_ctrl);
			pJob->nb_trans += next_trans;
			cpu_irq_restore(flags);
			return;
		}

		cpu_irq_restore(flags);
		// Here a ZLP has been received
		// and the DMA transfer must be not started.
		// It is the end of transfer
		pJob->buf_size = pJob->nb_trans;
	}

	if (USBHS_HSTPIPCFG_PTOKEN_OUT == USBHS_HostGetToken(USBHS, pipe)) {
		if (pJob->b_shortpacket) {
			// Need to send a ZLP (No possible with USB DMA)
			// enable interrupt to wait a free bank to sent ZLP
			USBHS_HostAckPipeIntType(USBHS, pipe, USBHS_HSTPIPICR_TXOUTIC);

			if (USBHS_HostGetIntTypeStatus(USBHS, pipe, USBHS_HSTPIPISR_RWALL)) {
				// Force interrupt in case of pipe already free
				USBHS_HostSetPipeIntType(USBHS, pipe, USBHS_HSTPIPIFR_TXOUTIS);
			}

			USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_TXOUTES);
			return;
		}
	}

	// Call callback to signal end of transfer
	USBH_HAL_PipeXfrEnd(pipe, UHD_TRANS_NOERROR);
}

/**
 * \internal
 * \brief Manages the pipe DMA interrupt
 *
 * \param pipe  Pipe number
 */
static void USBH_HAL_PipeDmaInterrupt(uint8_t pipe)
{
	USBH_PipeJob_t *pJob;
	uint32_t nb_remaining;
	Usbhs  *pUdp = USBHS;
	UsbhsHstdma *pUsbDma = &pUdp->USBHS_HSTDMA[pipe - 1];

	SCB_CleanInvalidateDCache();

	if (USBHS_GetHostPipeDmaStatus(pUsbDma) & USBHS_HSTDMASTATUS_CHANN_ENB) {
		return; // Ignore EOT_STA interrupt
	}

	// Save number of data no transfered
	nb_remaining = ( (USBHS_GetHostPipeDmaStatus(pUsbDma) &
					  USBHS_HSTDMASTATUS_BUFF_COUNT_Msk) >> USBHS_HSTDMASTATUS_BUFF_COUNT_Pos);
	memory_sync();

	if (nb_remaining) {
		// Get job corresponding at endpoint
		pJob = &uhd_pipe_job[pipe - 1];

		// Transfer no complete (short packet or ZLP) then:
		// Update number of transfered data
		pJob->nb_trans -= nb_remaining;

		// Set transfer complete to stop the transfer
		pJob->buf_size = pJob->nb_trans;
	}

	if (USBHS_HSTPIPCFG_PTOKEN_OUT == USBHS_HostGetToken(USBHS, pipe)) {
		// Wait that all banks are free to freeze clock of OUT endpoint
		// and call callback
		USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_NBUSYBKES);

		// For ISO out, start another DMA transfer since no ACK needed
		if (USBHS_HostGetPipeType(USBHS, pipe) == USBHS_HSTPIPCFG_PTYPE_ISO)
			USBH_HAL_PipeXfrEnd(pipe, UHD_TRANS_NOERROR);
	} else {
		if (!USBHS_IsHostPipeIntTypeEnable(USBHS, pipe, USBHS_HSTPIPIMR_PFREEZE)) {
			// Pipe is not freeze in case of :
			// - incomplete transfer when the request number INRQ is not complete.
			// - low USB speed and with a high CPU frequency,
			// a ACK from host can be always running on USB line.

			if (nb_remaining) {
				// Freeze pipe in case of incomplete transfer
				USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_PFREEZES);
			} else {
				// Wait freeze in case of ASK on going
				while (!USBHS_IsHostPipeIntTypeEnable(USBHS, pipe, USBHS_HSTPIPIMR_PFREEZE)) {
				}
			}
		}

		USBH_HAL_PipeXfrCmplt(pipe);
	}
}
#endif // ifdef UHD_PIPE_DMA_SUPPORTED

/**
 * \internal
 * \brief Manages the following pipe interrupts:
 * - Real end of USB transfers (bank empty)
 * - One bank is free to send a OUT ZLP
 * - Stall received
 * - Error during transfer
 *
 * \param pipe  Pipe number
 */
static void USBH_HAL_PipeInterrupt(uint8_t pipe)
{
	uint32_t status = USBHS_HostGetIntTypeStatus(USBHS, pipe, 0xFFFFFFFF);
	uint32_t statusInt = status & USBHS_IsHostPipeIntTypeEnable(USBHS, pipe,
						 0xFFFFFFFF);
#ifdef UHD_PIPE_FIFO_SUPPORTED

	// for none DMA endpoints
	if (!Is_uhd_pipe_dma_supported(pipe)) {
		// SHORTPACKETI: Short received
		if (statusInt & USBHS_HSTPIPISR_SHORTPACKETI) {
			USBHS_HostAckPipeIntType(USBHS, pipe,
									 USBHS_HSTPIPICR_SHORTPACKETIC); //uhd_ack_short_packet(pipe);
			USBH_HAL_PipeInReceived(pipe);
			return;
		}

		// RXIN: Full packet received
		if (statusInt & USBHS_HSTPIPIMR_RXINE ) {
			USBHS_HostAckPipeIntType(USBHS, pipe,
									 USBHS_HSTPIPICR_RXINIC);//uhd_ack_in_received(pipe);
			USBH_HAL_PipeInReceived(pipe);
			return;
		}

		// TXOUT: packet sent
		if (statusInt & USBHS_HSTPIPISR_TXOUTI) {
			USBH_HAL_PipeOutReady(pipe);
			return;
		}

		// OUT: all banks sent
		if (USBHS_IsHostPipeIntTypeEnable(USBHS, pipe, USBHS_HSTPIPIMR_NBUSYBKE)
			&& (0 == USBHS_HostGetNumOfBusyBank(USBHS, pipe))) {
			USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_PFREEZES);
			USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_NBUSYBKEC );
			//uhd_disable_bank_interrupt(pipe);
			USBHS_HostPipeIntDisable(USBHS, pipe);
			//uhd_disable_pipe_interrupt(pipe);
			USBHS_HostEnableAutoSw(USBHS, pipe);
			//uhd_enable_pipe_bank_autoswitch (pipe);
			USBH_HAL_PipeXfrEnd(pipe, UHD_TRANS_NOERROR);
			return;
		}

		if (status & USBHS_HSTPIPISR_RXSTALLDI) {
			USBHS_HostAckPipeIntType(USBHS, pipe,
									 USBHS_HSTPIPICR_RXSTALLDIC); //uhd_ack_stall(pipe);
			USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_RSTDTS);
			//uhd_reset_data_toggle(pipe);
			USBH_HAL_PipeAbort(pipe, UHD_TRANS_STALL);
			return;
		}

		if (status & USBHS_HSTPIPISR_PERRI) {
			// Get and ack error
			USBH_HAL_PipeAbort(pipe, USBH_HAL_GetPipeError(pipe));
			return;
		}

		assert(false); // Error system
		return;
	}

#endif // UDD_EP_FIFO_SUPPORTED

#ifdef UHD_PIPE_DMA_SUPPORTED

	// for DMA endpoints
	if (USBHS_IsHostPipeIntTypeEnable(USBHS, pipe, USBHS_HSTPIPIMR_NBUSYBKE)
		&& (0 == USBHS_HostGetNumOfBusyBank(USBHS, pipe))) {
		USBHS_HostEnablePipeIntType(USBHS, pipe,
									USBHS_HSTPIPIER_PFREEZES);//uhd_freeze_pipe(pipe);
		USBHS_HostDisablePipeIntType(USBHS, pipe,
									 USBHS_HSTPIPIDR_NBUSYBKEC);//uhd_disable_bank_interrupt(pipe);

		// For ISO, no ACK, finished when DMA done
		if (!(USBHS_HostGetPipeType(USBHS, pipe) == USBHS_HSTPIPCFG_PTYPE_ISO))
			USBH_HAL_PipeXfrEnd(pipe, UHD_TRANS_NOERROR);

		return;
	}

	if (USBHS_HSTPIPIMR_TXOUTE & statusInt) {
		USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_TXOUTEC);
		//uhd_disable_out_ready_interrupt(pipe);
		// One bank is free then send a ZLP
		USBHS_HostAckPipeIntType(USBHS, pipe,
								 USBHS_HSTPIPICR_TXOUTIC); //uhd_ack_out_ready(pipe);
		USBHS_HostDisablePipeIntType(USBHS, pipe,
									 (USBHS_HSTPIPIDR_FIFOCONC | USBHS_HSTPIPIDR_PFREEZEC));
		//USBHS_HostDisablePipeIntType(USBHS, pipe, );
		USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_NBUSYBKES);
		return;
	}

	if (status & USBHS_HSTPIPISR_RXSTALLDI) {
		USBHS_HostAckPipeIntType(USBHS, pipe,
								 USBHS_HSTPIPICR_RXSTALLDIC);//uhd_ack_stall(pipe);
		USBHS_HostEnablePipeIntType(USBHS, pipe, USBHS_HSTPIPIER_RSTDTS);
		USBH_HAL_PipeAbort(pipe, UHD_TRANS_STALL);
		return;
	}

	if (status & USBHS_HSTPIPISR_PERRI) {
		// Get and ack error
		USBH_HAL_PipeAbort(pipe, USBH_HAL_GetPipeError(pipe));
		return;
	}

	assert(false); // Error system
#endif // UHD_PIPE_DMA_SUPPORTED
}

/**
 * \internal
 * \brief Aborts the on going transfer on a pipe
 *
 * \param pipe Pipe number
 * \param status Reason of abort
 */
static void USBH_HAL_PipeAbort(uint8_t pipe, USBH_XfrStatus_t status)
{
	Usbhs *pUdp = USBHS;
	UsbhsHstdma *pUsbDma = &pUdp->USBHS_HSTDMA[pipe - 1];
	// Stop transfer
	USBHS_HostPipeReset(USBHS, pipe);

	// Autoswitch bank and interrupts has been reseted, then re-enable it
	USBHS_HostEnableAutoSw(USBHS, pipe);
	USBHS_HostEnablePipeIntType(USBHS, pipe,
								(USBHS_HSTPIPIER_RXSTALLDES | USBHS_HSTPIPIER_PERRES) );
	//uhd_enable_pipe_error_interrupt(pipe);
	USBHS_HostDisablePipeIntType(USBHS, pipe, USBHS_HSTPIPIDR_TXOUTEC);
#ifdef UHD_PIPE_DMA_SUPPORTED

	if (Is_uhd_pipe_dma_supported(pipe))
		USBHS_HostConfigureDma(pUsbDma, 0);

#endif
	USBH_HAL_PipeXfrEnd(pipe, status);
}

/**
 * \internal
 * \brief Call the callback linked to the end of pipe transfer
 *
 * \param pipe Pipe number
 * \param status Status of the transfer
 */
static void USBH_HAL_PipeXfrEnd(uint8_t pipe, USBH_XfrStatus_t status)
{
	uint32_t dev_addr , dev_ep;
	USBH_PipeJob_t *pJob;

	// Get job corresponding at endpoint
	pJob = &uhd_pipe_job[pipe - 1];

	if (pJob->busy == false) {
		return; // No job running
	}

	pJob->busy = false;

	if (NULL == pJob->call_end) {
		return; // No callback linked to job
	}

	dev_addr = USBHS_HostGetAddr(USBHS, pipe);
	dev_ep = USBHS_GetPipeEpAddr(USBHS, pipe);
	pJob->call_end(dev_addr, dev_ep, status, pJob->nb_trans);
}

//@}
