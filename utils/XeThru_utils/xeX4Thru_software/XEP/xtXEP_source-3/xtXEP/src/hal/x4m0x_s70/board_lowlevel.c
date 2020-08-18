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
 * Provides the low-level initialization function that called on chip startup.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"
#include "xep_hal.h"
#include "xt_config.h"


#if defined(ENABLE_TCM) && defined(__GNUC__)
	extern int _itcm_lma, _sitcm, _eitcm;
	extern int _dtcm_data_lma, _sdtcm_data, _edtcm_data;
#endif


/** Global DMA driver for all transfer */
sXdmad dmad;

/**
 * \brief XDMA handler.
 */
void XDMAC_Handler(void)
{
	XDMAD_Handler(&dmad);
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
/* Default memory map
   NO. Address range          Memory region    Memory type     Shareable?    Cache policy
   1   0x00000000- 0x1FFFFFFF Code             Normal
       0x00000000- 0x003FFFFF ITCM
       0x00400000- 0x005FFFFF Internal flash   Normal          Not shareable   WB
   2   0x20000000- 0x3FFFFFFF SRAM             Normal
       0x20000000- 0x203FFFFF DTCM
       0x20400000- 0x2043FFFF First Partition  Normal          Not shareable   WB
 if MPU_HAS_NOCACHE_REGION is defined
       0x20440000- 0x2045EFFF Second Partition Normal          Not shareable   WB
       0x2045F000- 0x2045FFFF Nocache SRAM     Normal          Shareable
 if MPU_HAS_NOCACHE_REGION is NOT defined
       0x20440000- 0x2045FFFF Second Partition Normal          Not shareable   WB
   3   0x40000000- 0x5FFFFFFF Peripheral       Device          Shareable
   4   0x60000000- 0x7FFFFFFF RAM
       0x60000000- 0x6FFFFFFF External EBI  Strongly-ordered   Shareable
       0x70000000- 0x7FFFFFFF SDRAM            Normal          Shareable       WBWA
   5   0x80000000- 0x9FFFFFFF QSPI          Strongly-ordered   Shareable
   6   0xA0100000- 0xA01FFFFF USBHS RAM        Device          Shareable
   7   0xE0000000- 0xFFFFFFFF System           -                  -
   */

/**
 * \brief Set up a memory region.
 */
void _SetupMemoryRegion(void)
{

	uint32_t dwRegionBaseAddr;
	uint32_t dwRegionAttr;

	memory_barrier();

	/***************************************************
	    ITCM memory region --- Normal
	    START_Addr:-  0x00000000UL
	    END_Addr:-    0x003FFFFFUL
	****************************************************/
	dwRegionBaseAddr =
		ITCM_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_ITCM_REGION;        // 1

	dwRegionAttr =
		MPU_AP_PRIVILEGED_READ_WRITE |
		MPU_CalMPURegionSize(ITCM_END_ADDRESS - ITCM_START_ADDRESS) |
		MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);

	/****************************************************
	    Internal flash memory region --- Normal read-only
	    (update to Strongly ordered in write accesses)
	    START_Addr:-  0x00400000UL
	    END_Addr:-    0x005FFFFFUL
	******************************************************/

     
    #define IFLASH_UNUSED_AREA_START_ADDRESS 0x004E0000
    #define IFLASH_RO_PARAMETERS_START_ADDRESS 0x004FC000
    #define IFLASH_RO_PARAMETERS_END_ADDRESS 0x00500000-1
    #define IFLASH_RW_PARAMETERS_START_ADDRESS 0x004F8000
    
    uint32_t flash_size = xtio_get_flash_size();
	
    //use overlapping regions..
    //first region maps whole flash as RO
	dwRegionBaseAddr =
		IFLASH_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_IFLASH_REGION;      //2

	dwRegionAttr =
		MPU_AP_READONLY |
		INNER_NORMAL_WB_NWA_TYPE(NON_SHAREABLE) |
		MPU_CalMPURegionSize(flash_size) |
		MPU_REGION_ENABLE;
    

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);

	uint32_t flash_app_start = 0x004E0000;
	uint32_t flash_app_end = 0x004F8000;
	uint32_t flash_rw_start = 0x004F8000;
	uint32_t flash_ro_start = 0x004FC000;
	uint32_t flash_ro_end = IFLASH_ADDR + 1*1024*1024;
    if (flash_size == (2*1024*1024))
	{
		flash_app_start = 0x005B0000;
		flash_app_end = 0x005F8000;
		flash_rw_start = 0x005F8000;
		flash_ro_start = 0x005FC000;
		flash_ro_end = IFLASH_ADDR + 2*1024*1024;
    
		// Application data flash region
		//second region maps from start of unused effectivly untill end of RO as MPU region must be 2^x
		dwRegionBaseAddr =
			0x5B0000 |
			MPU_REGION_VALID |
			14;      
		dwRegionAttr =
			MPU_AP_FULL_ACCESS |
			STRONGLY_ORDERED_SHAREABLE_TYPE |
			MPU_CalMPURegionSize(128*1024) |
			MPU_REGION_ENABLE;
		
		MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);

		// RW Param flash region
		//second region maps from start of unused effectivly untill end of RO as MPU region must be 2^x
		dwRegionBaseAddr =
			0x5D0000 |
			MPU_REGION_VALID |
			12;      
		dwRegionAttr =
			MPU_AP_FULL_ACCESS |
			STRONGLY_ORDERED_SHAREABLE_TYPE |
			MPU_CalMPURegionSize(256*1024) |
			MPU_REGION_ENABLE;
		
		MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);
		
		
		// RO parameters this region only needs to cover RO param section.
		//
		dwRegionBaseAddr =
			flash_ro_start |
			MPU_REGION_VALID |
			15;

		dwRegionAttr =
			MPU_AP_READONLY |
			INNER_NORMAL_WB_NWA_TYPE(NON_SHAREABLE) |
			MPU_CalMPURegionSize(flash_ro_end - flash_ro_start)|
			MPU_REGION_ENABLE;
		

		MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);
	} else 
	{
		// Application data flash region
		//second region maps from start of unused effectivly untill end of RO as MPU region must be 2^x
		dwRegionBaseAddr =
			flash_app_start |
			MPU_REGION_VALID |
			14;      
		dwRegionAttr =
			MPU_AP_FULL_ACCESS |
			STRONGLY_ORDERED_SHAREABLE_TYPE |
			(MPU_CalMPURegionSize(flash_app_end - flash_app_start)) |
			MPU_REGION_ENABLE;
		
		MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);

		// RW Param flash region
		//second region maps from start of unused effectivly untill end of RO as MPU region must be 2^x
		dwRegionBaseAddr =
			flash_rw_start |
			MPU_REGION_VALID |
			12;      
		dwRegionAttr =
			MPU_AP_FULL_ACCESS |
			STRONGLY_ORDERED_SHAREABLE_TYPE |
			MPU_CalMPURegionSize(flash_ro_start - flash_rw_start) |
			MPU_REGION_ENABLE;
		
		MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);
		
		
		// RO parameters this region only needs to cover RO param section.
		//
		dwRegionBaseAddr =
			flash_ro_start |
			MPU_REGION_VALID |
			15;

		dwRegionAttr =
			MPU_AP_READONLY |
			INNER_NORMAL_WB_NWA_TYPE(NON_SHAREABLE) |
			MPU_CalMPURegionSize(flash_ro_end - flash_ro_start)|
			MPU_REGION_ENABLE;
		

		MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);
	}
        

	/****************************************************
	    DTCM memory region --- Normal
	    START_Addr:-  0x20000000L
	    END_Addr:-    0x203FFFFFUL
	******************************************************/

	/* DTCM memory region */
	dwRegionBaseAddr =
		DTCM_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_DTCM_REGION;         //3

	dwRegionAttr =
		MPU_AP_FULL_ACCESS    |
		INNER_NORMAL_WB_NWA_TYPE(NON_SHAREABLE) |
		MPU_CalMPURegionSize(DTCM_END_ADDRESS - DTCM_START_ADDRESS) |
		MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);

	/****************************************************
	    SRAM Cacheable memory region --- Normal
	    START_Addr:-  0x20400000UL
	    END_Addr:-    0x2043FFFFUL
	******************************************************/
	/* SRAM memory  region */
	dwRegionBaseAddr =
		SRAM_FIRST_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_SRAM_REGION_1;         //4

	dwRegionAttr =
		MPU_AP_FULL_ACCESS    |
		INNER_NORMAL_WB_NWA_TYPE(NON_SHAREABLE) |
		MPU_CalMPURegionSize(SRAM_FIRST_END_ADDRESS - SRAM_FIRST_START_ADDRESS)
		| MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);


	/****************************************************
	    Internal SRAM second partition memory region --- Normal
	    START_Addr:-  0x20440000UL
	    END_Addr:-    0x2045FFFFUL
	******************************************************/
	/* SRAM memory region */
	dwRegionBaseAddr =
		SRAM_SECOND_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_SRAM_REGION_2;         //5

	dwRegionAttr =
		MPU_AP_FULL_ACCESS    |
		INNER_NORMAL_WB_NWA_TYPE(NON_SHAREABLE) |
		MPU_CalMPURegionSize(SRAM_SECOND_END_ADDRESS - SRAM_SECOND_START_ADDRESS) |
		MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);

#ifdef MPU_HAS_NOCACHE_REGION
	dwRegionBaseAddr =
		SRAM_NOCACHE_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_NOCACHE_SRAM_REGION;          //11

	dwRegionAttr =
		MPU_AP_FULL_ACCESS    |
		INNER_OUTER_NORMAL_NOCACHE_TYPE(SHAREABLE) |
		MPU_CalMPURegionSize(NOCACHE_SRAM_REGION_SIZE) |
		MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);
#endif

	/****************************************************
	    Peripheral memory region --- DEVICE Shareable
	    START_Addr:-  0x40000000UL
	    END_Addr:-    0x5FFFFFFFUL
	******************************************************/
	dwRegionBaseAddr =
		PERIPHERALS_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_PERIPHERALS_REGION;          //6

	dwRegionAttr = MPU_AP_FULL_ACCESS |
				   MPU_REGION_EXECUTE_NEVER |
				   SHAREABLE_DEVICE_TYPE |
				   MPU_CalMPURegionSize(PERIPHERALS_END_ADDRESS - PERIPHERALS_START_ADDRESS)
				   | MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);


	/****************************************************
	    External EBI memory  memory region --- Strongly Ordered
	    START_Addr:-  0x60000000UL
	    END_Addr:-    0x6FFFFFFFUL
	******************************************************/
	dwRegionBaseAddr =
		EXT_EBI_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_EXT_EBI_REGION;

	dwRegionAttr =
		MPU_AP_FULL_ACCESS |
		/* External memory Must be defined with 'Device' or 'Strongly Ordered'
		attribute for write accesses (AXI) */
		STRONGLY_ORDERED_SHAREABLE_TYPE |
		MPU_CalMPURegionSize(EXT_EBI_END_ADDRESS - EXT_EBI_START_ADDRESS) |
		MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);

	/****************************************************
	    SDRAM Cacheable memory region --- Normal
	    START_Addr:-  0x70000000UL
	    END_Addr:-    0x7FFFFFFFUL
	******************************************************/
	dwRegionBaseAddr =
		SDRAM_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_SDRAM_REGION;        //7

	dwRegionAttr =
		MPU_AP_FULL_ACCESS    |
		INNER_NORMAL_WB_RWA_TYPE(SHAREABLE) |
		MPU_CalMPURegionSize(SDRAM_END_ADDRESS - SDRAM_START_ADDRESS) |
		MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);

	/****************************************************
	    QSPI memory region --- Strongly ordered
	    START_Addr:-  0x80000000UL
	    END_Addr:-    0x9FFFFFFFUL
	******************************************************/
	dwRegionBaseAddr =
		QSPI_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_QSPIMEM_REGION;              //8

	dwRegionAttr =
		MPU_AP_FULL_ACCESS |
		MPU_REGION_EXECUTE_NEVER |
		STRONGLY_ORDERED_SHAREABLE_TYPE |
		MPU_CalMPURegionSize(QSPI_END_ADDRESS - QSPI_START_ADDRESS) |
		MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);


	/****************************************************
	    USB RAM Memory region --- Device
	    START_Addr:-  0xA0100000UL
	    END_Addr:-    0xA01FFFFFUL
	******************************************************/
	dwRegionBaseAddr =
		USBHSRAM_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_USBHSRAM_REGION;              //9

	dwRegionAttr =
		MPU_AP_FULL_ACCESS |
		MPU_REGION_EXECUTE_NEVER |
		SHAREABLE_DEVICE_TYPE |
		MPU_CalMPURegionSize(USBHSRAM_END_ADDRESS - USBHSRAM_START_ADDRESS) |
		MPU_REGION_ENABLE;

	MPU_SetRegion(dwRegionBaseAddr, dwRegionAttr);


	/* Enable the memory management fault , Bus Fault, Usage Fault exception */
	SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk
				   | SCB_SHCSR_USGFAULTENA_Msk);

	/* Enable the MPU region */
	MPU_Enable(MPU_ENABLE | MPU_PRIVDEFENA);

	memory_sync();
}

#ifdef ENABLE_TCM

#if defined (__ICCARM__) /* IAR Ewarm */
	#pragma section = "CSTACK"
	#pragma section = "CSTACK_DTCM"
	#define SRAM_STACK_BASE     (__section_begin("CSTACK"))
	#define DTCM_STACK_BASE     (__section_begin("CSTACK_DTCM"))
	#define SRAM_STACK_LIMIT    (__section_end("CSTACK"))
	#define DTCM_STACK_LIMIT    (__section_end("CSTACK_DTCM"))
#elif defined (__CC_ARM)  /* MDK */
	extern uint32_t Image$$ARM_LIB_STACK$$Base;
	extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Limit;
	extern uint32_t Image$$DTCM_STACK$$Base;
	extern uint32_t Image$$DTCM_STACK$$ZI$$Limit;
	#define SRAM_STACK_BASE     (&Image$$ARM_LIB_STACK$$Base)
	#define DTCM_STACK_BASE     (&Image$$DTCM_STACK$$Base)
	#define SRAM_STACK_LIMIT    (&Image$$ARM_LIB_STACK$$ZI$$Limit)
	#define DTCM_STACK_LIMIT    (&Image$$DTCM_STACK$$ZI$$Limit)
#elif defined (__GNUC__)  /* GCC */
	extern uint32_t _sdtcm_stack, _edtcm_stack, _sstack, _estack;
	#define SRAM_STACK_BASE     ((void *)(&_sstack))
	#define DTCM_STACK_BASE     ((void *)(&_sdtcm_stack))
	#define SRAM_STACK_LIMIT    ((void *)(&_estack))
	#define DTCM_STACK_LIMIT    ((void *)(&_edtcm_stack))
#endif

/** \brief  Change stack's location to DTCM

    The function changes the stack's location from SRAM to DTCM
 */
void TCM_StackInit(void);
void TCM_StackInit(void)
{
	uint32_t offset = (uint32_t)SRAM_STACK_LIMIT - (uint32_t)DTCM_STACK_LIMIT;
	volatile char *dst = (volatile char *)DTCM_STACK_LIMIT;
	volatile char *src = (volatile char *)SRAM_STACK_LIMIT;

	/* copy stack data from SRAM to DTCM */
	while (src > (volatile char *)SRAM_STACK_BASE)
		*--dst = *--src;

	__set_MSP(__get_MSP() - offset);
}

#endif


/**
 * \brief Performs the low-level initialization of the chip.
 */
__attribute__ ((optimize("-O0"))) extern WEAK void LowLevelInit(void)
{
    int status = 0;

    xt_config_init();

	SystemInit();
#ifndef MPU_EXAMPLE_FEATURE
	_SetupMemoryRegion();
#endif

	/* Enabling the FPU */
	SCB->CPACR |= 0x00F00000;
	__DSB();
	__ISB();

#if defined(ENABLE_TCM) && defined(__GNUC__)
	volatile char *dst = &_sitcm;
	volatile char *src = &_itcm_lma;

	/* copy code_TCM from flash to ITCM */
	while (dst < &_eitcm)
		*dst++ = *src++;

	dst = &_sdtcm_data;
	src = &_dtcm_data_lma;

	/* copy data_TCM from flash to DTCM */
	while (dst < &_edtcm_data)
		*dst++ = *src++;

#endif

    PMC_EnablePeripheral(ID_PIOA);
    PMC_EnablePeripheral(ID_PIOB);
    PMC_EnablePeripheral(ID_PIOD);

    status = xtio_led_init(XTIO_LED_RED, XTIO_LED_ONOFF, XTIO_LED_OFF);
    status = xtio_led_init(XTIO_LED_GREEN, XTIO_LED_ONOFF, XTIO_LED_OFF);
    status = xtio_led_init(XTIO_LED_BLUE, XTIO_LED_ONOFF, XTIO_LED_OFF);

	xt_get_reset_count();
	
	// WDT will trigger interrupt to store crash dump (~8sec)
	WDT_Enable(WDT, WDT_MR_WDV(0x7FF) | WDT_MR_WDFIEN | WDT_MR_WDD(0x7FF) | WDT_MR_WDDBGHLT);
	NVIC_SetPriority(WDT_IRQn, 0);
	NVIC_EnableIRQ(WDT_IRQn);

	// Reinforced WDT will trigger reset (~16sec)
	WDT_Enable(RSWDT, WDT_MR_WDV(0xFFF) | WDT_MR_WDRSTEN | WDT_MR_WDD(0xFFF) |WDT_MR_WDDBGHLT);

	// Enable timer and clock gate pin
	PIOA->PIO_PER = (1<<29);
	PIOA->PIO_OER = (1<<29);
	PIOA->PIO_SODR = (1<<29);
	
	PMC->PMC_SCER = (1<<14); // PCK6 = slow clock / 2 = 32768 / 2 = 16kHz
	PMC->PMC_PCK6 = (1<<4)|(0<<0);
	PMC_EnablePeripheral(ID_TC0);
	PMC_EnablePeripheral(ID_TC1);
	TC0->TC_CHANNEL[0].TC_CCR = (1<<0)|(1<<2); // Enable clock and trigger
	TC0->TC_CHANNEL[0].TC_CMR = (0<<0)|(3<<4); // PCK6 as clock source and gated by XC2
	
	TC0->TC_CHANNEL[1].TC_CCR = (1<<0)|(1<<2); // Enable clock and trigger
	TC0->TC_CHANNEL[1].TC_CMR = (0<<0); // PCK6 as clock source, no gating
	TC0->TC_CHANNEL[1].TC_IER = (1<<4); // RA compare match interrupt enabled
	TC0->TC_CHANNEL[1].TC_RC = 64000; // Set RA value
	
	NVIC_SetPriority(TC0_IRQn, 0);
	NVIC_EnableIRQ(TC0_IRQn);
	NVIC_SetPriority(TC1_IRQn, 0);
	NVIC_EnableIRQ(TC1_IRQn);

	TC0->TC_BCR = (1<<0); // Resets and triggers all channels

	/* Initialize PIO interrupts */
	PIO_InitializeInterrupts(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);

	/* Driver initialize */
	dmad.irqPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
	XDMAD_Initialize(&dmad, 0);
}
