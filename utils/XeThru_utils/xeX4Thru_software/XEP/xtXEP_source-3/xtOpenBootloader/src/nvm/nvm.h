/**
 * @file
 *
 * @brief Non-volatile memory access functions
 *
 */
#ifndef NVM_H_INCLUDED
#define NVM_H_INCLUDED


#include <stdint.h>

/**
 * @brief Initializes Non-volatile memory system
 */
void nvm_init(void);
/**
 * @brief Erase a set of pages.
 */
uint32_t nvm_erase_pages(
    uint16_t page           ///< page number
);
/**
 * @brief Erase a flash page.
 */
uint32_t nvm_erase_page(
    uint16_t page           ///< page number
);
/**
 * @brief Writes a flash memory page
 */
uint32_t nvm_write_page(
    uint16_t page,           ///< page number
    const void *buffer       ///< pointer to the buffer
);
/**
 * 
 */
uint32_t nvm_get_unique_id(
    uint8_t* buffer,        ///< Pointer to buffer
    uint32_t max_size,      ///< Size of buffer
    uint32_t* size          ///< Pointer to size count
);
/**
 * @brief Locks flash memory
 */
void nvm_lock(void);
/**
 * @brief Locks part of the flash memory from 0 to last_page
 */
void nvm_lock_pages(
    uint16_t last_page       ///< number of a last page to lock
);
/**
 * @brief Checks if memory is locked
 */
bool nvm_is_locked(void);
/**
 * @brief Unlocks flash memory
 */
void nvm_unlock(void);
/**
 * @brief Executes application from flash memory
 */
void nvm_exec_app(
    void *start              ///< address of the application to run
);
#endif // NVM_H_INCLUDED
