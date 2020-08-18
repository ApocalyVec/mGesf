/**
 * @file
 *
 * @brief Local header file for xtio FLASH functions.
 */

#ifndef XTIO_FLASH_H
#define  XTIO_FLASH_H

#include "xep_hal.h"

uint32_t xtio_flash_store_hw_data(uint8_t * buffer, uint32_t size);
uint32_t xtio_flash_read_hw_data(uint8_t* buffer, uint32_t max_size, uint32_t* size);

#endif //  XTIO_FLASH_H
