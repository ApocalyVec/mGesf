/**
 * \file
 *
 * \brief SAM Integrity Check Monitor (ICM).
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

#include "icm.h"
#include "sysclk.h"
#include "sleepmgr.h"

/**
 * \ingroup group_sam_drivers_icm
 *
 * @{
 */

/**
 * \internal
 * \brief ICM callback function pointer.
 */
icm_callback_t icm_callback_pointer[_ICM_NUM_OF_INTERRUPT_SOURCE];

void icm_init(Icm *p_icm, struct icm_config *icm_cfg)
{
	/* Clock initialization */
	sysclk_enable_peripheral_clock(ID_ICM);

	/* ICM Reset */
	icm_reset(ICM);

	/* ICM basic configuration */
	p_icm->ICM_CFG = (icm_cfg->is_write_back ? ICM_CFG_WBDIS : 0) |
			(icm_cfg->is_dis_end_mon ? ICM_CFG_EOMDIS : 0) |
			(icm_cfg->is_sec_list_branch ? ICM_CFG_SLBDIS : 0) |
			 ICM_CFG_BBC(icm_cfg->bbc) | (icm_cfg->is_auto_mode ? ICM_CFG_ASCD : 0) |
			(icm_cfg->is_dual_buf ? ICM_CFG_DUALBUFF : 0) |
			(icm_cfg->is_user_hash ? ICM_CFG_UIHASH : 0) |
			ICM_CFG_HAPROT(icm_cfg->hash_area_val) |
			ICM_CFG_DAPROT(icm_cfg->des_area_val);

	if(icm_cfg->is_user_hash) {
		icm_set_algo(p_icm, icm_cfg->ualgo);
	}
}

void icm_enable(Icm *p_icm)
{
	sleepmgr_lock_mode(SLEEPMGR_SLEEP_WFI);
	p_icm->ICM_CTRL = ICM_CTRL_ENABLE;
}

void icm_disable(Icm *p_icm)
{
	sleepmgr_unlock_mode(SLEEPMGR_SLEEP_WFI);
	p_icm->ICM_CTRL = ICM_CTRL_DISABLE;
}

status_code_t icm_region_recompute_hash(Icm *p_icm, enum icm_region_num reg_num)
{
	if(icm_get_status(ICM) & ICM_SR_RMDIS(1 << reg_num)) {
		p_icm->ICM_CTRL = ICM_CTRL_REHASH(1 << reg_num);
		return STATUS_OK;
	} else {
		return STATUS_ERR_DENIED;
	}
}

void icm_set_callback(Icm *p_icm, icm_callback_t callback,
		enum icm_region_num reg_num, enum icm_interrupt_source interrupt_source,
		uint8_t irq_level)
{
	Assert(p_icm);
	Assert(callback);

	icm_callback_pointer[interrupt_source] = callback;
	irq_register_handler(ICM_IRQn, irq_level);
	if(interrupt_source == ICM_INTERRUPT_URAD) {
		icm_enable_interrupt(p_icm,
				1 << (uint32_t)(_ICM_NUM_OF_REGION * (uint32_t)interrupt_source));
	} else {
		icm_enable_interrupt(p_icm,
				1 << (_ICM_NUM_OF_REGION * (uint32_t)interrupt_source + reg_num));
	}
}

/**
 * \internal
 * \brief Common ICM interrupt handler
 *
 * The optional callback used by the interrupt handler is set by the
 * icm_set_callback() function.
 *
 * \param source   Interrupt source number
 */
static void icm_interrupt(enum icm_interrupt_source source,
		enum icm_region_num reg_num)
{
	if (icm_callback_pointer[source]) {
		icm_callback_pointer[source](reg_num);
	}
}

/**
 * \brief Interrupt handler for ICM.
 */
void ICM_Handler(void)
{
	volatile uint32_t status;
	uint8_t i, j;

	status = (icm_get_interrupt_status(ICM) & icm_get_interrupt_mask(ICM));

	for (i = 0; i < _ICM_NUM_OF_INTERRUPT_SOURCE; i++) {
		if(i == ICM_INTERRUPT_URAD) {
			if (status & (1 << (_ICM_NUM_OF_REGION * i))) {
				icm_interrupt((enum icm_interrupt_source)i, (enum icm_region_num)0);
			}
		} else {
			for (j = 0; j < _ICM_NUM_OF_REGION; j++) {
				if (status & (1 << (_ICM_NUM_OF_REGION * i + j))) {
					icm_interrupt((enum icm_interrupt_source)i, (enum icm_region_num)j);
				}
			}
		}
	}
}

void icm_set_algo(Icm *p_icm, enum icm_algo algo)
{
	p_icm->ICM_CFG &= ~ICM_CFG_UALGO_Msk;

	switch(algo) {
		case ICM_SHA_1:
			p_icm->ICM_CFG |= ICM_CFG_UALGO_SHA1;
			break;
		case ICM_SHA_224:
			p_icm->ICM_CFG |= ICM_CFG_UALGO_SHA224;
			break;
		case ICM_SHA_256:
			p_icm->ICM_CFG |= ICM_CFG_UALGO_SHA256;
			break;
		default:
			break;
	}
}

void icm_set_hash_value(Icm *p_icm, uint32_t *p_value)
{
	uint32_t algo = p_icm->ICM_CFG & ICM_CFG_UALGO_Msk;

	p_icm->ICM_UIHVAL[0] = p_value[0];
	p_icm->ICM_UIHVAL[1] = p_value[1];
	p_icm->ICM_UIHVAL[2] = p_value[2];
	p_icm->ICM_UIHVAL[3] = p_value[3];
	p_icm->ICM_UIHVAL[4] = p_value[4];
	p_icm->ICM_UIHVAL[5] = 0;
	p_icm->ICM_UIHVAL[6] = 0;
	p_icm->ICM_UIHVAL[7] = 0;

	switch(algo) {
		case (ICM_SHA_1 << ICM_CFG_UALGO_Pos):
			break;
		case (ICM_SHA_224 << ICM_CFG_UALGO_Pos):
			p_icm->ICM_UIHVAL[5] = p_value[5];
			p_icm->ICM_UIHVAL[6] = p_value[6];
			break;
		case (ICM_SHA_256 << ICM_CFG_UALGO_Pos):
			p_icm->ICM_UIHVAL[5] = p_value[5];
			p_icm->ICM_UIHVAL[6] = p_value[6];
			p_icm->ICM_UIHVAL[7] = p_value[7];
			break;
		default:
			break;
	}
}

/**
 * @}
 */
