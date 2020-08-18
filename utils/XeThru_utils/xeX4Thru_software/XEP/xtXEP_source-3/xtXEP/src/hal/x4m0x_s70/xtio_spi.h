#ifndef XTIO_SPI_H
#define XTIO_SPI_H

//==============================================================================
// SPI/QuadSPI
//

typedef enum {
    XTIO_SPI_NONE,
    XTIO_SPI,
    XTIO_QSPI
 }xtio_spi_mode_t;

/// Typedef of callback function for setting SPI mode
typedef int (* xtio_set_spi_mode_t)(void * spi_handle, xtio_spi_mode_t xtio_spi_mode);
/// Typedef of callback function for getting SPI mode
typedef int (* xtio_get_spi_mode_t)(void * spi_handle, xtio_spi_mode_t * xtio_spi_mode);
/// Typedef of callback function for writing through SPI
typedef int (* xtio_spi_write_t)(void * spi_handle, uint8_t * write_buffer, uint32_t number_of_bytes_to_write);
/// Typedef of callback function for reading through SPI
typedef int (* xtio_spi_read_t)(void * spi_handle, uint8_t * read_buffer, uint32_t number_of_bytes_to_read);
/// Typedef of callback function for write + read through SPI
typedef int (* xtio_spi_write_read_t)(void * spi_handle, uint8_t * write_buffer, uint32_t number_of_bytes_to_write, uint8_t * read_buffer, uint32_t number_of_bytes_to_read);

/**
 * Struct with information/data about a SPI interface
 */
typedef struct
{
    void * user_reference;
    void * spi_if;
    xtio_set_spi_mode_t set_spi_mode;
    xtio_get_spi_mode_t get_spi_mode;
    xtio_spi_write_t spi_write;
    xtio_spi_read_t spi_read;
    xtio_spi_write_read_t spi_write_read;
} xtio_spi_handle_t;

/**
 * Struct with pointers to SPI interface callback functions
 */
typedef struct
{
    xtio_set_spi_mode_t set_spi_mode;
    xtio_get_spi_mode_t get_spi_mode;
    xtio_spi_write_t spi_write;
    xtio_spi_read_t spi_read;
    xtio_spi_write_read_t spi_write_read;
} xtio_spi_callbacks_t;

/**
 * Initialize the SPI system
 *
 * If spi_callbacks pointers is NULL when calling this function,
 * default internal functions would be assigned
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_spi_init(
    xtio_spi_handle_t ** spi_handle,     ///<
    void* instance_memory,
    xtio_spi_callbacks_t * spi_callbacks,
    void * user_reference,
    xtio_spi_mode_t default_spi_mode
);

/**
 * Calculate necessary memory for xtio_spi interface.
 *
 * Return value used to allocate memory before calling @ref xtio_spi_init.
 *
 * @return  Memory size in bytes needed to initialize SPI interface.
 */
uint32_t xtio_spi_get_instance_size(void);

#endif