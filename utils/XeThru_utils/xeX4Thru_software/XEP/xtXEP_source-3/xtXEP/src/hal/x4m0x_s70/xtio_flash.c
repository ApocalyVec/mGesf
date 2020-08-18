/**
 * @file
 *
 * @brief Implementation of flash functions for X4M0x boards with Atmel SAM S70 MCU.
 *
 * See @ref X4M0x_SAMS70/xtio_flash.h and @ref xt_XEP_HAL.h for more documentation.
 *
 * @todo Implement PWM mode for LEDs
 */

#include "xtmemory.h"
#include "xtsemaphore.h"
#include "xtio_flash.h"
#include "board.h"
#include <FreeRTOS.h>
#include <task.h>

#include "flashd.h"

#define DISABLE_XTIO_FLASH_ON_NOEXTRAM

#define FLASH_PAGE_SIZE     512
#define FLASH_BLOCK_SIZE    16

static uint8_t *flash_buffer = NULL;

uint8_t *flash_addr = (uint8_t *)0;

void *flash_lock_object = NULL;

volatile uint32_t flash_systick_stalls = 0;

void flash_in_progress_callback(void);

__attribute__((section(".ramfunc"), noinline)) void flash_in_progress_callback(void)
{
    if (SysTick->CTRL & (1 << 16))
        flash_systick_stalls++;
}

/**
 * @brief Erases specified flash area. Must be aligned to erase size. 
 * 
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
__attribute__((section(".ramfunc"), noinline))
uint32_t
xtio_flash_erase(uint32_t address, uint32_t size)
{
    if (flash_lock_object == NULL)
        flash_lock_object = xtlock_create();

    if (xtlock_lock(flash_lock_object, 5000 / portTICK_PERIOD_MS) != XTLOCK_OK)
        return XT_ERROR;

    if (((address % (FLASH_PAGE_SIZE * FLASH_BLOCK_SIZE)) != 0) ||
        ((size % (FLASH_PAGE_SIZE * FLASH_BLOCK_SIZE)) != 0))
    {
        xtlock_unlock(flash_lock_object);
        return XT_ERROR;
    }

    uint32_t status = 0;
    for (int page = 0; page*FLASH_PAGE_SIZE < size; page += FLASH_BLOCK_SIZE)
    {
        status = FLASHD_ErasePages(address + (page*FLASH_PAGE_SIZE), FLASH_BLOCK_SIZE, flash_in_progress_callback);
        
        while (flash_systick_stalls)
        {
            SysTick_Handler();
            flash_systick_stalls--;
        }

        if (status != 0)
        {
            xtlock_unlock(flash_lock_object);
            return XT_ERROR;
        }
    }
        
    xtlock_unlock(flash_lock_object);
    return XT_SUCCESS;
}


/**
 * @brief Writes data to flash. User must make sure to call erase prior to writing.  
 * 
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
__attribute__((section(".ramfunc"), noinline))
uint32_t
xtio_flash_write(uint32_t address, uint8_t* data, uint32_t size)
{
    if (flash_lock_object == NULL)
        flash_lock_object = xtlock_create();

    if (xtlock_lock(flash_lock_object, 5000 / portTICK_PERIOD_MS) != XTLOCK_OK)
        return XT_ERROR;
    
    // Write data to flash. Alignment is handled by callee. 
    uint32_t status = FLASHD_Write(address, data, size, flash_in_progress_callback);

    while (flash_systick_stalls)
    {
        SysTick_Handler();
        flash_systick_stalls--;
    }

    if (status != 0)
    {
        xtlock_unlock(flash_lock_object);
        return XT_ERROR;
    }
        
    xtlock_unlock(flash_lock_object);
    return XT_SUCCESS;
}


/**
 * @brief stores data in flash.
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
__attribute__((section(".ramfunc"), noinline))
uint32_t
xtio_flash_store(uint32_t destination_address, uint8_t *buffer, uint32_t size)
{
#ifdef DISABLE_XTIO_FLASH_ON_NOEXTRAM
    if (!xt_external_memory_enabled())
        return XT_ERROR;
#endif

    if (!flash_buffer)
        flash_buffer = xtmemory_malloc_default(16 * 512);

    if (flash_lock_object == NULL)
        flash_lock_object = xtlock_create();

    if (xtlock_lock(flash_lock_object, 5000 / portTICK_PERIOD_MS) != XTLOCK_OK)
        return XT_ERROR;

    uint32_t page_size = 512;
    uint32_t pages_per_erase = 16;
    uint32_t pages = size / page_size;
    uint32_t pages_left = size % page_size;
    if (pages_left)
        pages = pages + 1;
    uint32_t status = 0;

    // Calculate section that will be erased
    uint32_t first_erase_address = destination_address & ~((pages_per_erase * page_size) - 1);
    uint32_t last_erase_address = (destination_address + size);
    if (last_erase_address % (pages_per_erase * page_size))
        last_erase_address += (pages_per_erase * page_size);
    last_erase_address &= ~((pages_per_erase * page_size) - 1);

    uint32_t erase_size = last_erase_address - first_erase_address;

    uint32_t data_start_offset = destination_address - first_erase_address;

    for (int offset = 0; offset < erase_size; offset += (pages_per_erase * page_size))
    {
        // Copy flash area to erase
        for (uint32_t i = 0; i < ((pages_per_erase * page_size) / sizeof(uint32_t)); i++)
            ((uint32_t *)flash_buffer)[i] = ((uint32_t *)(first_erase_address + offset))[i];

        // Calculate offset of data
        uint32_t data_write_offset = data_start_offset;
        if (offset > data_start_offset)
            data_write_offset = 0;
        uint32_t data_read_offset = 0;
        if (offset > 0)
            data_read_offset = offset - data_start_offset;
        // Copy data to write
        uint32_t i = 0;
        while ((((data_write_offset + i) != (pages_per_erase * page_size)) &&
                ((data_read_offset + i) < size)))
        {
            flash_buffer[data_write_offset + i] = buffer[data_read_offset + i];
            i++;
        }

        // Perform erase of flash
        uint32_t address = first_erase_address + offset;

        status = FLASHD_ErasePages(address, pages_per_erase, flash_in_progress_callback);

        while (flash_systick_stalls)
        {
            SysTick_Handler();
            flash_systick_stalls--;
        }

        if (status != 0)
        {
            xtlock_unlock(flash_lock_object);
            return XT_ERROR;
        }

        // Write data
        status = FLASHD_Write(address, flash_buffer, (pages_per_erase * page_size), flash_in_progress_callback);

        while (flash_systick_stalls)
        {
            SysTick_Handler();
            flash_systick_stalls--;
        }

        if (status != 0)
        {
            xtlock_unlock(flash_lock_object);
            return XT_ERROR;
        }
    }

    xtlock_unlock(flash_lock_object);
    return XT_SUCCESS;
}

uint32_t xtio_get_page_size()
{
    return IFLASH_PAGE_SIZE;
}

uint32_t xtio_get_block_size()
{
    return IFLASH_LOCK_REGION_SIZE;
}

uint32_t xtio_get_flash_size(void)
{
    if ((CHIPID->CHIPID_CIDR & CHIPID_CIDR_NVPSIZ_Msk) == CHIPID_CIDR_NVPSIZ_2048K)
    {
        return 2 * 1024 * 1024;
    }

    return 1 * 1024 * 1024;
}

uint32_t xtio_flash_store_hw_data(uint8_t *buffer, uint32_t size)
{
    irqflags_t flags = cpu_irq_save();

    if (size > IFLASH_PAGE_SIZE)
    {
        cpu_irq_restore(flags);
        return XT_ERROR;
    }

    uint32_t dwError;
    dwError = EFC_PerformCommand(EFC, EFC_FCMD_EUS, 0, 0, NULL);

    uint32_t *aligned_addr = (uint32_t *)0x20000;
    uint32_t i = 0;
    while (i < (size / sizeof(uint32_t)))
    {
        aligned_addr[i] = ((uint32_t *)buffer)[i];
        i++;
    }

    uint8_t remainder = size % sizeof(uint32_t);
    if (remainder)
    {
        uint32_t padded_value = 0xFFFFFFFF;
        for (int j = 0; j < remainder; j++)
        {
            padded_value &= ~(0xFF << (8 * j));
            padded_value |= (buffer[(i * 4) + j] << (8 * j));
        }
        aligned_addr[i++] = padded_value;
    }

    while (i < IFLASH_PAGE_SIZE / sizeof(uint32_t))
        aligned_addr[i++] = 0xFFFFFFFF;

    /* Cache coherence operation before flash write*/
    SCB_CleanDCache_by_Addr((uint32_t *)aligned_addr, IFLASH_PAGE_SIZE);

    dwError |= EFC_PerformCommand(EFC, EFC_FCMD_WUS, 0, 0, NULL);

    cpu_irq_restore(flags);

    if (dwError)
        return XT_ERROR;

    return XT_SUCCESS;
}

__attribute__((section(".ramfunc"), noinline))
uint32_t
xtio_flash_read_hw_data(uint8_t *buffer, uint32_t max_size, uint32_t *size)
{
    irqflags_t flags = cpu_irq_save();

    uint32_t dwError;
    dwError = EFC_PerformCommand(EFC, EFC_FCMD_STUS, 0, 0, NULL);
    if (dwError)
    {
        cpu_irq_restore(flags);
        return XT_ERROR;
    }

    uint32_t i = 0;
    uint8_t *aligned_addr = (uint8_t *)0x20000;
    while ((i < max_size) && (i < IFLASH_PAGE_SIZE))
    {
        buffer[i] = aligned_addr[i];
        i++;
    }
    *size = i;

    dwError = EFC_PerformCommand(EFC, EFC_FCMD_SPUS, 0, 0, NULL);

    cpu_irq_restore(flags);

    if (dwError)
        return XT_ERROR;

    return XT_SUCCESS;
}

__attribute__((section(".ramfunc"), noinline))
uint32_t
xtio_flash_get_unique_id(uint8_t *buffer, uint32_t max_size, uint32_t *size)
{
    if (max_size < 16)
        return XT_ERROR;

    irqflags_t flags = cpu_irq_save();

    // OJE comment: Using EFC_PerformCommand(EFC, EFC_FCMD_STUI, 0, 0) fails due to incorrect wait for ready.
    // Had to reimplement the same method here.

    uint32_t status;

    // Send the Start Read unique Identifier command (STUI) by writing the Flash Command Register with the STUI command.
    EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EFC_FCMD_STUI;

    // When the Unique Identifier is ready to be read, the FRDY bit in the Flash Programming Status Register (EEFC_FSR) falls.
    do
    {
        status = EFC->EEFC_FSR;
    } while ((status & EEFC_FSR_FRDY) == EEFC_FSR_FRDY);

    uint32_t i = 0;
    uint8_t *aligned_addr = (uint8_t *)0x20000;
    while ((i < max_size) && (i < 16)) // 128bit  Unique Identifier Area
    {
        // Read bytes, alreday in ascii format.
        buffer[i] = aligned_addr[i];
        if (buffer[i] == 0)
            buffer[i] = '0'; // First byte 0, giving empty string.
        i++;
    }
    *size = i;

    // To stop the Unique Identifier mode, the user needs to send the Stop Read unique Identifier command (SPUI) by writing the Flash Command Register with the SPUI command.
    EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EFC_FCMD_SPUI;

    // When the Stop read Unique Unique Identifier command (SPUI) has been performed, the FRDY bit in the Flash Programming Status Register (EEFC_FSR) rises.
    do
    {
        status = EFC->EEFC_FSR;
    } while ((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);

    uint32_t dwError = (status & (EEFC_FSR_FLOCKE | EEFC_FSR_FCMDE | EEFC_FSR_FLERR));

    cpu_irq_restore(flags);

    if (dwError)
        return XT_ERROR;

    return XT_SUCCESS;
}
