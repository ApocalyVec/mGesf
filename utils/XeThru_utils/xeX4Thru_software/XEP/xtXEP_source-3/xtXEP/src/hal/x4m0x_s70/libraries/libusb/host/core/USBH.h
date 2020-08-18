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
 * \brief Interface of the USB Host Controller (UHC)
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//#include "conf_usb_host.h"

#ifndef _UHC_H_
#define _UHC_H_

#include "USBHDriver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup usb_host_group
 * \defgroup USBH_group USB host Controller (UHC)
 *
 * The UHC provides a high-level abstraction of the usb host.
 * You can use these functions to control the main host state
 * (start/suspend/resume/...).
 *
 * \section USB_HOST_CONF USB host user configuration
 * The following USB host configuration must be included in the conf_usb_host.h
 * file of the application.
 *
 * USB_HOST_UHI (List of UHI APIs)<br>
 * Define the list of UHI supported by USB host. (Ex.: UHI_MSC,UHI_HID_MOUSE).
 *
 * USB_HOST_POWER_MAX (mA)<br>
 * Maximum current allowed on Vbus.
 *
 * USB_HOST_HS_SUPPORT (Only defined)<br>
 * Authorize the USB host to run in High Speed.
 *
 * USB_HOST_HUB_SUPPORT (Only defined)<br>
 * Authorize the USB HUB support.
 *
 * \section USB_HOST_CALLBACK USB host user callback
 * The following optional USB host callback can be defined in the conf_usb_host.h
 * file of the application.
 *
 * void UHC_MODE_CHANGE(bool b_host_mode)<br>
 * To notify that the USB mode are switched automatically.
 * This is possible only when ID pin is available.
 *
 * void UHC_VBUS_CHANGE(bool b_present)<br>
 * To notify that the Vbus level has changed
 * (Available only in USB hardware with Vbus monitoring).
 *
 * void UHC_VBUS_ERROR(void)<br>
 * To notify that a Vbus error has occurred
 * (Available only in USB hardware with Vbus monitoring).
 *
 * void UHC_CONNECTION_EVENT(USBH_device_t* dev,bool b_present)<br>
 * To notify that a device has been connected or disconnected.
 *
 * void UHC_WAKEUP_EVENT(void)<br>
 * Called when a USB device or the host have wake up the USB line.
 *
 * void UHC_SOF_EVENT(void)<br>
 * Called for each received SOF each 1 ms.
 * Available in High and Full speed mode.
 *
 * uint8_t UHC_DEVICE_CONF(USBH_device_t* dev)<br>
 * Called when a USB device configuration must be chosen.
 * Thus, the application can choose either a configuration number
 * for this device or a configuration number 0 to reject it.
 * If callback not defined the configuration 1 is chosen.
 *
 * void UHC_ENUM_EVENT(USBH_device_t* dev, uint8_t b_status)<br>
 * Called when a USB device enumeration is completed or fail.
 *
 * @{
 */

// Descriptor storage in internal RAM
#if (defined UHC_DATA_USE_HRAM_SUPPORT)
#  if defined(__GNUC__)
#    define UHC_DATA(x) COMPILER_WORD_ALIGNED __attribute__((__section__(".data_hram0")))
#    define UHC_BSS(x)  COMPILER_ALIGNED(x)   __attribute__((__section__(".bss_hram0")))
#  elif defined(__ICCAVR32__)
#    define UHC_DATA(x) COMPILER_ALIGNED(x)   __data32
#    define UHC_BSS(x)  COMPILER_ALIGNED(x)   __data32
#  endif
#else
#  define UHC_DATA(x) COMPILER_ALIGNED(x)
#  define UHC_BSS(x)  COMPILER_ALIGNED(x)
#endif

/**
 * \brief Structure to store device information
 */
typedef struct {
	//! USB device descriptor
	USBDeviceDescriptor dev_desc;

	// USB address
	uint8_t address;

	// USB speed
	USBH_Speed_t speed;

	//! USB current configuration descriptor
	USBConfigurationDescriptor *conf_desc;

#ifdef USB_HOST_LPM_SUPPORT
	USB_DeviceLPMDescriptor *lpm_desc;
#endif

#ifdef USB_HOST_HUB_SUPPORT
	USBH_device_t *prev;
	USBH_device_t *next;
	USBH_device_t *hub;
	// Power consumption if device or devices connected to a HUB downstream port are NUB powered
	uint16_t power;
	uint8_t hub_port;
#endif
} USBH_device_t;

/**
 * \brief Enumeration status
 * Used in UHC_ENUM_EVENT() callback
 * when a USB device enumeration is completed.
 */
typedef enum {
	//! Device is enumerated.
	//! The supported USB device interfaces has been enabled.
	UHC_ENUM_SUCCESS = 0,

	//! None of the interfaces are supported by the UHIs.
	UHC_ENUM_UNSUPPORTED,

	//! Device power is not supported.
	UHC_ENUM_OVERCURRENT,

	//! A problem occurred during USB enumeration.
	UHC_ENUM_FAIL,

	//! USB hardware can not support it. Not enough free pipes.
	UHC_ENUM_HARDWARE_LIMIT,

	//! USB software can not support it. Implementation limit.
	UHC_ENUM_SOFTWARE_LIMIT,

	//! USB software can not support it. Not enough memory.
	UHC_ENUM_MEMORY_LIMIT,

	//! The device has been disconnected during USB enumeration.
	UHC_ENUM_DISCONNECT,
} USBH_enum_status_t;

/**
 * \name Functions to control the USB host stack
 *
 * @{
 */

/*! \brief Starts the host mode
 */
void USBH_start(void);

/*! \brief Stops the host mode
 *
 * \param b_id_stop  Stop USB ID pin management, if true.
 */
void USBH_stop(bool b_id_stop);

/**
 * \brief Suspends a USB line
 *
 * \param b_remotewakeup Authorize the remote wakeup features, if true
 */
void USBH_suspend(bool b_remotewakeup);

/**
 * \brief Test if the suspend state is enabled on the USB line.
 * \return USB line in SUSPEND state or device not connected, if true
 */
bool USBH_is_suspend(void);

/**
 * \brief Resumes the USB line
 */
void USBH_resume(void);

#ifdef USB_HOST_LPM_SUPPORT
/**
 * \brief Suspends a USB line through LPM feature
 *
 * \param b_remotewakeup Authorize the remote wakeup features, if true
 * \param besl Best effort service latency value
 *
 * \return flase if the LPM is not supported by USB Device
 */
bool USBH_suspend_lpm(bool b_remotewakeup, uint8_t besl);
#endif // USB_HOST_LPM_SUPPORT

//@}


/**
 * \name User functions to manage a device
 *
 * @{
 */

/**
 * \brief Returns the number of connected devices
 *
 * \return Number of device connected on USB tree
 */
uint8_t USBH_get_device_number(void);

/**
 * \brief Gets the USB string manufacturer from a USB device
 *
 * This function waits the end of setup requests
 * and the timing can be long (3ms to 15s).
 * Thus, do not call it in an interrupt routine.
 * This function allocates a buffer which must be free by user application.
 *
 * \param dev    Device to request
 *
 * \return Pointer on unicode string, or NULL if function fails.
 */
char *USBH_dev_get_string_manufacturer(USBH_device_t *dev);


/**
 * \brief Gets the USB string product from a USB device
 *
 * This function waits the end of setup requests
 * and the timing can be long (3ms to 15s).
 * Thus, do not call it in an interrupt routine.
 * This function allocates a buffer which must be free by user application.
 *
 * \param dev    Device to request
 *
 * \return Pointer on unicode string, or NULL if function fails.
 */
char *USBH_dev_get_string_product(USBH_device_t *dev);


/**
 * \brief Gets the USB string serial from a USB device
 *
 * This function waits the end of setup requests
 * and the timing can be long (3ms to 15s).
 * Thus, do not call it in an interrupt routine.
 * This function allocates a buffer which must be free by user application.
 *
 * \param dev    Device to request
 *
 * \return Pointer on unicode string, or NULL if function fails.
 */
char *USBH_dev_get_string_serial(USBH_device_t *dev);

/**
 * \brief Gets a USB string from a USB device
 *
 * This function waits the end of setup requests
 * and the timing can be long (3ms to 15s).
 * Thus, do not call it in an interrupt routine.
 * This function allocates a buffer which must be free by user application.
 *
 * \param dev    Device to request
 * \param str_id String ID requested
 *
 * \return Pointer on unicode string, or NULL if function fails.
 */
char *USBH_dev_get_string(USBH_device_t *dev, uint8_t str_id);

/**
 * \brief Gets the maximum consumption of a device (mA)
 *
 * \param dev    Device to request
 *
 * \return Maximum consumption of the device (mA)
 */
uint16_t USBH_dev_get_power(USBH_device_t *dev);

/**
 * \brief Returns the current device speed
 *
 * \param dev    Device to request
 *
 * \return Device speed
 */
USBH_Speed_t USBH_dev_get_speed(USBH_device_t *dev);

/**
 * \brief Tests if the device supports the USB high speed
 * This function can wait the end of a setup request
 * and the timing can be long (1ms to 5s).
 * Thus, do not call it in an interrupt routine.
 *
 * \param dev    Device to request
 *
 * \return True, if high speed is supported
 */
bool USBH_dev_is_high_speed_support(USBH_device_t *dev);

//@}

//@}

/**
 * \ingroup usb_group
 * \defgroup usb_host_group USB Stack Host
 *
 * This module includes USB Stack Host implementation.
 * The stack is divided in three parts:
 * - USB Host Controller (UHC) provides USB chapter 9 compliance
 * - USB Host Interface (UHI) provides USB Class compliance
 * - USB Host Driver (UHD) provides USB Driver for each Atmel MCU

 * Many USB Host applications can be implemented on Atmel MCU.
 * Atmel provides the application note AVR4950 about USB Host Stack general
 * information.
 *
 * @{
 */

//! @}

#ifdef __cplusplus
}
#endif

/**
 * \ingroup USBH_group
 * \defgroup USBH_basic_use_case_setup_prereq USB Host Controller (UHC) - Prerequisites
 * Common prerequisites for all USB hosts.
 *
 * This module is based on USB host stack full interrupt driven and supporting
 * \ref sleepmgr_group sleepmgr. For AVR and SAM3/4 devices the
 * \ref clk_group clock services is supported. For SAMD21 devices the
 * \ref asfdoc_sam0_system_clock_group clock driver is supported.
 *
 * The following procedure must be executed to setup the project correctly:
 * - Specify the clock configuration:
 *   - UC3 and SAM3/4 devices without USB high speed support need 48MHz clock input.\n
 *     You must use a PLL and an external OSC.
 *   - UC3 and SAM3/4 devices with USB high speed support need 12MHz clock input.\n
 *     You must use an external OSC.
 *   - UC3 devices with USBC hardware need CPU frequency higher than 25MHz.
 *   - SAMD21 devices without USB high speed support need 48MHz clock input.\n
 *     You must use a DFLL and an external OSC.
 * - In conf_board.h, the define CONF_BOARD_USB_PORT must be added to enable USB lines.
 * (Not mandatory for all boards)
 * - Enable interrupts
 * - Initialize the clock service
 *
 * The usage of \ref sleepmgr_group sleepmgr service is optional, but recommended to reduce power
 * consumption:
 * - Initialize the sleep manager service
 * - Activate sleep mode when the application is in IDLE state
 */

/**
 * \ingroup USBH_group
 * \defgroup USBH_basic_use_case_setup_code USB Host Controller (UHC) - Example code
 * Common example code for all USB hosts.
 *
 * Content of conf_usb_host.h:
 * \code
    #define USB_HOST_POWER_MAX  500
\endcode
 *
 * Add to application C-file:
 * \code
    void usb_init(void)
    {
    USBH_start();
    }
\endcode
 */

/**
 * \ingroup USBH_group
 * \defgroup USBH_basic_use_case_setup_flow USB Device Controller (UHC) - Workflow
 * Common workflow for all USB devices.
 *
 * -# Ensure that conf_usb_host.h is available and contains the following configuration
 * which is the main USB device configuration:
 *   - \code // Maximum current allowed on Vbus (mA) which depends of 5V generator
    #define USB_HOST_POWER_MAX  500 // (500mA) \endcode
 * -# Call the USB host stack start function to enable USB Host stack:
 *   - \code USBH_start(); \endcode
 */

/**
 * \page USBH_use_case_1 Enable USB high speed support
 *
 * In this use case, the USB host is used to support USB high speed.
 *
 * \section USBH_use_case_1_setup Setup steps
 *
 * Prior to implement this use case, be sure to have already
 * apply the UDI module "basic use case".
 *
 * \section USBH_use_case_1_usage Usage steps
 *
 * \subsection USBH_use_case_1_usage_code Example code
 * Content of conf_usb_host.h:
 * \code
    #define USB_HOST_HS_SUPPORT
\endcode
 *
 * \subsection USBH_use_case_1_usage_flow Workflow
 * -# Ensure that conf_usb_host.h is available and contains the following parameters
 * required for a USB device high speed (480Mbit/s):
 *   - \code #define  USB_HOST_HS_SUPPORT \endcode
 */

/**
 * \page USBH_use_case_2 Multiple classes support
 *
 * In this use case, the USB host is used to support several USB classes.
 *
 * \section USBH_use_case_2_setup Setup steps
 *
 * Prior to implement this use case, be sure to have already
 * apply the UDI module "basic use case".
 *
 * \section USBH_use_case_2_usage Usage steps
 *
 * \subsection USBH_use_case_2_usage_code Example code
 * Content of conf_usb_host.h:
 * \code
    #define USB_HOST_UHI   UHI_HID_MOUSE, UHI_MSC, UHI_CDC
\endcode
 *
 * \subsection USBH_use_case_2_usage_flow Workflow
 * -# Ensure that conf_usb_host.h is available and contains the following parameters:
 *   - \code #define USB_HOST_UHI   UHI_HID_MOUSE, UHI_MSC, UHI_CDC \endcode
 *     \note USB_HOST_UHI defines the list of UHI supported by USB host.
 *     Here, you must add all classes that you want to support.
 */

/**
 * \page USBH_use_case_3 Dual roles support
 *
 * In this use case, the USB host and USB device are enabled, it is the dual role.
 *
 * Note: On the Atmel boards, the switch of USB role is managed automatically by the
 * USB stack thank to a USB OTG connector and its USB ID pin.
 * For a dual role management without OTG connector, please refer to
 * "AVR4950 section 6.1 Dual roles".
 *
 * \section USBH_use_case_3_setup Setup steps
 *
 * Prior to implement this use case, be sure to have already
 * apply the UDI module "basic use case".
 *
 * \section USBH_use_case_3_usage Usage steps
 *
 * \subsection USBH_use_case_3_usage_code Example code
 * Content of conf_usb_host.h:
 * \code
    #define UHC_MODE_CHANGE(b_host_mode)   my_callback_mode_change(b_host_mode)
    extern void my_callback_mode_change(bool b_host_mode);
\endcode
 *
 * Add to application C-file:
 * \code
     void usb_init(void)
     {
       //udc_start();
       USBH_start();
     }

     bool my_host_mode;
     void my_callback_mode_change(bool b_host_mode)
     {
       my_host_mode = b_host_mode;
     }

     void my_usb_task(void)
     {
       if (my_host_mode) {
         // CALL USB Host task
       } else {
         // CALL USB Device task
       }
     }
\endcode
 *
 * \subsection USBH_use_case_3_usage_flow Workflow
 * -# In case of USB dual roles (Device and Host), the USB stack must be enabled
 * by USBH_start() and the udc_start() must not be called.
 *   - \code //udc_start();
     USBH_start(); \endcode
 * -# In dual role, to known the current USB mode, the callback to notify the
 * mode changes can be used.
 *   - Ensure that conf_usb_host.h contains the following parameters.
 * \code
    #define UHC_MODE_CHANGE(b_host_mode)   my_callback_mode_change(b_host_mode)
    extern void my_callback_mode_change(bool b_host_mode);
\endcode
 *   - Ensure that application contains the following code:
 * \code
    bool my_host_mode;
    void my_callback_mode_change(bool b_host_mode)
    {
      my_host_mode = b_host_mode;
    }

    void my_usb_task(void)
    {
      if (my_host_mode) {
        // CALL USB Host task
      } else {
        // CALL USB Device task
      }
    }
\endcode
 */

#endif // _UHC_H_
