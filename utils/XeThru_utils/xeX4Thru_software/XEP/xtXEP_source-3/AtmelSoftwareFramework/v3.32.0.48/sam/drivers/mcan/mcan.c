/**
 * \file
 *
 * \brief SAM Control Area Network (MCAN) Low Level Driver
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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

#include "mcan.h"
#include "pmc.h"
#include <string.h>
#include <sysclk.h>

/** @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/** @endcond */

/* PCK5 ID,assigned to MCAN module */
#define PMC_PCK_5               5
/* Get a value of 2 to 15 bit. */
#define BIT_2_TO_15_MASK         0x0000fffc

/* Message ram definition. */
COMPILER_ALIGNED(4)
static struct mcan_rx_element_buffer mcan0_rx_buffer[CONF_MCAN0_RX_BUFFER_NUM];
COMPILER_ALIGNED(4)
static struct mcan_rx_element_fifo_0 mcan0_rx_fifo_0[CONF_MCAN0_RX_FIFO_0_NUM];
COMPILER_ALIGNED(4)
static struct mcan_rx_element_fifo_1 mcan0_rx_fifo_1[CONF_MCAN0_RX_FIFO_1_NUM];
COMPILER_ALIGNED(4)
static struct mcan_tx_element mcan0_tx_buffer[CONF_MCAN0_TX_BUFFER_NUM + CONF_MCAN0_TX_FIFO_QUEUE_NUM];
COMPILER_ALIGNED(4)
static struct mcan_tx_event_element mcan0_tx_event_fifo[CONF_MCAN0_TX_EVENT_FIFO];
COMPILER_ALIGNED(4)
static struct mcan_standard_message_filter_element mcan0_rx_standard_filter[CONF_MCAN0_RX_STANDARD_ID_FILTER_NUM];
COMPILER_ALIGNED(4)
static struct mcan_extended_message_filter_element mcan0_rx_extended_filter[CONF_MCAN0_RX_EXTENDED_ID_FILTER_NUM];

COMPILER_ALIGNED(4)
static struct mcan_rx_element_buffer mcan1_rx_buffer[CONF_MCAN1_RX_BUFFER_NUM];
COMPILER_ALIGNED(4)
static struct mcan_rx_element_fifo_0 mcan1_rx_fifo_0[CONF_MCAN1_RX_FIFO_0_NUM];
COMPILER_ALIGNED(4)
static struct mcan_rx_element_fifo_1 mcan1_rx_fifo_1[CONF_MCAN1_RX_FIFO_1_NUM];
COMPILER_ALIGNED(4)
static struct mcan_tx_element mcan1_tx_buffer[CONF_MCAN1_TX_BUFFER_NUM + CONF_MCAN1_TX_FIFO_QUEUE_NUM];
COMPILER_ALIGNED(4)
static struct mcan_tx_event_element mcan1_tx_event_fifo[CONF_MCAN1_TX_EVENT_FIFO];
COMPILER_ALIGNED(4)
static struct mcan_standard_message_filter_element mcan1_rx_standard_filter[CONF_MCAN1_RX_STANDARD_ID_FILTER_NUM];
COMPILER_ALIGNED(4)
static struct mcan_extended_message_filter_element mcan1_rx_extended_filter[CONF_MCAN1_RX_EXTENDED_ID_FILTER_NUM];

/**
 * \brief initialize MCAN memory .
 *
 * \param hw  Base address of the MCAN
 *
 */
static void _mcan_message_memory_init(Mcan *hw)
{
	if (hw == MCAN0) {
		hw->MCAN_SIDFC = ((uint32_t)mcan0_rx_standard_filter & BIT_2_TO_15_MASK) |
				MCAN_SIDFC_LSS(CONF_MCAN0_RX_STANDARD_ID_FILTER_NUM);
		hw->MCAN_XIDFC = ((uint32_t)mcan0_rx_extended_filter & BIT_2_TO_15_MASK) |
				MCAN_XIDFC_LSE(CONF_MCAN0_RX_EXTENDED_ID_FILTER_NUM);
		hw->MCAN_RXF0C = ((uint32_t)mcan0_rx_fifo_0 & BIT_2_TO_15_MASK) |
				MCAN_RXF0C_F0S(CONF_MCAN0_RX_FIFO_0_NUM);
		hw->MCAN_RXF1C = ((uint32_t)mcan0_rx_fifo_1 & BIT_2_TO_15_MASK) |
				MCAN_RXF1C_F1S(CONF_MCAN0_RX_FIFO_1_NUM);
		hw->MCAN_RXBC = ((uint32_t)mcan0_rx_buffer & BIT_2_TO_15_MASK);
		hw->MCAN_TXBC = ((uint32_t)mcan0_tx_buffer & BIT_2_TO_15_MASK) |
				MCAN_TXBC_NDTB(CONF_MCAN0_TX_BUFFER_NUM) |
				MCAN_TXBC_TFQS(CONF_MCAN0_TX_FIFO_QUEUE_NUM);
		hw->MCAN_TXEFC = ((uint32_t)mcan0_tx_event_fifo & BIT_2_TO_15_MASK) |
				MCAN_TXEFC_EFS(CONF_MCAN0_TX_EVENT_FIFO);
	} else if (hw == MCAN1) {
		hw->MCAN_SIDFC = ((uint32_t)mcan1_rx_standard_filter & BIT_2_TO_15_MASK) |
				MCAN_SIDFC_LSS(CONF_MCAN1_RX_STANDARD_ID_FILTER_NUM);
		hw->MCAN_XIDFC = ((uint32_t)mcan1_rx_extended_filter & BIT_2_TO_15_MASK) |
				MCAN_XIDFC_LSE(CONF_MCAN1_RX_EXTENDED_ID_FILTER_NUM);
		hw->MCAN_RXF0C = ((uint32_t)mcan1_rx_fifo_0 & BIT_2_TO_15_MASK) |
				MCAN_RXF0C_F0S(CONF_MCAN1_RX_FIFO_0_NUM);
		hw->MCAN_RXF1C = ((uint32_t)mcan1_rx_fifo_1 & BIT_2_TO_15_MASK) |
				MCAN_RXF1C_F1S(CONF_MCAN1_RX_FIFO_1_NUM);
		hw->MCAN_RXBC = ((uint32_t)mcan1_rx_buffer & BIT_2_TO_15_MASK);
		hw->MCAN_TXBC = ((uint32_t)mcan1_tx_buffer & BIT_2_TO_15_MASK) |
				MCAN_TXBC_NDTB(CONF_MCAN1_TX_BUFFER_NUM) |
				MCAN_TXBC_TFQS(CONF_MCAN1_TX_FIFO_QUEUE_NUM);
		hw->MCAN_TXEFC = ((uint32_t)mcan1_tx_event_fifo & BIT_2_TO_15_MASK) |
				MCAN_TXEFC_EFS(CONF_MCAN1_TX_EVENT_FIFO);
	}

	/**
	 * The data size in conf_mcan.h should be 8/12/16/20/24/32/48/64,
	 * The corresponding setting value in register is 0/1//2/3/4/5/6/7.
	 * To simplify the calculation, seperate to two group 8/12/16/20/24 which
	 * increased with 4 and 32/48/64 which increased with 16.
	 */
	if (CONF_MCAN_ELEMENT_DATA_SIZE <= 24) {
		hw->MCAN_RXESC = MCAN_RXESC_RBDS((CONF_MCAN_ELEMENT_DATA_SIZE - 8) / 4) |
				MCAN_RXESC_F0DS((CONF_MCAN_ELEMENT_DATA_SIZE - 8) / 4) |
				MCAN_RXESC_F1DS((CONF_MCAN_ELEMENT_DATA_SIZE - 8) / 4);
		hw->MCAN_TXESC = MCAN_TXESC_TBDS((CONF_MCAN_ELEMENT_DATA_SIZE - 8) / 4);
	} else {
		hw->MCAN_RXESC = MCAN_RXESC_RBDS((CONF_MCAN_ELEMENT_DATA_SIZE - 32) / 16 + 5) |
				MCAN_RXESC_F0DS((CONF_MCAN_ELEMENT_DATA_SIZE - 32) / 16 + 5) |
				MCAN_RXESC_F1DS((CONF_MCAN_ELEMENT_DATA_SIZE - 32) / 16 + 5);
		hw->MCAN_TXESC = MCAN_TXESC_TBDS((CONF_MCAN_ELEMENT_DATA_SIZE - 32) / 16 + 5);
	}
}

/**
 * \brief set default configuration when initialization.
 *
 * \param hw  Base address of the MCAN
 * \param config  default configuration parameters.
 */
static void _mcan_set_configuration(Mcan *hw, struct mcan_config *config)
{
	/* Timing setting. */
	hw->MCAN_BTP = MCAN_BTP_BRP(CONF_MCAN_NBTP_NBRP_VALUE) |
			MCAN_BTP_SJW(CONF_MCAN_NBTP_NSJW_VALUE) |
			MCAN_BTP_TSEG1(CONF_MCAN_NBTP_NTSEG1_VALUE) |
			MCAN_BTP_TSEG2(CONF_MCAN_NBTP_NTSEG2_VALUE);
	hw->MCAN_FBTP = MCAN_FBTP_FBRP(CONF_MCAN_FBTP_FBRP_VALUE) |
			MCAN_FBTP_FSJW(CONF_MCAN_FBTP_FSJW_VALUE) |
			MCAN_FBTP_FTSEG1(CONF_MCAN_FBTP_FTSEG1_VALUE) |
			MCAN_FBTP_FTSEG2(CONF_MCAN_FBTP_FTSEG2_VALUE) |
			MCAN_FBTP_TDCO(config->delay_compensation_offset);

	if (config->tdc_enable) {
		hw->MCAN_FBTP |= MCAN_FBTP_TDC_ENABLED;
	}
	
	hw->MCAN_RWD |= MCAN_RWD_WDC(config->watchdog_configuration);

	if (config->transmit_pause) {
		hw->MCAN_CCCR |= MCAN_CCCR_TXP;
	}

	if (!config->automatic_retransmission) {
		hw->MCAN_CCCR |= MCAN_CCCR_DAR;
	}

	if (config->clock_stop_request) {
		hw->MCAN_CCCR |= MCAN_CCCR_CSR;
	}

	hw->MCAN_TSCC = MCAN_TSCC_TCP(config->timestamp_prescaler) |
			MCAN_TSCC_TSS_TCP_INC;

	hw->MCAN_TOCC = MCAN_TOCC_TOP(config->timeout_period) |
			config->timeout_mode | config->timeout_enable;

	hw->MCAN_GFC = MCAN_GFC_ANFS(config->nonmatching_frames_action_standard) |
			MCAN_GFC_ANFE(config->nonmatching_frames_action_extended);
	if (config->remote_frames_standard_reject) {
		hw->MCAN_GFC |= MCAN_GFC_RRFS;
	}
	if (config->remote_frames_extended_reject) {
		hw->MCAN_GFC|= MCAN_GFC_RRFE;
	}

	hw->MCAN_XIDAM = config->extended_id_mask;

	if (config->rx_fifo_0_overwrite) {
		hw->MCAN_RXF0C |= MCAN_RXF0C_F0OM;
	}
	hw->MCAN_RXF0C |= MCAN_RXF0C_F0WM(config->rx_fifo_0_watermark);

	if (config->rx_fifo_1_overwrite) {
		hw->MCAN_RXF1C |= MCAN_RXF1C_F1OM;
	}
	hw->MCAN_RXF1C |= MCAN_RXF1C_F1WM(config->rx_fifo_1_watermark);

	if (config->tx_queue_mode) {
		hw->MCAN_TXBC |= MCAN_TXBC_TFQM;
	}

	hw->MCAN_TXEFC |= MCAN_TXEFC_EFWM(config->tx_event_fifo_watermark);
}

/**
 * \brief enable can module clock.
 *
 * \param module_inst  MCAN instance
 *
 */
static void _mcan_enable_peripheral_clock(struct mcan_module *const module_inst)
{
	if (module_inst->hw == MCAN0) {
		/* Turn on the digital interface clock. */
		pmc_enable_periph_clk(ID_MCAN0);
	} else if (module_inst->hw == MCAN1) {
		/* Turn on the digital interface clock. */
		pmc_enable_periph_clk(ID_MCAN1);
	}
}

/**
 * \brief initialize can module.
 *
 * \param module_inst  MCAN instance
 * \param hw  Base address of MCAN.
 * \param config default configuration .
 */
void mcan_init(struct mcan_module *const module_inst, Mcan *hw,
		struct mcan_config *config)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(hw);
	Assert(config);

	/* Associate the software module instance with the hardware module */
	module_inst->hw = hw;

	pmc_disable_pck(PMC_PCK_5);

	pmc_switch_pck_to_pllack(PMC_PCK_5, PMC_PCK_PRES(9));
	pmc_enable_pck(PMC_PCK_5);

	/* Enable peripheral clock */
	_mcan_enable_peripheral_clock(module_inst);


	/* Configuration Change Enable. */
	hw->MCAN_CCCR |= MCAN_CCCR_CCE;

	/* Initialize the message memory address. */
	_mcan_message_memory_init(hw);

	/* Set the configuration. */
	_mcan_set_configuration(hw, config);

	/* Enable the interrupt setting which no need change. */
	hw->MCAN_ILE = MCAN_ILE_EINT0 | MCAN_ILE_EINT1;
	hw->MCAN_TXBTIE = 0xFFFFFFFFul;
	hw->MCAN_TXBCIE = 0xFFFFFFFFul;
}

/**
 * \brief Set MCAN baudrate.
 *
 * \param[in]  hw          Pointer to the MCAN module instance
 * \param[in]  baudrate    MCAN baudrate
 */
void mcan_set_baudrate(Mcan *hw, uint32_t baudrate)
{
	uint32_t gclk_mcan_value;
	uint32_t mcan_nbtp_nbrp_value;
	uint32_t mcan_nbtp_nsgw_value = 3, mcan_nbtp_ntseg1_value = 10, mcan_nbtp_ntseg2_value = 7;

	gclk_mcan_value = sysclk_get_peripheral_hz();

	mcan_nbtp_nbrp_value = gclk_mcan_value / baudrate / (3 + mcan_nbtp_ntseg1_value + mcan_nbtp_ntseg2_value);
	
	hw->MCAN_BTP = MCAN_BTP_BRP(mcan_nbtp_nbrp_value) |
			MCAN_BTP_SJW(mcan_nbtp_nsgw_value) |
			MCAN_BTP_TSEG1(mcan_nbtp_ntseg1_value) |
			MCAN_BTP_TSEG2(mcan_nbtp_ntseg2_value);
}

/**
 * \brief Set MCAN_FD baudrate.
 *
 * \param[in]  hw          Pointer to the MCAN_FD module instance
 * \param[in]  baudrate    MCAN_FD baudrate
 */
void mcan_fd_set_baudrate(Mcan *hw, uint32_t baudrate)
{
	uint32_t gclk_mcan_fd_value;
	uint32_t mcan_fd_dbtp_dbrp_value;
	uint32_t mcan_fd_dbtp_dsgw_value = 3, mcan_fd_dbtp_dtseg1_value = 9, mcan_fd_dbtp_dtseg2_value = 3;

	gclk_mcan_fd_value = sysclk_get_peripheral_hz();
	
	mcan_fd_dbtp_dbrp_value = gclk_mcan_fd_value / baudrate / (3 + mcan_fd_dbtp_dtseg1_value + mcan_fd_dbtp_dtseg2_value);
	
	hw->MCAN_FBTP = MCAN_FBTP_FBRP(mcan_fd_dbtp_dbrp_value) |
			MCAN_FBTP_FSJW(mcan_fd_dbtp_dsgw_value) |
			MCAN_FBTP_FTSEG1(mcan_fd_dbtp_dtseg1_value) |
			MCAN_FBTP_FTSEG2(mcan_fd_dbtp_dtseg2_value);
}

/**
 * \brief start can module after initialization.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_start(struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR &= ~MCAN_CCCR_INIT;
	/* Wait for the sync. */
	while (module_inst->hw->MCAN_CCCR & MCAN_CCCR_INIT);
}

/**
 * \brief stop mcan module when bus off occurs
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_stop(struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_INIT;
	/* Wait for the sync. */
	while (!(module_inst->hw->MCAN_CCCR & MCAN_CCCR_INIT));
}

/**
 * \brief switch mcan module into fd mode.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_enable_fd_mode(struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_INIT;
	/* Wait for the sync. */
	while (!(module_inst->hw->MCAN_CCCR & MCAN_CCCR_INIT));
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_CCE;

	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_CME(2);
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_CMR(2);
}

/** 
 * \brief disable fd mode of mcan module.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_disable_fd_mode(struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR &= MCAN_CCCR_CME(MCAN_CCCR_CME_ISO11898_1);
}

/**
 * \brief enable restricted mode of mcan module.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_enable_restricted_operation_mode(
		struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_INIT;
	/* Wait for the sync. */
	while (!(module_inst->hw->MCAN_CCCR & MCAN_CCCR_INIT));
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_CCE;

	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_ASM;
}

/**
 * \brief disable restricted mode of mcan module.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_disable_restricted_operation_mode(
		struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR &= ~MCAN_CCCR_ASM;
}

/**
 * \brief enable bus monitor mode of mcan module.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_enable_bus_monitor_mode(struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_INIT;
	/* Wait for the sync. */
	while (!(module_inst->hw->MCAN_CCCR & MCAN_CCCR_INIT));
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_CCE;

	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_MON;
}

/**
 * \brief disable bus monitor mode of mcan module.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_disable_bus_monitor_mode(struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR &= ~MCAN_CCCR_MON;
}

/**
 * \brief enable sleep mode of mcan module.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_enable_sleep_mode(struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_CSR;
	/* Wait for the sync. */
	while (!(module_inst->hw->MCAN_CCCR & MCAN_CCCR_INIT));

	while (!(module_inst->hw->MCAN_CCCR & MCAN_CCCR_CSA));
}

/**
 * \brief disable sleep mode of mcan module.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_disable_sleep_mode(struct mcan_module *const module_inst)
{
	/* Enable peripheral clock */
	_mcan_enable_peripheral_clock(module_inst);

	module_inst->hw->MCAN_CCCR &= ~MCAN_CCCR_CSR;
	while ((module_inst->hw->MCAN_CCCR & MCAN_CCCR_CSA));
}

/**
 * \brief enable test mode of mcan module.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_enable_test_mode(struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_INIT;
	/* Wait for the sync. */
	while (!(module_inst->hw->MCAN_CCCR & MCAN_CCCR_INIT));
	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_CCE;

	module_inst->hw->MCAN_CCCR |= MCAN_CCCR_TEST;
	module_inst->hw->MCAN_TEST |= MCAN_TEST_LBCK;
}

/**
 * \brief disable test mode of mcan module.
 *
 * \param module_inst  MCAN instance
 *
 */
void mcan_disable_test_mode(struct mcan_module *const module_inst)
{
	module_inst->hw->MCAN_CCCR &= ~MCAN_CCCR_TEST;
}

/**
 * \brief set standard receive CAN ID.
 *
 * \param module_inst  MCAN instance
 * \param sd_filter  structure of CAN ID
 * \param index  CAN messages memory index for different CAN ID
 *
 * \return status code.
 */
enum status_code mcan_set_rx_standard_filter(
		struct mcan_module *const module_inst,
		struct mcan_standard_message_filter_element *sd_filter, uint32_t index)
{
	if (module_inst->hw == MCAN0) {
		mcan0_rx_standard_filter[index].S0.reg = sd_filter->S0.reg;
		return STATUS_OK;
	} else if (module_inst->hw == MCAN1) {
		mcan1_rx_standard_filter[index].S0.reg = sd_filter->S0.reg;
		return STATUS_OK;
	}
	return ERR_INVALID_ARG;
}

/**
 * \brief set extended receive CAN ID.
 *
 * \param module_inst  MCAN instance
 * \param sd_filter  structure of extended CAN ID
 * \param index  CAN messages memory index for different CAN ID
 *
 * \return status code.
 */
enum status_code mcan_set_rx_extended_filter(
		struct mcan_module *const module_inst,
		struct mcan_extended_message_filter_element *et_filter, uint32_t index)
{
	if (module_inst->hw == MCAN0) {
		mcan0_rx_extended_filter[index].F0.reg = et_filter->F0.reg;
		mcan0_rx_extended_filter[index].F1.reg = et_filter->F1.reg;
		return STATUS_OK;
	} else if (module_inst->hw == MCAN1) {
		mcan1_rx_extended_filter[index].F0.reg = et_filter->F0.reg;
		mcan1_rx_extended_filter[index].F1.reg = et_filter->F1.reg;
		return STATUS_OK;
	}
	return ERR_INVALID_ARG;
}

/**
 * \brief get dedicated rx buffer element .
 *
 * \param module_inst  MCAN instance
 * \param rx_element  structure of element
 * \param index  CAN messages memory index for receiving CAN ID
 *
 * \return status code.
 */
enum status_code mcan_get_rx_buffer_element(
		struct mcan_module *const module_inst,
		struct mcan_rx_element_buffer *rx_element, uint32_t index)
{
	if (module_inst->hw == MCAN0) {
		memcpy(rx_element, &mcan0_rx_buffer[index], sizeof(struct mcan_rx_element_buffer));
		return STATUS_OK;
	} else if (module_inst->hw == MCAN1) {
		memcpy(rx_element, &mcan1_rx_buffer[index], sizeof(struct mcan_rx_element_buffer));
		return STATUS_OK;
	}
	return ERR_INVALID_ARG;
}

/**
 * \brief get FIFO rx buffer element .
 *
 * \param module_inst  MCAN instance
 * \param rx_element  structure of element
 * \param index  CAN messages memory index for receiving CAN ID
 *
 * \return status code.
 */
enum status_code mcan_get_rx_fifo_0_element(
		struct mcan_module *const module_inst,
		struct mcan_rx_element_fifo_0 *rx_element, uint32_t index)
{
	if (module_inst->hw == MCAN0) {
		memcpy(rx_element, &mcan0_rx_fifo_0[index], sizeof(struct mcan_rx_element_buffer));
		return STATUS_OK;
	} else if (module_inst->hw == MCAN1) {
		memcpy(rx_element, &mcan1_rx_fifo_0[index], sizeof(struct mcan_rx_element_buffer));
		return STATUS_OK;
	}
	return ERR_INVALID_ARG;
}

/**
 * \brief get FIFO rx buffer element .
 *
 * \param module_inst  MCAN instance
 * \param rx_element  structure of element
 * \param index  CAN messages memory index for receiving CAN ID
 *
 * \return status code.
 */
enum status_code mcan_get_rx_fifo_1_element(
		struct mcan_module *const module_inst,
		struct mcan_rx_element_fifo_1 *rx_element, uint32_t index)
{
	if (module_inst->hw == MCAN0) {
		memcpy(rx_element, &mcan0_rx_fifo_1[index], sizeof(struct mcan_rx_element_buffer));
		return STATUS_OK;
	} else if (module_inst->hw == MCAN1) {
		memcpy(rx_element, &mcan1_rx_fifo_1[index], sizeof(struct mcan_rx_element_buffer));
		return STATUS_OK;
	}
	return ERR_INVALID_ARG;
}

/**
 * \brief set dedicated transmit buffer element .
 *
 * \param module_inst  MCAN instance
 * \param tx_element  structure of element
 * \param index  CAN messages memory index for transmitting CAN ID
 *
 * \return status code.
 */
enum status_code mcan_set_tx_buffer_element(
		struct mcan_module *const module_inst,
		struct mcan_tx_element *tx_element, uint32_t index)
{
	uint32_t i;
	if (module_inst->hw == MCAN0) {
		mcan0_tx_buffer[index].T0.reg = tx_element->T0.reg;
		mcan0_tx_buffer[index].T1.reg = tx_element->T1.reg;
		for (i = 0; i < CONF_MCAN_ELEMENT_DATA_SIZE; i++) {
			mcan0_tx_buffer[index].data[i] = tx_element->data[i];
		}
		return STATUS_OK;
	} else if (module_inst->hw == MCAN1) {
		mcan1_tx_buffer[index].T0.reg = tx_element->T0.reg;
		mcan1_tx_buffer[index].T1.reg = tx_element->T1.reg;
		for (i = 0; i < CONF_MCAN_ELEMENT_DATA_SIZE; i++) {
			mcan1_tx_buffer[index].data[i] = tx_element->data[i];
		}
		return STATUS_OK;
	}
	return ERR_INVALID_ARG;
}

/**
 * \brief set FIFO transmit buffer element .
 *
 * \param module_inst  MCAN instance
 * \param tx_element  structure of element
 * \param index  CAN messages memory index for transmitting CAN ID
 *
 * \return status code.
 */
enum status_code mcan_get_tx_event_fifo_element(
		struct mcan_module *const module_inst,
		struct mcan_tx_event_element *tx_event_element, uint32_t index)
{
	if (module_inst->hw == MCAN0) {
		tx_event_element->E0.reg = mcan0_tx_event_fifo[index].E0.reg;
		tx_event_element->E1.reg = mcan0_tx_event_fifo[index].E1.reg;
		return STATUS_OK;
	} else if (module_inst->hw == MCAN1) {
		tx_event_element->E0.reg = mcan1_tx_event_fifo[index].E0.reg;
		tx_event_element->E1.reg = mcan1_tx_event_fifo[index].E1.reg;
		return STATUS_OK;
	}
	return ERR_INVALID_ARG;
}

/** @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/** @endcond */