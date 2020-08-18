/**
 * @file
 *
 * @brief Non-volatile memory access functions
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <asf.h>
#include "conf_nvm.h"
#include <nvm/nvm.h>


#define  NVM_PAGES_PER_LOCK_REGION			(IFLASH_LOCK_REGION_SIZE / IFLASH_PAGE_SIZE)


static uint32_t (*iap_exec)(uint32_t id, uint32_t cmd);

static uint32_t calc_address(uint16_t _page)
{
	uint32_t ret = _page;
	ret *= (uint32_t)IFLASH_PAGE_SIZE;
	ret += IFLASH_ADDR;

	return ret;
}

__no_inline
RAMFUNC
static void word_cpy(uint32_t *dst, uint32_t *src)
{
	for (uint16_t i = 0; i < IFLASH_PAGE_SIZE/sizeof(uint32_t); ++i) {
		*dst++ = *src++;
		__DMB();
	}
}

static uint32_t flash_do_iap(uint8_t cmd, uint16_t page)
{
	uint32_t status = 0;
	/*Barrier*/
	__DSB();
	__ISB();
	__disable_irq();
	__DMB();

	// Start Flash erase and wait for completion
	status = iap_exec(0, EEFC_FCR_FKEY_PASSWD
	| EEFC_FCR_FARG(page)
	| cmd);

	__DMB();
	
	/*Barrier*/
	__DSB();
	__ISB();
	__enable_irq();
	
	return status;
}

void nvm_init(void)
{
	iap_exec = (uint32_t (*)( uint32_t, uint32_t ))
	*((uint32_t*)(IROM_ADDR + 8) ) ;

	//Configure flash
	uint32_t status;

	EFC->EEFC_FCR = EEFC_FMR_FWS(6) | EEFC_FMR_CLOE;
	do {
		status = EFC->EEFC_FSR;
	} while ((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);
}

/**
 * Erase a set of pages.
 */
uint32_t nvm_erase_pages(uint16_t page)
{	
	//EPA COMMAND FARG[0:1] sets number of pages to erase
	page |= EPA_0_1_CONFIGRATION;
	return flash_do_iap(EEFC_FCR_FCMD_EPA, page);
}



uint32_t nvm_erase_page(uint16_t page)
{
	uint32_t page_address = calc_address(page);
	uint32_t *dst = (uint32_t*)page_address;
	
	for (uint16_t i = 0; i < IFLASH_PAGE_SIZE / 4; ++i) {
		*dst++ = 0xFFFFffff;
	}
	
	return flash_do_iap(EEFC_FCR_FCMD_WP, page);
}


uint32_t nvm_write_page(uint16_t page, const void *buffer)
{
	uint32_t page_address = calc_address(page);
	uint32_t *dst = (uint32_t *)page_address;
	uint32_t *src = (uint32_t *)buffer;

	word_cpy(dst, src);

	return flash_do_iap(EEFC_FCR_FCMD_WP, page);
}

__attribute__ ((section (".ramfunc")))
uint32_t nvm_get_unique_id(uint8_t* buffer, uint32_t max_size, uint32_t* size)
{
	if (max_size < 16) 
		return 1;
	
	irqflags_t flags = cpu_irq_save();

	uint32_t status;
	
	// Send the Start Read unique Identifier command (STUI) by writing the Flash Command Register with the STUI command.
	EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EFC_FCMD_STUI;

	// When the Unique Identifier is ready to be read, the FRDY bit in the Flash Programming Status Register (EEFC_FSR) falls.
	do {
		status = EFC->EEFC_FSR;
	} while ((status & EEFC_FSR_FRDY) == EEFC_FSR_FRDY);

	uint32_t i = 0;
	uint8_t* aligned_addr = (uint8_t*)0x20000;
	while ((i < max_size) && (i < 16)) // 128bit  Unique Identifier Area
	{
		// Read bytes, alreday in ascii format.
		buffer[i] = aligned_addr[i];
		if (buffer[i]==0) buffer[i] = '0'; // First byte 0, giving empty string.
		i++;
	}
	*size = i;

	// To stop the Unique Identifier mode, the user needs to send the Stop Read unique Identifier command (SPUI) by writing the Flash Command Register with the SPUI command.
	EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EFC_FCMD_SPUI;

	// When the Stop read Unique Unique Identifier command (SPUI) has been performed, the FRDY bit in the Flash Programming Status Register (EEFC_FSR) rises.
	do {
		status = EFC->EEFC_FSR;
	} while ((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);

	uint32_t dwError = (status & (EEFC_FSR_FLOCKE | EEFC_FSR_FCMDE | EEFC_FSR_FLERR));

	cpu_irq_restore(flags);

	if (dwError)
		return 1;
	
	return 0;
}

void nvm_lock(void)
{
    for (uint16_t page = 0; page < IFLASH_NB_OF_PAGES; page += NVM_PAGES_PER_LOCK_REGION) {
        flash_do_iap(EFC_FCMD_SLB, page);
    }
}

void nvm_lock_pages(uint16_t last_page)
{
	for (uint16_t page = 0; page <= last_page; page += NVM_PAGES_PER_LOCK_REGION) {
		flash_do_iap(EFC_FCMD_SLB, page);
	}
}


bool nvm_is_locked(void)
{
    const uint32_t ignored = 0;
    flash_do_iap(EFC_FCMD_GLB, ignored);
    /* read result */
    uint32_t volatile status = EFC->EEFC_FRR;
    /* mask result */
    const uint32_t result = status & (1 << APP_FIRST_LOCK);
    return result != 0;
}


void nvm_unlock(void)
{
    for (uint16_t page = APP_FIRST_PAGE; page < APP_LAST_PAGE_UNLOCK; page += NVM_PAGES_PER_LOCK_REGION) {
        flash_do_iap(EFC_FCMD_CLB, page);
    }
}


void nvm_exec_app(void *start)
{
    int i;
	sysclk_set_source(SYSCLK_SRC_MAINCK_12M_RC);
    // -- Disable interrupts
    // Disable IRQ
    __disable_irq();
    // Disable IRQs
    for (i = 0; i < 8; i ++) NVIC->ICER[i] = 0xFFFFFFFF;
    // Clear pending IRQs
    for (i = 0; i < 8; i ++) NVIC->ICPR[i] = 0xFFFFFFFF;

    // -- Modify vector table location
    // Barriars
    __DSB();
    __ISB();
    // Change the vector table
    SCB->VTOR = ((uint32_t)start & SCB_VTOR_TBLOFF_Msk);
    // Barriars
    __DSB();
    __ISB();

    // -- Enable interrupts
    //__enable_irq();

    // -- Load Stack & PC
   __asm__ ("mov   r1, %[Rm]        \n\t\
             ldr   r0, [r1, #4]  \n\t\
             ldr   sp, [r1]      \n\t\
             blx   r0\n\t" ::[Rm] "r" (start));
}
