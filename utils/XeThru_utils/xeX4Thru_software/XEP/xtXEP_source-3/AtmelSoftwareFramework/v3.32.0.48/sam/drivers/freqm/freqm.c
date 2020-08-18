/**
 * \file
 *
 * \brief Frequency Meter driver for SAM4L.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#include "freqm.h"
#include "sysclk.h"
#include "sleepmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \internal
 * \brief FREQM callback function pointer array
 */
freqm_callback_t freqm_callback[FREQM_INTERRUPT_SOURCE_N];

struct freqm_dev_inst *_freqm_instance;

/**
 * \brief Initializes Frequency Meter configuration structure to defaults.
 *
 *  \param cfg  Configuration structure to initialize to default values.
 */
void freqm_get_config_defaults(struct freqm_config *const cfg)
{
	/* Sanity check arguments */
	Assert(cfg);

	/* Default configuration values */
	cfg->duration = FREQM_DURATION_DEFAULT;
	cfg->msr_clk = FREQM_CPU;
	cfg->ref_clk = FREQM_REF_OSC32;
}

/**
 * \brief Configure FREQM with specified value.
 *
 * \param dev_inst  Device structure pointer.
 * \param freqm   Base address of the FREQM instance.
 * \param cfg    Pointer to FREQM configuration
 *
 * \return Status code
 */
enum status_code freqm_init(
		struct freqm_dev_inst *const dev_inst,
		Freqm *const freqm,
		struct freqm_config *const cfg)
{
	uint32_t timeout = FREQM_NUM_OF_ATTEMPTS;

	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(freqm);
	Assert(cfg);

	dev_inst->hw_dev = freqm;
	dev_inst->freqm_cfg = cfg;

	sysclk_enable_peripheral_clock(freqm);

	freqm->FREQM_MODE = 0;

	freqm_disable_refclk(dev_inst);
	while (freqm_get_status(dev_inst) & FREQM_STATUS_RCLKBUSY) {
		if (!timeout--) {
			return ERR_TIMEOUT;
		}
	}
	freqm->FREQM_MODE |= FREQM_MODE_REFSEL(cfg->ref_clk);
	freqm_enable_refclk(dev_inst);
	while (freqm_get_status(dev_inst) & FREQM_STATUS_RCLKBUSY) {
		if (!timeout--) {
			return ERR_TIMEOUT;
		}
	}
	freqm->FREQM_MODE |= FREQM_MODE_REFNUM(cfg->duration);
	freqm->FREQM_MODE |= FREQM_MODE_CLKSEL(cfg->msr_clk);

	_freqm_instance = dev_inst;

	return STATUS_OK;
}

/**
 * \brief Get measurement result.

 * \param dev_inst  Device structure pointer.
 * \param p_result  Pointer to measurement result value.
 *
 * \return Status code
 */
enum status_code freqm_get_result_blocking(struct freqm_dev_inst *const dev_inst,
		uint32_t *p_result)
{
	uint32_t timeout = FREQM_NUM_OF_ATTEMPTS;

	/* Wait until the measurement is done */
	while (freqm_get_status(dev_inst) & FREQM_STATUS_BUSY) {
		if (!timeout--) {
			return ERR_TIMEOUT;
		}
	}
	*p_result = dev_inst->hw_dev->FREQM_VALUE;
	return STATUS_OK;
}

/**
 * \brief Enable FEQM.
 *
 * \param dev_inst  Device structure pointer.
 *
 */
void freqm_enable(struct freqm_dev_inst *const dev_inst)
{
	sysclk_enable_peripheral_clock(dev_inst->hw_dev);
	sleepmgr_lock_mode(SLEEPMGR_SLEEP_1);
}

/**
 * \brief Disable FREQM.
 *
 * \param dev_inst  Device structure pointer.
 *
 * \return Status code
 */
enum status_code freqm_disable(struct freqm_dev_inst *const dev_inst)
{
	uint32_t timeout = FREQM_NUM_OF_ATTEMPTS;

	/* Wait until the measurement is done */
	while (freqm_get_status(dev_inst) & FREQM_STATUS_BUSY) {
		if (!timeout--) {
			return ERR_TIMEOUT;
		}
	}
	sysclk_disable_peripheral_clock(dev_inst->hw_dev);
	sleepmgr_unlock_mode(SLEEPMGR_SLEEP_1);

	return STATUS_OK;
}

/**
 * \brief Set callback for FREQM interrupt handler
 *
 * \param dev_inst  Device structure pointer.
 * \param source Interrupt source.
 * \param callback callback function pointer.
 * \param irq_level interrupt level.
 */
void freqm_set_callback(struct freqm_dev_inst *const dev_inst,
		freqm_interrupt_source_t source, freqm_callback_t callback,
		uint8_t irq_level)
{
	freqm_callback[source] = callback;
	irq_register_handler((IRQn_Type)FREQM_IRQn, irq_level);
	freqm_enable_interrupt(dev_inst, source);
}

/**
 * \brief Interrupt handler for FREQM.
 */
void FREQM_Handler(void)
{
	uint32_t status = freqm_get_interrupt_status(_freqm_instance);
	uint32_t mask = freqm_get_interrupt_mask(_freqm_instance);

	if ((status & FREQM_ISR_DONE) && (mask & FREQM_IMR_DONE)) {
		freqm_callback[FREQM_INTERRUPT_MEASURMENT_READY]();
	}

	if ((status & FREQM_ISR_RCLKRDY) && (mask & FREQM_IMR_RCLKRDY)) {
		freqm_callback[FREQM_INTERRUPT_REFERENCE_CLOCK_READY]();
	}
}



#ifdef __cplusplus
}
#endif

