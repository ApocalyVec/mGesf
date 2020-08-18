/**
 * @file
 *
 * @brief Non-volatile memory layout description
 *
 */
#include "nvm_sections.h"
#include <compiler.h>
#include <config/conf_nvm.h>

 
 const MemorySections_t memory_sections = {	
	 .AppFirstPage		= (0x00410000-IFLASH_ADDR) / IFLASH_PAGE_SIZE,
	 .AppLastPage		= (0x005B0000-IFLASH_ADDR) / IFLASH_PAGE_SIZE,
	 .AppStart			= 0x00410000,
	 .AppEnd			= 0x005B0000,
	 .AppStorageStart	= 0x005B0000,
	 .AppStorageEnd		= 0x005F0000,
};


