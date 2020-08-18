/**
 * @file
 *
 * @brief Local header file for Radar HAL.
 */

#ifndef RADAR_HAL_H
#define RADAR_HAL_H

#include <stdint.h>
//#include "xep_hal.h"
//#include "xtio_spi.h"

#define X4_ENABLE_PIN 5
#define X4_GPIO_INT 6
#define SPI_CHANNEL 0


/**
 * Structure to hold information about the current radar.
 */
typedef struct {
	int radar_id;                   ///< Id of current radar
	//xtio_spi_handle_t * spi_handle; ///< Pointer to spi handle used to communicate with the radar IC
}radar_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Calculate necessary memory for radar_hal_init.
 *
 * Return value used to allocate memory before calling @ref radar_hal_init.
 *
 * @return  Memory size in bytes needed to initialize Radar HAL.
 */
int radar_hal_get_instance_size(void);

/**
 * Initiate Radar HAL
 *
 * @param  radar_handle    Pointer to Radar handle is returned in this variable
 * @param  instance_memory Pointer to memory allocated before calling this function
 * @return                 Status of execution.
 */
int radar_hal_init(radar_handle_t ** radar_handle, void* instance_memory);

/**
 * Set Radar IC enable pin to specified level.
 *
 * @param  radar_handle Pointer to initialized Radar handle
 * @param  value        Value/level to set enable pin to
 * @return              Status of execution.
 */
uint32_t radar_hal_pin_set_enable(radar_handle_t * radar_handle, uint8_t value);

/**
 * Write data to Radar using SPI
 *
 * @param  radar_handle Pointer to initialized Radar handle
 * @param  data         Pointer to buffer with data to write
 * @param  length       Length of data to write
 * @return              Status of execution.
 */
uint32_t radar_hal_spi_write(radar_handle_t * radar_handle, uint8_t* data, uint32_t length);

/**
 * Read data from Radar using SPI
 *
 * @param  radar_handle Pointer to initialized Radar handle
 * @param  data         Pointer to buffer where data is returned
 * @param  length       Length of data to read
 * @return              Status of execution.
 */
uint32_t radar_hal_spi_read(radar_handle_t * radar_handle, uint8_t* data, uint32_t length);

/**
 * Write and read data to Radar using SPI
 *
 * @param  radar_handle Pointer to initialized Radar handle
 * @param  data         Pointer to buffer with data
 * @param  length       Length of data to write
 * @return              Status of execution.
 */

/**
 * Write and read data to Radar using SPI
 *
 * @param  radar_handle Pointer to initialized Radar handle
 * @param  wdata        Pointer to buffer with data to write
 * @param  wlength      Length of data to write
 * @param  rdata        Pointer to buffer where data is returned
 * @param  rlength      Length of data to read
 * @return              Status of execution.
 */
uint32_t radar_hal_spi_write_read(radar_handle_t * radar_handle, uint8_t* wdata, uint32_t wlength, uint8_t* rdata, uint32_t rlength);

#ifdef __cplusplus
}
#endif

#endif // RADAR_HAL_H
