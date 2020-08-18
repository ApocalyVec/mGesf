/**
 * \file
 *
 * \brief Interface of the USB Host Controller (UHC)
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \if ASF_MANUAL
 *
 * \defgroup asfdoc_uhc_group USB Host Controller (UHC)
 *
 * The UHC provides a high-level abstraction of the usb host.
 * You can use these functions to control the main host state
 * (start/suspend/resume/...).
 *
 * All USB Host Interface (UHI) in USB Host Stack is based on UHC to support
 * USB enumeration.
 *
 * For more details for Atmel Software Framework (ASF) USB Host Stack,
 * refer to following application note:
 * - <a href="http://www.atmel.com/dyn/resources/prod_documents/doc8486.pdf">
 *   AVR4950: ASF - USB Host Stack</a>
 *
 * This documentation describes common USB Host usage based on UHC, as follow:
 * - \ref asfdoc_uhc_api_overview
 * - \ref asfdoc_uhc_basic_use_case_setup
 * - \ref asfdoc_uhc_use_cases
 *
 * \section asfdoc_uhc_api_overview API Overview
 * @{
 */

/**
 * \brief Structure to store device information
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved only for module-internal use.
 */
typedef struct{
	/** USB device descriptor */
	usb_dev_desc_t dev_desc;

	/** USB address */
	uint8_t address;

	/** USB speed */
	uhd_speed_t speed;

	/** USB current configuration descriptor */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
	usb_conf_desc_t *conf_desc;

#ifdef USB_HOST_LPM_SUPPORT
	usb_dev_lpm_desc_t *lpm_desc;
#endif

#ifdef USB_HOST_HUB_SUPPORT
	uhc_device_t *prev;
	uhc_device_t *next;
	uhc_device_t *hub;
	// Power consumption if device or devices connected to a HUB downstream port are NUB powered
	uint16_t power;
	uint8_t hub_port;
#endif
} uhc_device_t;

/**
 * \brief Enumeration status
 * Used in UHC_ENUM_EVENT() callback
 * when a USB device enumeration is completed.
 */
typedef enum {
	/** Device is enumerated.
	 *  The supported USB device interfaces has been enabled. */
	UHC_ENUM_SUCCESS = 0,

	/** None of the interfaces are supported by the UHIs. */
	UHC_ENUM_UNSUPPORTED,

	/** Device power is not supported. */
	UHC_ENUM_OVERCURRENT,

	/** A problem occurred during USB enumeration. */
	UHC_ENUM_FAIL,

	/** USB hardware can not support it. Not enough free pipes. */
	UHC_ENUM_HARDWARE_LIMIT,

	/** USB software can not support it. Implementation limit. */
	UHC_ENUM_SOFTWARE_LIMIT,

	/** USB software can not support it. Not enough memory. */
	UHC_ENUM_MEMORY_LIMIT,

	/** The device has been disconnected during USB enumeration. */
	UHC_ENUM_DISCONNECT,
} uhc_enum_status_t;

/**
 * \name Functions to Control the USB Host Stack
 *
 * @{
 */

/** \brief Starts the host mode.
 */
void uhc_start(void);

/** \brief Stops the host mode.
 *
 * \param[in] b_id_stop  Stop USB ID pin management, if true.
 */
void uhc_stop(bool b_id_stop);

/**
 * \brief Suspends a USB line.
 *
 * \param[in] b_remotewakeup Authorize the remote wakeup features, if true.
 */
void uhc_suspend(bool b_remotewakeup);

/**
 * \brief Test if the suspend state is enabled on the USB line.
 * \return USB line in SUSPEND state or device not connected, if true.
 */
bool uhc_is_suspend(void);

/**
 * \brief Resumes the USB line.
 */
void uhc_resume(void);

/**
 * \brief Suspends a USB line through LPM feature(SAM D21).
 *
 * \param[in] b_remotewakeup Authorize the remote wakeup features, if true.
 * \param[in] hird Host Initiated Resume Duration.
 *
 * \return False if the LPM is not supported by USB Device.
 */
bool uhc_suspend_lpm(bool b_remotewakeup, uint8_t hird);

/**@}*/


/**
 * \name User Functions to Manage a Device
 *
 * @{
 */

/**
 * \brief Returns the number of connected devices.
 *
 * \return Number of device connected on USB tree.
 */
uint8_t uhc_get_device_number(void);

/**
 * \brief Gets the USB string manufacturer from a USB device.
 *
 * This function waits the end of setup requests
 * and the timing can be long (3ms to 15s).
 * Thus, do not call it in an interrupt routine.
 * This function allocates a buffer which must be free by user application.
 *
 * \param[in] dev    Device to request.
 *
 * \return Pointer on unicode string, or NULL if function fails.
 */
char* uhc_dev_get_string_manufacturer(uhc_device_t* dev);


/**
 * \brief Gets the USB string product from a USB device.
 *
 * This function waits the end of setup requests
 * and the timing can be long (3ms to 15s).
 * Thus, do not call it in an interrupt routine.
 * This function allocates a buffer which must be free by user application.
 *
 * \param[in] dev    Device to request.
 *
 * \return Pointer on unicode string, or NULL if function fails.
 */
char* uhc_dev_get_string_product(uhc_device_t* dev);


/**
 * \brief Gets the USB string serial from a USB device.
 *
 * This function waits the end of setup requests
 * and the timing can be long (3ms to 15s).
 * Thus, do not call it in an interrupt routine.
 * This function allocates a buffer which must be free by user application.
 *
 * \param[in] dev    Device to request.
 *
 * \return Pointer on unicode string, or NULL if function fails.
 */
char* uhc_dev_get_string_serial(uhc_device_t* dev);

/**
 * \brief Gets a USB string from a USB device.
 *
 * This function waits the end of setup requests
 * and the timing can be long (3ms to 15s).
 * Thus, do not call it in an interrupt routine.
 * This function allocates a buffer which must be free by user application.
 *
 * \param[in] dev    Device to request.
 * \param[in] str_id String ID requested.
 *
 * \return Pointer on unicode string, or NULL if function fails.
 */
char* uhc_dev_get_string(uhc_device_t* dev, uint8_t str_id);

/**
 * \brief Gets the maximum consumption of a device (mA).
 *
 * \param[in] dev    Device to request.
 *
 * \return Maximum consumption of the device (mA).
 */
uint16_t uhc_dev_get_power(uhc_device_t* dev);

/**
 * \brief Returns the current device speed.
 *
 * \param[in] dev    Device to request.
 *
 * \return Device speed.
 */
uhd_speed_t uhc_dev_get_speed(uhc_device_t* dev);

/**
 * \brief Tests if the device supports the USB high speed.
 * This function can wait the end of a setup request
 * and the timing can be long (1ms to 5s).
 * Thus, do not call it in an interrupt routine.
 *
 * \param[in] dev    Device to request.
 *
 * \return True, if high speed is supported.
 */
bool uhc_dev_is_high_speed_support(uhc_device_t* dev);
/**@}*/

/**
 * @}
 * \endif
 */

/**
 * \page asfdoc_uhc_basic_use_case_setup USB Host Basic Setup
 *
 * \section USB_HOST_CONF USB Host User Configuration
 * The following USB host configuration must be included in the conf_usb_host.h
 * file of the application:
 *
 * \b 1. USB_HOST_UHI (List of UHI APIs).
 *
 * Define the list of UHI supported by USB host. (E.g.: UHI_MSC, UHI_HID_MOUSE).
 *
 * \b 2. USB_HOST_POWER_MAX (mA).
 *
 * Maximum current allowed on Vbus.
 *
 * \b 3. USB_HOST_HS_SUPPORT (Only defined).
 *
 * Authorize the USB host to run in High Speed.
 *
 * \b 4. USB_HOST_HUB_SUPPORT (Only defined).
 *
 * Authorize the USB HUB support.
 *
 * \section USB_HOST_CALLBACK USB Host User Callback
 * The following optional USB host callback can be defined in the conf_usb_host.h
 * file of the application:
 *
 * \b 1. void UHC_MODE_CHANGE (bool b_host_mode).
 *
 * To notify that the USB mode are switched automatically.
 * This is possible only when ID pin is available.
 *
 * \b 2. void UHC_VBUS_CHANGE (bool b_present).
 *
 * To notify that the Vbus level has changed
 * (Available only in USB hardware with Vbus monitoring).
 *
 * \b 3. void UHC_VBUS_ERROR (void).
 *
 * To notify that a Vbus error has occurred
 * (Available only in USB hardware with Vbus monitoring).
 *
 * \b 4. void UHC_CONNECTION_EVENT (uhc_device_t* dev, bool b_present).
 *
 * To notify that a device has been connected or disconnected.
 *
 * \b 5. void UHC_WAKEUP_EVENT (void).
 *
 * Called when a USB device or the host have wake up the USB line.
 *
 * \b 6. void UHC_SOF_EVENT (void).
 *
 * Called for each received SOF each 1ms.
 * Available in High and Full speed mode.
 *
 * \b 7. uint8_t UHC_DEVICE_CONF (uhc_device_t* dev).
 *
 * Called when a USB device configuration must be chosen.
 * Thus, the application can choose either a configuration number
 * for this device or a configuration number 0 to reject it.
 * If callback not defined the configuration 1 is chosen.
 *
 * \b 8. void UHC_ENUM_EVENT (uhc_device_t* dev, uint8_t b_status).
 *
 * Called when a USB device enumeration is completed or failed.
 *
 * \section asfdoc_uhc_basic_use_case_setup_steps USB Host Setup Steps
 *
 * \subsection asfdoc_uhc_basic_use_case_setup_prereq USB Host Controller (UHC) - Prerequisites
 * Common prerequisites for all USB hosts.
 *
 * This module is based on USB host stack full interrupt driven and supporting
 * \ref sleepmgr_group "sleepmgr". For AVR&reg; and Atmel&reg; | SMART ARM&reg;-based
 * SAM3/4 devices the \ref clk_group "clock services" is supported. For SAM D21 devices the
 * \ref asfdoc_sam0_system_clock_group "clock driver" is supported.
 *
 * The following procedure must be executed to setup the project correctly:
 * - Specify the clock configuration:
 *   - UC3 and SAM3/4 devices without USB high speed support need 48MHz clock input.
 *     You must use a PLL and an external OSC.
 *   - UC3 and SAM3/4 devices with USB high speed support need 12MHz clock input.
 *     You must use an external OSC.
 *   - UC3 devices with USBC hardware need CPU frequency higher than 25MHz
 *   - SAM D21 devices without USB high speed support need 48MHz clock input.
 *     You must use a DFLL and an external OSC.
 * - In conf_board.h, the define CONF_BOARD_USB_PORT must be added to enable USB lines.
 *   (Not mandatory for all boards).
 * - Enable interrupts
 * - Initialize the clock service
 *
 * The usage of \ref sleepmgr_group "sleep manager" service is optional, but
 * recommended to reduce power consumption:
 * - Initialize the sleep manager service
 * - Activate sleep mode when the application is in IDLE state
 *
 *
 * For AVR and SAM3/4 devices, add to the initialization code:
 * \code
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	board_init();
	sleepmgr_init(); // Optional
 \endcode
 *
 * For SAM D21 devices, add to the initialization code:
 * \code
	system_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	sleepmgr_init(); // Optional
 \endcode
 * Add to the main IDLE loop:
 * \code
	sleepmgr_enter_sleep(); // Optional
 \endcode
 *
 * \subsection asfdoc_uhc_basic_use_case_setup_code USB Host Controller (UHC) - Example Code
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
	uhc_start();
}
 \endcode
 *
 * \subsection asfdoc_uhc_basic_use_case_setup_flow USB Device Controller (UHC) - Workflow
 * Common workflow for all USB devices.
 *
 * -# Ensure that conf_usb_host.h is available and contains the following configuration
 * which is the main USB device configuration:
 * \code
	// Maximum current allowed on Vbus (mA) which depends of 5V generator
	#define USB_HOST_POWER_MAX  500 // (500mA)
 \endcode

 * -# Call the USB host stack start function to enable USB Host stack:
 *   \code
	uhc_start();
 \endcode
 *
 * \section uhc_conf_clock conf_clock.h Examples
 *
 * Content of conf_clock.h for AT32UC3A0, AT32UC3A1, and AT32UC3B devices (USBB):
 * \code
	// Configuration based on 12MHz external OSC:
	#define CONFIG_PLL1_SOURCE          PLL_SRC_OSC0
	#define CONFIG_PLL1_MUL             8
	#define CONFIG_PLL1_DIV             2
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_PLL1
	#define CONFIG_USBCLK_DIV           1 // Fusb = Fsys/(2 ^ USB_div)
 \endcode
 *
 * Content of conf_clock.h for AT32UC3A3 and AT32UC3A4 devices (USBB with high speed support):
 * \code
	// Configuration based on 12MHz external OSC:
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_OSC0
	#define CONFIG_USBCLK_DIV           1 // Fusb = Fsys/(2 ^ USB_div)
 \endcode
 *
 * Content of conf_clock.h for AT32UC3C device (USBC):
 * \code
	// Configuration based on 12MHz external OSC:
	#define CONFIG_PLL1_SOURCE          PLL_SRC_OSC0
	#define CONFIG_PLL1_MUL             8
	#define CONFIG_PLL1_DIV             2
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_PLL1
	#define CONFIG_USBCLK_DIV           1 // Fusb = Fsys/(2 ^ USB_div)
	// CPU clock need of clock > 25MHz to run with USBC
	#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_PLL1
 \endcode
 *
 * Content of conf_clock.h for SAM3X and SAM3A devices (UOTGHS: USB OTG High Speed):
 * \code
	// USB Clock Source fixed at UPLL.
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_UPLL
	#define CONFIG_USBCLK_DIV           1
 \endcode
 *
 * Content of conf_clocks.h for SAM D21 devices (USB):
 * \code
  // USB Clock Source fixed at DFLL.
  // SYSTEM_CLOCK_SOURCE_XOSC32K configuration - External 32KHz crystal/clock oscillator
  #  define CONF_CLOCK_XOSC32K_ENABLE               true
  #  define CONF_CLOCK_XOSC32K_EXTERNAL_CRYSTAL     SYSTEM_CLOCK_EXTERNAL_CRYSTAL
  #  define CONF_CLOCK_XOSC32K_STARTUP_TIME         SYSTEM_XOSC32K_STARTUP_65536
  #  define CONF_CLOCK_XOSC32K_AUTO_AMPLITUDE_CONTROL  false
  #  define CONF_CLOCK_XOSC32K_ENABLE_1KHZ_OUPUT    false
  #  define CONF_CLOCK_XOSC32K_ENABLE_32KHZ_OUTPUT  true
  #  define CONF_CLOCK_XOSC32K_ON_DEMAND            false
  #  define CONF_CLOCK_XOSC32K_RUN_IN_STANDBY       true
  // SYSTEM_CLOCK_SOURCE_DFLL configuration - Digital Frequency Locked Loop
  #  define CONF_CLOCK_DFLL_ENABLE                  true
  #  define CONF_CLOCK_DFLL_LOOP_MODE               SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED
  #  define CONF_CLOCK_DFLL_ON_DEMAND               true

  // DFLL closed loop mode configuration
  #  define CONF_CLOCK_DFLL_SOURCE_GCLK_GENERATOR   GCLK_GENERATOR_1
  #  define CONF_CLOCK_DFLL_MULTIPLY_FACTOR         (48000000/32768)
  #  define CONF_CLOCK_DFLL_QUICK_LOCK              true
  #  define CONF_CLOCK_DFLL_TRACK_AFTER_FINE_LOCK   true
  #  define CONF_CLOCK_DFLL_KEEP_LOCK_ON_WAKEUP     true
  #  define CONF_CLOCK_DFLL_ENABLE_CHILL_CYCLE      true
  #  define CONF_CLOCK_DFLL_MAX_COARSE_STEP_SIZE    (0x1f / 8)
  #  define CONF_CLOCK_DFLL_MAX_FINE_STEP_SIZE      (0xff / 8)

  #  define CONF_CLOCK_CONFIGURE_GCLK               true

  // Configure GCLK generator 0 (Main Clock)
  #  define CONF_CLOCK_GCLK_0_ENABLE                true
  #  define CONF_CLOCK_GCLK_0_RUN_IN_STANDBY        true
  #  define CONF_CLOCK_GCLK_0_CLOCK_SOURCE          SYSTEM_CLOCK_SOURCE_DFLL
  #  define CONF_CLOCK_GCLK_0_PRESCALER             1
  #  define CONF_CLOCK_GCLK_0_OUTPUT_ENABLE         false

  // Configure GCLK generator 1
  #  define CONF_CLOCK_GCLK_1_ENABLE                true
  #  define CONF_CLOCK_GCLK_1_RUN_IN_STANDBY        false
  #  define CONF_CLOCK_GCLK_1_CLOCK_SOURCE          SYSTEM_CLOCK_SOURCE_XOSC32K
  #  define CONF_CLOCK_GCLK_1_PRESCALER             1
  #  define CONF_CLOCK_GCLK_1_OUTPUT_ENABLE         true

 \endcode
 */

/**
 * \if ASF_MANUAL
 * \page asfdoc_uhc_use_cases USB Host Advanced Use Cases
 * - \subpage uhc_use_case_1
 * - \subpage uhc_use_case_2
 * - \subpage uhc_use_case_3
 * \endif
 */

/**
 * \page uhc_use_case_1 Enable USB High Speed Support
 *
 * In this use case, the USB host is used to support USB high speed.
 *
 * \section uhc_use_case_1_setup Setup Steps
 *
 * Prior to implement this use case, be sure to have already
 * applied the UHI module "basic use case".
 *
 * \section uhc_use_case_1_usage Usage Steps
 *
 * \subsection uhc_use_case_1_usage_code Example Code
 * Content of conf_usb_host.h:
 * \code
	#define USB_HOST_HS_SUPPORT
 \endcode
 *
 * \subsection uhc_use_case_1_usage_flow Workflow
 * -# Ensure that conf_usb_host.h is available and contains the following parameters
 * required for a USB device high speed (480Mbit/s):
 *  \code
	#define  USB_HOST_HS_SUPPORT
 \endcode
 */

/**
 * \page uhc_use_case_2 Multiple Classes Support
 *
 * In this use case, the USB host is used to support several USB classes.
 *
 * \section uhc_use_case_2_setup Setup Steps
 *
 * Prior to implement this use case, be sure to have already
 * applied the UHI module "basic use case".
 *
 * \section uhc_use_case_2_usage Usage Steps
 *
 * \subsection uhc_use_case_2_usage_code Example Code
 * Content of conf_usb_host.h:
 * \code
	#define USB_HOST_UHI   UHI_HID_MOUSE, UHI_MSC, UHI_CDC
 \endcode

 *
 * \subsection uhc_use_case_2_usage_flow Workflow
 * -# Ensure that conf_usb_host.h is available and contains the following parameters:
 * \code
	#define USB_HOST_UHI   UHI_HID_MOUSE, UHI_MSC, UHI_CDC
 \endcode
 * \note USB_HOST_UHI defines the list of UHI supported by USB host.
 *       Here, you must add all classes that you want to support.
 */

/**
 * \page uhc_use_case_3 Dual Roles Support
 *
 * In this use case, the USB host and USB device are enabled, it is the dual role.
 *
 * \note On the Atmel boards, the switch of USB role is managed automatically by the
 * USB stack thank to a USB On-The-Go (OTG) connector and its USB ID pin.
 * Refer to section "Dual roles" for further information in the application note:
 * - <a href="http://www.atmel.com/images/doc8486.pdf">
 *   Atmel AVR4950: ASF - USB Host Stack</a>
 *
 * \section uhc_use_case_3_setup Setup Steps
 *
 * Prior to implement this use case, be sure to have already
 * applied the UHI module "basic use case".
 *
 * \section uhc_use_case_3_usage Usage Steps
 *
 * \subsection uhc_use_case_3_usage_code Example Code
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
   uhc_start();
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
 * \subsection uhc_use_case_3_usage_flow Workflow
 * -# In case of USB dual roles (Device and Host), the USB stack must be enabled
 *    by uhc_start() and the udc_start() must not be called.
 * \code
	//udc_start();
	uhc_start();
  \endcode
 *
 * -# In dual role, to know the current USB mode, the callback to notify the
 * mode changes can be used.
 *   - Ensure that conf_usb_host.h contains the following parameters:
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

