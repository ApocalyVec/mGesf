/**
 * \file
 *
 * \brief Analog Comparator interface driver for SAM4L.
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

#include <sysclk.h>
#include <sleepmgr.h>
#include "acifc.h"

/// @cond
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

struct ac_dev_inst  *_ac_instance;
ac_callback_t ac_callback_pointer[AC_INTERRUPT_NUM];

/**
 * \internal Configure the ACIFC module.
 *
 * \param[in] dev_inst Driver structure pointer
 * \param[in] cfg      Module configuration structure pointer
 */
static inline void _ac_set_config(
		struct ac_dev_inst *const dev_inst,
		struct ac_config *const cfg)
{
	/* Sanity check arguments. */
	Assert(dev_inst);
	Assert(dev_inst->hw_dev);
	Assert(cfg);

	dev_inst->hw_dev->ACIFC_CTRL |=
			(cfg->event_trigger ? ACIFC_CTRL_EVENTEN : 0);
}

/**
 * \brief Initialize the ACIFC module.
 *
 * \note Enables the ACIFC module's source clock.
 *
 * \param[out] dev_inst Driver structure pointer
 * \param[in] ac        Module hardware register base address pointer
 * \param[in] cfg       Module configuration structure pointer
 *
 * \return Status of initialization.
 * \retval STATUS_OK         Module initiated correctly
 * \retval STATUS_ERR_DENIED Module has already been enabled
 * \retval STATUS_ERR_BUSY   Module is busy performing a comparison
 */
enum status_code ac_init(
		struct ac_dev_inst *const dev_inst,
		Acifc *const ac,
		struct ac_config *const cfg)
{
	/* Sanity check arguments. */
	Assert(dev_inst);
	Assert(ac);
	Assert(cfg);

	uint32_t ac_ctrl = ac->ACIFC_CTRL;
	/* Check if module is enabled. */
	if (ac_ctrl & ACIFC_CTRL_EN) {
		return STATUS_ERR_DENIED;
	}

	/* Check if comparison is in progress. */
	if (ac_ctrl & ACIFC_CTRL_USTART) {
		return STATUS_ERR_BUSY;
	}

	/* Enable and configure device instance */
	dev_inst->hw_dev = ac;
	sysclk_enable_peripheral_clock(ac);
	_ac_set_config(dev_inst, cfg);

	_ac_instance = (struct ac_dev_inst *)dev_inst;

	return STATUS_OK;
}

/**
 * \brief Configure the specified ACIFC channel.
 *
 * \param[in] dev_inst Driver structure pointer
 * \param[in] channel  ACIFC channel number (range 0 to 7 inclusive)
 * \param[in] cfg      ACIFC channel configuration structure pointer
 */
void ac_ch_set_config(
		struct ac_dev_inst *const dev_inst,
		uint32_t channel,
		struct ac_ch_config *cfg)
{
	/* Sanity check arguments. */
	Assert(dev_inst);
	Assert(dev_inst->hw_dev);
	Assert(channel < AC_NB_OF_CH);
	Assert(cfg);

	uint32_t ac_conf = 0;
	ac_conf = ACIFC_CONF_HYS(cfg->hysteresis_voltage) |
			(cfg->always_on ? ACIFC_CONF_ALWAYSON : 0) |
			(cfg->fast_mode ? ACIFC_CONF_FAST : 0) |
			(cfg->event_negative ? ACIFC_CONF_EVENN : 0) |
			(cfg->event_positive ? ACIFC_CONF_EVENP : 0) |
			ACIFC_CONF_INSELN(cfg->negative_input) |
			ACIFC_CONF_MODE(cfg->comparator_mode) |
			ACIFC_CONF_IS(cfg->interrupt_setting);

	dev_inst->hw_dev->ACIFC_CONF[channel].ACIFC_CONF = ac_conf;
}

/**
 * \brief Configure the specified ACIFC window.
 *
 * \param[in] dev_inst Driver structure pointer
 * \param[in] window   ACIFC window number (range 0 to 3 inclusive)
 * \param[in] cfg      ACIFC window configuration structure
 */
void ac_win_set_config(
		struct ac_dev_inst *const dev_inst,
		uint32_t window,
		struct ac_win_config *cfg)
{
	/* Sanity check arguments. */
	Assert(dev_inst);
	Assert(dev_inst->hw_dev);
	Assert(window < AC_NB_OF_WIN);
	Assert(cfg);

	uint32_t ac_confw = 0;
	ac_confw = (cfg->enable ? ACIFC_CONFW_WFEN : 0) |
			(cfg->event_enable ? ACIFC_CONFW_WEVEN : 0) |
			ACIFC_CONFW_WEVSRC(cfg->event_source) |
			ACIFC_CONFW_WIS(cfg->interrupt_setting);

	dev_inst->hw_dev->ACIFC_CONFW[window].ACIFC_CONFW = ac_confw;
}

/**
 * \brief Set the ACIFC interrupt callback.
 *
 * \param[in] dev_inst  Driver structure pointer
 * \param[in] source    \ref ac_interrupt_source "ACIFC interrupt source"
 * \param[in] callback  Interrupt callback function pointer
 * \param[in] irq_level Interrupt priority level
 */
void ac_set_callback(struct ac_dev_inst *const dev_inst,
		ac_interrupt_source_t source,
		ac_callback_t callback,
		uint8_t irq_level)
{
	ac_callback_pointer[source] = callback;
	irq_register_handler(ACIFC_IRQn, irq_level);
	ac_enable_interrupt(dev_inst, source);
}

/**
 * \brief Enable the specified ACIFC interrupt source.
 *
 * \param[out] dev_inst Driver structure pointer
 * \param[in] source    \ref ac_interrupt_source "Interrupt source" to enable
 */
void ac_enable_interrupt(
		struct ac_dev_inst *const dev_inst,
		ac_interrupt_source_t source)
{
	/* Sanity check arguments. */
	Assert(dev_inst);

	switch (source) {
		case AC_INTERRUPT_CONVERSION_COMPLETED_0:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_0:
		case AC_INTERRUPT_CONVERSION_COMPLETED_1:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_1:
		case AC_INTERRUPT_CONVERSION_COMPLETED_2:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_2:
		case AC_INTERRUPT_CONVERSION_COMPLETED_3:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_3:
		case AC_INTERRUPT_CONVERSION_COMPLETED_4:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_4:
		case AC_INTERRUPT_CONVERSION_COMPLETED_5:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_5:
		case AC_INTERRUPT_CONVERSION_COMPLETED_6:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_6:
		case AC_INTERRUPT_CONVERSION_COMPLETED_7:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_7:
			dev_inst->hw_dev->ACIFC_IER = (1 << source);
			break;
		case AC_INTERRUPT_WINDOW_0:
		case AC_INTERRUPT_WINDOW_1:
		case AC_INTERRUPT_WINDOW_2:
		case AC_INTERRUPT_WINDOW_3:
			dev_inst->hw_dev->ACIFC_IER = (1 << (source -
					AC_INTERRUPT_WINDOW_0 + AC_WIN_INT_OFFSET));
			break;
		default:
			break;
	}
}

/**
 * \brief Disable the specified ACIFC interrupt source.
 *
 * \param[in] dev_inst Driver structure pointer
 * \param[in] source   \ref ac_interrupt_source "Interrupt source" to disable
 *
 */
void ac_disable_interrupt(
		struct ac_dev_inst *const dev_inst,
		ac_interrupt_source_t source)
{
	/* Sanity check arguments. */
	Assert(dev_inst);

	switch (source) {
		case AC_INTERRUPT_CONVERSION_COMPLETED_0:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_0:
		case AC_INTERRUPT_CONVERSION_COMPLETED_1:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_1:
		case AC_INTERRUPT_CONVERSION_COMPLETED_2:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_2:
		case AC_INTERRUPT_CONVERSION_COMPLETED_3:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_3:
		case AC_INTERRUPT_CONVERSION_COMPLETED_4:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_4:
		case AC_INTERRUPT_CONVERSION_COMPLETED_5:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_5:
		case AC_INTERRUPT_CONVERSION_COMPLETED_6:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_6:
		case AC_INTERRUPT_CONVERSION_COMPLETED_7:
		case AC_INTERRUPT_STARTUP_TIME_ELAPSED_7:
			dev_inst->hw_dev->ACIFC_IDR = (1 << source);
			break;
		case AC_INTERRUPT_WINDOW_0:
		case AC_INTERRUPT_WINDOW_1:
		case AC_INTERRUPT_WINDOW_2:
		case AC_INTERRUPT_WINDOW_3:
			dev_inst->hw_dev->ACIFC_IDR = (1 << (source -
					AC_INTERRUPT_WINDOW_0 + AC_WIN_INT_OFFSET));
			break;
		default:
			break;
	}
}

/**
 * \brief Enable the ACIFC module.
 *
 * \param[in] dev_inst Driver structure pointer
 */
void ac_enable(
		struct ac_dev_inst *const dev_inst)
{
	sleepmgr_lock_mode(SLEEPMGR_BACKUP);
	dev_inst->hw_dev->ACIFC_CTRL |= ACIFC_CTRL_EN;
}

/**
 * \brief Disable the ACIFC Module.
 *
 * \param[in] dev_inst Driver structure pointer
 */
void ac_disable(
		struct ac_dev_inst *const dev_inst)
{
	dev_inst->hw_dev->ACIFC_CTRL &= ~ACIFC_CTRL_EN;
	sleepmgr_unlock_mode(SLEEPMGR_BACKUP);
}

/**
 * \internal ACIFC interrupt handler.
 */
static void ac_interrupt_handler(void)
{
	uint32_t status = ac_get_interrupt_status(_ac_instance);
	uint32_t mask = ac_get_interrupt_mask(_ac_instance);
	uint32_t i;

	for (i = 0; i < AC_INTERRUPT_WINDOW_0; i++) {
		if ((status & (1 << i)) && (mask & (1 << i))) {
			ac_callback_pointer[i]();
		}
	}
	for (i = 0; i < (AC_INTERRUPT_NUM - AC_INTERRUPT_WINDOW_0); i++) {
		if ((status & (1 << (i + AC_WIN_INT_OFFSET))) &&
				(mask & (1 << (i + AC_WIN_INT_OFFSET)))) {
			ac_callback_pointer[i + AC_INTERRUPT_WINDOW_0]();
		}
	}
}

/**
 * \internal Interrupt handler for ACIFC interrupt.
 */
void ACIFC_Handler(void)
{
	ac_interrupt_handler();
}

/// @cond
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
