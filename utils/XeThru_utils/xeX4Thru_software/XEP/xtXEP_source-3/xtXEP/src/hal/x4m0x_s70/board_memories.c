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
 * Implementation of memories configuration on board.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "board.h"
#include "xt_selftest.h"
#include "xep_hal.h"
#include "xt_config.h"

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

#define SDRAM_BA0 (1 << 20)
#define SDRAM_BA1 (1 << 21)

void BOARD_ConfigureAndTestSdram(void)
{
	if (xt_get_operation_mode() == XT_OPMODE_FACTORY)
        xt_selftest_ext_ram_shorts();

	BOARD_ConfigureSdram();
    
    // Perform a selftest of functionality of external SDRAM
    if (xt_get_operation_mode() == XT_OPMODE_FACTORY)
    {
        xt_selftest_ext_ram_functionality();
    }
}


/**
 * \brief Configures the EBI for SDRAM (IS42S16100E-7B) access.
 */
void BOARD_ConfigureSdram(void)
{
	const Pin pinsSdram[] = {BOARD_SDRAM_PINS};
	volatile uint32_t i;
	volatile uint8_t *pSdram = (uint8_t *) SDRAM_CS_ADDR;
	uint32_t ul_clk = BOARD_MCK * 2;

	/* Configure PIO */
	PIO_Configure(pinsSdram, PIO_LISTSIZE(pinsSdram));
	PMC_EnablePeripheral(ID_SDRAMC);
	MATRIX->CCFG_SMCNFCS = CCFG_SMCNFCS_SDRAMEN;

	/* 1. SDRAM features must be set in the configuration register:
	asynchronous timings (TRC, TRAS, etc.), number of columns, rows,
	CAS latency, and the data bus width. */
	SDRAMC->SDRAMC_CR =
        SDRAMC_CR_NC(sdram_config.cols - 8)             // Column bits
		| SDRAMC_CR_NR(sdram_config.rows - 11)          // Row bits
		| SDRAMC_CR_CAS(sdram_config.cas_latency - 1)   // CAS Latency
		| ((sdram_config.banks == 4) ? SDRAMC_CR_NB_BANK4 : SDRAMC_CR_NB_BANK2)     // Banks
		| SDRAMC_CR_DBW                                 // 16 bit
		| SDRAMC_CR_TWR(sdram_config.write_recovery_delay)
		| SDRAMC_CR_TRC_TRFC(sdram_config.row_cycle_delay_and_row_refresh_cycle)
		| SDRAMC_CR_TRP(sdram_config.row_precharge_delay)
		| SDRAMC_CR_TRCD(sdram_config.row_to_column_delay)
		| SDRAMC_CR_TRAS(sdram_config.active_to_precharge_delay)
		| SDRAMC_CR_TXSR(sdram_config.exit_self_refresh_to_active_delay);

    /* Set unaligned support */
    SDRAMC->SDRAMC_CFR1 = SDRAMC_CFR1_TMRD(sdram_config.load_mode_register_command_to_active_or_refresh_command) 
                            + ((sdram_config.supports_unaligned_access != 0) ?  SDRAMC_CFR1_UNAL_SUPPORTED : 
                                                                                SDRAMC_CFR1_UNAL_UNSUPPORTED);

	/* 2. For mobile SDRAM, temperature-compensated self refresh (TCSR), drive
	strength (DS) and partial array self refresh (PASR) must be set in the
	Low Power Register. */
	SDRAMC->SDRAMC_LPR = sdram_config.low_power_config;

	/* 3. The SDRAM memory type must be set in the Memory Device Register.*/
    SDRAMC->SDRAMC_MDR = (sdram_config.low_power_config != 0) ? SDRAMC_MDR_MD_LPSDRAM : 
                                                                SDRAMC_MDR_MD_SDRAM;

	/* 4. A minimum pause of 200 ��s is provided to precede any signal toggle.*/
	for (i = 0; i < ((ul_clk / 1000000) * 1000); i++) {
		;
	}
	
	/* 5. (1)A NOP command is issued to the SDRAM devices. The application must
	set Mode to 1 in the Mode Register and perform a write access to
	any SDRAM address.*/
	SDRAMC->SDRAMC_MR = SDRAMC_MR_MODE_NOP;
	*pSdram = 0;

	for (i = 0; i < ((ul_clk / 1000000) * 1000); i++) {
		;
	}
	
	/* 6. An All Banks Precharge command is issued to the SDRAM devices.
	The application must set Mode to 2 in the Mode Register and perform a write
	access to any SDRAM address. */
	SDRAMC->SDRAMC_MR = SDRAMC_MR_MODE_ALLBANKS_PRECHARGE;
	*pSdram = 0;

	for (i = 0; i < ((ul_clk / 1000000) * 1000); i++) {
		;
	}
	
	/* 7. Eight auto-refresh (CBR) cycles are provided. The application must
	set the Mode to 4 in the Mode Register and perform a write access to any
	SDRAM location eight times.*/
	for (i = 0; i < 8; i++) {
		SDRAMC->SDRAMC_MR = SDRAMC_MR_MODE_AUTO_REFRESH;
		*pSdram = 0;
	}

	for (i = 0; i < ((ul_clk / 1000000) * 1000); i++) {
		;
	}
	
	/*8. A Mode Register set (MRS) cycle is issued to program the parameters of
	the SDRAM devices, in particular CAS latency and burst length. The
	application must set Mode to 3 in the Mode Register and perform a write
	access to the SDRAM. The write address must be chosen so that BA[1:0]
	are set to 0. For example, with a 16-bit 128 MB SDRAM (12 rows, 9 columns,
	4 banks) bank address, the SDRAM write access should be done at the address
	0x70000000.*/
	SDRAMC->SDRAMC_MR = SDRAMC_MR_MODE_LOAD_MODEREG;
	uint32_t dummy = SDRAMC->SDRAMC_MR;
	(void) dummy;
	__DMB();
	*((uint16_t *)(pSdram + 0b0000000110000)) = 0xcafe;
	//##KIH *pSdram = 0;

	for (i = 0; i < ((ul_clk / 1000000) * 1000); i++) {
		;
	}
	
	/*9. For mobile SDRAM initialization, an Extended Mode Register set (EMRS)
	cycle is issued to program the SDRAM parameters (TCSR, PASR, DS). The
	application must set Mode to 5 in the Mode Register and perform a write
	access to the SDRAM. The write address must be chosen so that BA[1] or BA[0]
	are set to 1.
	For example, with a 16-bit 128 MB SDRAM, (12 rows, 9 columns, 4 banks) bank
	address the SDRAM write access should be done at the address 0x70800000 or
	0x70400000. */
	//SDRAMC->SDRAMC_MR = SDRAMC_MR_MODE_EXT_LOAD_MODEREG;
	// *((uint8_t *)(pSdram + SDRAM_BA0)) = 0;

	/* 10. The application must go into Normal Mode, setting Mode to 0 in the
	Mode Register and performing a write access at any location in the SDRAM. */
	SDRAMC->SDRAMC_MR = SDRAMC_MR_MODE_NORMAL;
	*pSdram = 0;

	for (i = 0; i < ((ul_clk / 1000000) * 1000); i++) {
		;
	}

	/* 11. Write the refresh rate into the count field in the SDRAMC Refresh
	Timer register. (Refresh rate = delay between refresh cycles).
	The SDRAM device requires a refresh every 15.625 ��s or 7.81 ��s.
	With a 100 MHz frequency, the Refresh Timer Counter Register must be set
	with the value 1562(15.625 ��s x 100 MHz) or 781(7.81 ��s x 100 MHz). */
	// For IS42S16100E, 2048 refresh cycle every 32ms, every 15.625 ��s
	/* ((32 x 10(^-3))/2048) x150 x (10^6) */
	// SDRAMC->SDRAMC_TR = 1562 * 3;
	i = ul_clk / 1000000u;  //KIH Was i = ul_clk / 1000u; but this will give overflow in next line
	i *= sdram_config.refresh_timer_count;
	i /= 1000u; //KIH Was i /= 1000000u; ajusted for change above    
	SDRAMC->SDRAMC_TR = SDRAMC_TR_COUNT(i);	
//	SDRAMC->SDRAMC_CFR1 |= SDRAMC_CFR1_UNAL;
	/* After initialization, the SDRAM devices are fully functional. */
}
