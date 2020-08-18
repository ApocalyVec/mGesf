/**
 * \file
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

#include "sam4cm.h"

typedef void (*intfunc) (void);
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

void __iar_program_start(void);
int __low_level_init(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M4 core handlers */
#pragma weak NMI_Handler=Dummy_Handler
#pragma weak HardFault_Handler=Dummy_Handler
#pragma weak MemManage_Handler=Dummy_Handler
#pragma weak BusFault_Handler=Dummy_Handler
#pragma weak UsageFault_Handler=Dummy_Handler
#pragma weak SVC_Handler=Dummy_Handler
#pragma weak DebugMon_Handler=Dummy_Handler
#pragma weak PendSV_Handler=Dummy_Handler
#pragma weak SysTick_Handler=Dummy_Handler

/* Peripherals handlers */
#pragma weak SUPC_Handler=Dummy_Handler
#pragma weak RSTC_Handler=Dummy_Handler
#pragma weak RTC_Handler=Dummy_Handler
#pragma weak RTT_Handler=Dummy_Handler
#pragma weak WDT_Handler=Dummy_Handler
#pragma weak PMC_Handler=Dummy_Handler
#pragma weak EFC_Handler=Dummy_Handler
#pragma weak UART0_Handler=Dummy_Handler
#pragma weak PIOA_Handler=Dummy_Handler
#pragma weak PIOB_Handler=Dummy_Handler
#pragma weak USART0_Handler=Dummy_Handler
#pragma weak USART1_Handler=Dummy_Handler
#pragma weak USART2_Handler=Dummy_Handler
#pragma weak USART3_Handler=Dummy_Handler
#pragma weak TWI0_Handler=Dummy_Handler
#pragma weak TWI1_Handler=Dummy_Handler
#pragma weak SPI0_Handler=Dummy_Handler
#pragma weak TC0_Handler=Dummy_Handler
#pragma weak TC1_Handler=Dummy_Handler
#pragma weak TC2_Handler=Dummy_Handler
#pragma weak TC3_Handler=Dummy_Handler
#pragma weak TC4_Handler=Dummy_Handler
#pragma weak TC5_Handler=Dummy_Handler
#pragma weak ADC_Handler=Dummy_Handler
#pragma weak ARM1_Handler=Dummy_Handler
#pragma weak IPC0_Handler=Dummy_Handler
#pragma weak SLCDC_Handler=Dummy_Handler
#pragma weak TRNG_Handler=Dummy_Handler
#pragma weak ICM_Handler=Dummy_Handler
#pragma weak CPKCC_Handler=Dummy_Handler
#pragma weak AES_Handler=Dummy_Handler
#pragma weak PIOC_Handler=Dummy_Handler
#pragma weak UART1_Handler=Dummy_Handler
#pragma weak IPC1_Handler=Dummy_Handler
#pragma weak SPI1_Handler=Dummy_Handler
#pragma weak PWM_Handler=Dummy_Handler

/* Exception Table */
#pragma language = extended
#pragma segment = "CSTACK"

/* The name "__vector_table" has special meaning for C-SPY: */
/* it is where the SP start value is found, and the NVIC vector */
/* table register (VTOR) is initialized to this address if != 0 */

#pragma section = ".intvec"
#pragma location = ".intvec"
const DeviceVectors __vector_table = {
	(void*) __sfe("CSTACK"),

	.pfnReset_Handler      = (void*) Reset_Handler,
	.pfnNMI_Handler        = (void*) NMI_Handler,
	.pfnHardFault_Handler  = (void*) HardFault_Handler,
	.pfnMemManage_Handler  = (void*) MemManage_Handler,
	.pfnBusFault_Handler   = (void*) BusFault_Handler,
	.pfnUsageFault_Handler = (void*) UsageFault_Handler,
	.pfnReserved1_Handler  = (void*) (0UL),          /* Reserved */
	.pfnReserved2_Handler  = (void*) (0UL),          /* Reserved */
	.pfnReserved3_Handler  = (void*) (0UL),          /* Reserved */
	.pfnReserved4_Handler  = (void*) (0UL),          /* Reserved */
	.pfnSVC_Handler        = (void*) SVC_Handler,
	.pfnDebugMon_Handler   = (void*) DebugMon_Handler,
	.pfnReserved5_Handler  = (void*) (0UL),          /* Reserved */
	.pfnPendSV_Handler     = (void*) PendSV_Handler,
	.pfnSysTick_Handler    = (void*) SysTick_Handler,

	/* Configurable interrupts */
	.pfnSUPC_Handler   = (void*) SUPC_Handler,   /* 0  Supply Controller */
	.pfnRSTC_Handler   = (void*) RSTC_Handler,   /* 1  Reset Controller */
	.pfnRTC_Handler    = (void*) RTC_Handler,    /* 2  Real Time Clock */
	.pfnRTT_Handler    = (void*) RTT_Handler,    /* 3  Real Time Timer */
	.pfnWDT_Handler    = (void*) WDT_Handler,    /* 4  Watchdog Timer */
	.pfnPMC_Handler    = (void*) PMC_Handler,    /* 5  Power Management Controller */
	.pfnEFC_Handler    = (void*) EFC_Handler,    /* 6  Enhanced Embedded Flash Controller */
	.pvReserved7       = (void*) (0UL),          /* 7  Reserved */
	.pfnUART0_Handler  = (void*) UART0_Handler,  /* 8  UART 0 */
	.pvReserved9       = (void*) (0UL),          /* 9  Reserved */
	.pvReserved10      = (void*) (0UL),          /* 10 Reserved */
	.pfnPIOA_Handler   = (void*) PIOA_Handler,   /* 11 Parallel I/O Controller A */
	.pfnPIOB_Handler   = (void*) PIOB_Handler,   /* 12 Parallel I/O Controller B */
	.pvReserved13      = (void*) (0UL),          /* 13 Reserved */
	.pfnUSART0_Handler = (void*) USART0_Handler, /* 14 USART 0 */
	.pfnUSART1_Handler = (void*) USART1_Handler, /* 15 USART 1 */
	.pfnUSART2_Handler = (void*) USART2_Handler, /* 16 USART 2 */
	.pfnUSART3_Handler = (void*) USART3_Handler, /* 17 USART 3 */
	.pvReserved18      = (void*) (0UL),          /* 18 Reserved */
	.pfnTWI0_Handler   = (void*) TWI0_Handler,   /* 19 Two Wire Interface 0 */
	.pfnTWI1_Handler   = (void*) TWI1_Handler,   /* 20 Two Wire Interface 1 */
	.pfnSPI0_Handler   = (void*) SPI0_Handler,   /* 21 Serial Peripheral Interface 0 */
	.pvReserved22      = (void*) (0UL),          /* 22 Reserved */
	.pfnTC0_Handler    = (void*) TC0_Handler,    /* 23 Timer/Counter 0 */
	.pfnTC1_Handler    = (void*) TC1_Handler,    /* 24 Timer/Counter 1 */
	.pfnTC2_Handler    = (void*) TC2_Handler,    /* 25 Timer/Counter 2 */
	.pfnTC3_Handler    = (void*) TC3_Handler,    /* 26 Timer/Counter 3 */
	.pfnTC4_Handler    = (void*) TC4_Handler,    /* 27 Timer/Counter 4 */
	.pfnTC5_Handler    = (void*) TC5_Handler,    /* 28 Timer/Counter 5 */
	.pfnADC_Handler    = (void*) ADC_Handler,    /* 29 Analog To Digital Converter */
	.pfnARM1_Handler   = (void*) ARM1_Handler,   /* 30 FPU signals : FPIXC, FPOFC, FPUFC, FPIOC, FPDZC, FPIDC, FPIXC */
	.pfnIPC0_Handler   = (void*) IPC0_Handler,   /* 31 Interprocessor communication 0 */
	.pfnSLCDC_Handler  = (void*) SLCDC_Handler,  /* 32 Segment LCD Controller */
	.pfnTRNG_Handler   = (void*) TRNG_Handler,   /* 33 True Random Generator */
	.pfnICM_Handler    = (void*) ICM_Handler,    /* 34 Integrity Check Module */
	.pfnCPKCC_Handler  = (void*) CPKCC_Handler,  /* 35 Public Key Cryptography Controller */
	.pfnAES_Handler    = (void*) AES_Handler,    /* 36 Advanced Enhanced Standard */
	.pfnPIOC_Handler   = (void*) PIOC_Handler,   /* 37 Parallel I/O Controller C */
	.pfnUART1_Handler  = (void*) UART1_Handler,  /* 38 UART 1 */
	.pfnIPC1_Handler   = (void*) IPC1_Handler,   /* 39 Interprocessor communication 1 */
	.pfnSPI1_Handler   = (void*) SPI1_Handler,   /* 40 Serial Peripheral interface 1 */
	.pfnPWM_Handler    = (void*) PWM_Handler,    /* 41 Pulse Width Modulation */
};

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
int __low_level_init(void)
{
	uint32_t *pSrc = __section_begin(".intvec");

	SCB->VTOR = (uint32_t)pSrc;

	return 1; /* if return 0, the data sections will not be initialized */
}

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
void Reset_Handler(void)
{
	__iar_program_start();
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
	while (1) {
	}
}
