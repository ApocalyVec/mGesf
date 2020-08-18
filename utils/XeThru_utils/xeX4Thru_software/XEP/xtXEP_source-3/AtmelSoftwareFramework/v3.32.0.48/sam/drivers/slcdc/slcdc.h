/**
 * \file
 *
 * \brief SAM Segment LCD Controller (SLCDC).
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef SLCDC_H_INCLUDED
#define SLCDC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"
#include "status_codes.h"
#include "conf_slcdc.h"

/**
 * \defgroup group_sam_drivers_slcdc Segment Liquid Crystal Display Controller (SLCDC)
 *
 * This is a driver for configuring, enabling/disabling and use of the on-chip SLCDC.
 *
 * \section dependencies Dependencies
 *
 * The SLCDC module depends on the following modules:
 * - \ref sysclk_group "System Clock Management (Sysclock)"
 * - \ref interrupt_group "Interrupt"
 * - \ref sleepmgr_group "Sleep manager (sleepmgr)"
 * @{
 */

/** Buffer On-Time */
enum slcdc_buf_time {
	/** Nominal drive time is 0% of SCLK period */
	SLCDC_BUFTIME_OFF = SLCDC_MR_BUFTIME_OFF,
	/** Nominal drive time is 2 periods of SCLK clock */
	SLCDC_BUFTIME_X2_SCLK = SLCDC_MR_BUFTIME_X2_SCLK_PERIOD,
	/** Nominal drive time is 4 periods of SCLK clock */
	SLCDC_BUFTIME_X4_SCLK = SLCDC_MR_BUFTIME_X4_SCLK_PERIOD,
	/** Nominal drive time is 8 periods of SCLK clock */
	SLCDC_BUFTIME_X8_SCLK = SLCDC_MR_BUFTIME_X8_SCLK_PERIOD,
	/** Nominal drive time is 16 periods of SCLK clock */
	SLCDC_BUFTIME_X16_SCLK = SLCDC_MR_BUFTIME_X16_SCLK_PERIOD,
	/** Nominal drive time is 32 periods of SCLK clock */
	SLCDC_BUFTIME_X32_SCLK = SLCDC_MR_BUFTIME_X32_SCLK_PERIOD,
	/** Nominal drive time is 64 periods of SCLK clock */
	SLCDC_BUFTIME_X64_SCLK = SLCDC_MR_BUFTIME_X64_SCLK_PERIOD,
	/** Nominal drive time is 128 periods of SCLK clock */
	SLCDC_BUFTIME_X128_SCLK = SLCDC_MR_BUFTIME_X128_SCLK_PERIOD,
	/** Nominal drive time is 50% of SCLK period */
	SLCDC_BUFTIME_PERCENT_50 = SLCDC_MR_BUFTIME_PERCENT_50,
	/** Nominal drive time is 100% of SCLK period */
	SLCDC_BUFTIME_PERCENT_100 = SLCDC_MR_BUFTIME_PERCENT_100,
};

/** Display Mode */
enum slcdc_disp_mode {
	/** Normal Mode */
	SLCDC_DISPMODE_NORMAL = SLCDC_DR_DISPMODE_NORMAL,
	/** Force Off Mode */
	SLCDC_DISPMODE_FORCE_OFF = SLCDC_DR_DISPMODE_FORCE_OFF,
	/** Force On Mode */
	SLCDC_DISPMODE_FORCE_ON = SLCDC_DR_DISPMODE_FORCE_ON,
	/** Blinking Mode */
	SLCDC_DISPMODE_BLINKING = SLCDC_DR_DISPMODE_BLINKING,
	/** Inverted Mode */
	SLCDC_DISPMODE_INVERTED = SLCDC_DR_DISPMODE_INVERTED,
	/** Inverted Blinking Mode */
	SLCDC_DISPMODE_INVERTED_BLINK = SLCDC_DR_DISPMODE_INVERTED_BLINK,
	/** User Buffer Only Load Mode */
	SLCDC_DISPMODE_USER_BUFFER = SLCDC_DR_DISPMODE_USER_BUFFER_LOAD,
	/** Buffer Swap Mode */
	SLCDC_DISPMODE_BUFFERS_SWAP = SLCDC_DR_DISPMODE_BUFFERS_SWAP,
};

/**
 * Basic configuration for SLCDC.
 */
struct slcdc_config {
	/** Buffer On-Time */
	enum slcdc_buf_time buf_time;
	/** Frame rate */
	uint32_t frame_rate;
	/** Display Mode. */
	enum slcdc_disp_mode disp_mode;
};

/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the slcdc_callback_t type.
 *
 */
typedef void (*slcdc_callback_t)(void);

/* The maxium clock prescaler and divider */
#define SLCDC_CLOCK_DIV_MAX          8
#define SLCDC_CLOCK_PRE_MAX          8

#if CONF_SLCDC_COM_NUM == 1
#define SLCDC_NCOM_VALUE                  16
#elif CONF_SLCDC_COM_NUM == 2
#define SLCDC_NCOM_VALUE                  16
#elif CONF_SLCDC_COM_NUM == 3
#define SLCDC_NCOM_VALUE                  15
#elif CONF_SLCDC_COM_NUM == 4
#define SLCDC_NCOM_VALUE                  16
#elif CONF_SLCDC_COM_NUM == 5
#define SLCDC_NCOM_VALUE                  15
#elif CONF_SLCDC_COM_NUM == 6
#define SLCDC_NCOM_VALUE                  18
#endif

/**
 * \brief Initialize SLCDC with specified configuration.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param slcdc_cfg  Pointer to SLCDC Configuration.
 */
status_code_t slcdc_init(Slcdc *p_slcdc, struct slcdc_config *slcdc_cfg);

/**
 * \brief  Enable SLCDC low power wave mode.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 */
static inline void slcdc_enable_lp_mode(Slcdc *p_slcdc)
{
	p_slcdc->SLCDC_MR |= SLCDC_MR_LPMODE;
}

/**
 * \brief  Disable SLCDC low power wave mode.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 */
static inline void slcdc_disable_lp_mode(Slcdc *p_slcdc)
{
	p_slcdc->SLCDC_MR &= ~SLCDC_MR_LPMODE;
}

/**
 * \brief  Set display mode for SLCDC.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param mode Display mode.
 */
static inline void slcdc_set_display_mode(Slcdc *p_slcdc,
		enum slcdc_disp_mode mode)
{
	p_slcdc->SLCDC_DR &= ~SLCDC_DR_DISPMODE_Msk;
	p_slcdc->SLCDC_DR |= mode;
}

/**
  * \brief  Set blinking frequency or period for SLCDC.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param value  Blinking frequency or period value.
 * \param is_freq_period   Select frequency if true.
 */
status_code_t slcdc_set_blink_freq(Slcdc *p_slcdc, uint32_t value,
		bool is_period_freq);

/**
 * \brief  Set Buffer On-Time for SLCDC.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param buf_time Buffer time value .
 */
static inline void slcdc_set_buf_time(Slcdc *p_slcdc,
		enum slcdc_buf_time buf_time)
{
	p_slcdc->SLCDC_MR &= ~SLCDC_MR_BUFTIME_Msk;
	p_slcdc->SLCDC_MR |= buf_time;
}

/**
 * \brief  Set frame rate for SLCDC.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param frame_rate  Frame rate value .
 */
status_code_t slcdc_set_frame_rate(Slcdc *p_slcdc, uint32_t frame_rate);

/**
 * \brief Enable SLCDC.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 */
void slcdc_enable(Slcdc *p_slcdc);

/**
 * \brief Disable SLCDC.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 */
void slcdc_disable(Slcdc *p_slcdc);

/**
 * \brief Reset SLCDC.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 */
static inline void slcdc_reset(Slcdc *p_slcdc)
{
	p_slcdc->SLCDC_CR = SLCDC_CR_SWRST;
}

/**
 * \brief Get the SLCDC status.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 *
 * \return return bit OR of the SLCDC status register.
 */
static inline uint32_t slcdc_get_status(Slcdc *p_slcdc)
{
	return p_slcdc->SLCDC_SR;
}

/**
 * \brief SLCDC interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param callback Reference to a callback function.
 * \param ul_sources Interrupts to be enabled.
 * \param irq_line  interrupt line.
 * \param irq_level interrupt level.
 */
void slcdc_set_callback(Slcdc *p_slcdc, slcdc_callback_t callback,
		uint32_t ul_sources, uint8_t irq_line, uint8_t irq_level);

/**
 * \brief SLCDC enable interrupt.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param ul_sources Interrupts to be enabled.
 */
static inline void slcdc_enable_interrupt(Slcdc *p_slcdc, uint32_t ul_sources)
{
	p_slcdc->SLCDC_IER = ul_sources;
}

/**
 * \brief SLCDC disable interrupt.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param ul_sources Interrupts to be disabled.
 */
static inline void slcdc_disable_interrupt(Slcdc *p_slcdc, uint32_t ul_sources)
{
	p_slcdc->SLCDC_IDR = ul_sources;
}

/**
 * \brief Get SLCDC interrupt mask.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 *
 * \return The interrupt mask value.
 */
static inline uint32_t slcdc_get_interrupt_mask(Slcdc *p_slcdc)
{
	return p_slcdc->SLCDC_IMR;
}

/**
 * \brief Get SLCDC interrupt status.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 *
 * \return The interrupt status value.
 */
static inline uint32_t slcdc_get_interrupt_status(Slcdc *p_slcdc)
{
	return p_slcdc->SLCDC_ISR;
}

/**
 * \brief SLCDC set segment map0.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param ul_sources Segments to be mapped.
 */
static inline void slcdc_set_segmap0(Slcdc *p_slcdc, uint32_t ul_sources)
{
	p_slcdc->SLCDC_SMR0 = ul_sources;
}

/**
 * \brief SLCDC set segment map1.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param ul_sources Segments to be mapped.
 */
static inline void slcdc_set_segmap1(Slcdc *p_slcdc, uint32_t ul_sources)
{
	p_slcdc->SLCDC_SMR1 = ul_sources;
}

/**
 * \brief Enable or disable write protection of SLCDC registers.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param ul_enable 1 to enable, 0 to disable.
 */
static inline void slcdc_set_writeprotect(Slcdc *p_slcdc, uint32_t ul_enable)
{
	if (ul_enable) {
		p_slcdc->SLCDC_WPMR = SLCDC_WPMR_WPKEY(0x4c4344) | SLCDC_WPMR_WPEN;
	} else {
		p_slcdc->SLCDC_WPMR = SLCDC_WPMR_WPKEY(0x4c4344);
	}
}

/**
 * \brief Indicate write protect status.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 *
 * \return SLCDC_WPSR value.
 */
static inline uint32_t slcdc_get_writeprotect_status(Slcdc *p_slcdc)
{
	return p_slcdc->SLCDC_WPSR;
}

/**
 * \brief Set LSB Pixels memory associated to COMx terminal.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param index   COM terminal index.
 * \param val    SEG terminal value
 */
static inline void slcdc_set_lsb_memory(Slcdc *p_slcdc,
		uint32_t index,uint32_t val)
{
    p_slcdc->SLCDC_COM[index].SLCDC_LMEMR |= val;
}

/**
 * \brief Set MSB Pixels memory associated to COMx terminal.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param index   COM terminal index.
 * \param val    SEG terminal value
 */
static inline void slcdc_set_msb_memory(Slcdc *p_slcdc,
		uint32_t index,uint32_t val)
{
    p_slcdc->SLCDC_COM[index].SLCDC_MMEMR |= val;
}

/**
 * \brief Clear LSB Pixels memory associated to COMx terminal.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param index   COM terminal index.
 * \param val    SEG terminal value
 */
static inline void slcdc_clear_lsb_memory(Slcdc *p_slcdc,
		uint32_t index,uint32_t val)
{
    p_slcdc->SLCDC_COM[index].SLCDC_LMEMR &= ~val;
}

/**
 * \brief Clear MSB Pixels memory associated to COMx terminal.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param index   COM terminal index.
 * \param val    SEG terminal value
 */
static inline void slcdc_clear_msb_memory(Slcdc *p_slcdc,
		uint32_t index,uint32_t val)
{
    p_slcdc->SLCDC_COM[index].SLCDC_MMEMR &= ~val;
}

/**
 * \brief Set all SLCDC display memory.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 */
void slcdc_set_display_memory(Slcdc *p_slcdc);

/**
 * \brief Clear all SLCDC display memory.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 */
 void slcdc_clear_display_memory(Slcdc *p_slcdc);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/**
 * \page sam_drivers_slcdc_quick_start Quick Start Guide for the SLCDC driver
 *
 * This is the quick start guide for the \ref group_sam_drivers_slcdc, with
 * step-by-step instructions on how to configure and use the driver for
 * a specific use case.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section slcdc_qs_use_cases Use cases
 * - \ref slcdc_basic
 *
 * \section slcdc_basic SLCDC basic usage
 *
 * This use case will demonstrate how to configure and use of the on-chip
 * SLCD controller to address an external LCD segment (C42048A).
 *
 * \section slcdc_basic_setup Setup steps
 *
 * \subsection slcdc_basic_prereq Prerequisites
 *
 * This module requires the following service
 * - \ref clk_group "Clock Management (Sysclock)"
 * - \ref supc_group "Supply Controller (SUPC)"
 * - \ref sleepmgr_group "Sleep manager (sleepmgr)"
 *
 * \subsection slcdc_basic_setup_code Setup Code Example
 *
 * Add this to the main loop or a setup function:
 * \code
	   // Set LCD power mode: Internal supply
	   supc_set_slcd_power_mode(SUPC, SLCDC_POWER_MODE_LCDON_INVR);

	   struct slcdc_config slcdc_cfg;

	   // SLCDC Controller initialization
	   // - Clock,
	   // - Display mode: Normal
	   // - Frame Rate:  64Hz
	   slcdc_cfg.buf_time = SLCDC_BUFTIME_X64_SCLK;
	   slcdc_cfg.frame_rate= 64;
	   slcdc_cfg.disp_mode = SLCDC_DISPMODE_NORMAL;
	   slcdc_init(&slcdc_cfg);
	   slcdc_enable();

	   // Turn on LCD back light
	   ioport_set_pin_level(LCD_BL_GPIO, IOPORT_PIN_LEVEL_HIGH);
\endcode
 *
 * \subsection slcdc_basic_setup_workflow Basic Setup Workflow
 *
 * -# Set LCD power mode
 *  - \code supc_set_slcd_power_mode(SUPC, SLCDC_POWER_MODE_LCDON_INVR); \endcode
 * -# Initialize SLCDC with basic configuration
 *  - \code slcdc_init(&slcdc_cfg); \endcode
 * -# Enable SLCDC module
 *  - \code slcdc_enable(); \endcode
 * -# Turn on LCD back light
 *  - \code ioport_set_pin_level(LCD_BL_GPIO, IOPORT_PIN_LEVEL_HIGH); \endcode
 *
 */
#endif /* SLCDC_H_INCLUDED */
