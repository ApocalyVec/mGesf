
#include "radar_hal.h"
#include "xep_hal.h"
#include <FreeRTOS.h>
#include <task.h>
#include <string.h>

int radar_hal_init(radar_handle_t ** radar_handle, void* instance_memory)
{
	int status = XT_SUCCESS;
	radar_handle_t * radar_handle_local = (radar_handle_t *)instance_memory;
	memset(radar_handle_local, 0, sizeof(radar_handle_t));
	radar_handle_local->radar_id = 0;
	status = xtio_spi_init(
		(void *)&(radar_handle_local->spi_handle), 
		(void *)((int)instance_memory + sizeof(radar_handle_t)), 
		NULL, 
		&(radar_handle_local->radar_id), 
		XTIO_SPI);
	*radar_handle = radar_handle_local;

    xtio_set_direction(XTIO_X4_ENABLE, XTIO_OUTPUT, XTIO_PIN_LEVEL_LOW);
    xtio_set_pin_mode(XTIO_X4_IO1, XTIO_PULL_DOWN);
    xtio_set_pin_mode(XTIO_X4_IO2, XTIO_PULL_DOWN);

	return status;
}

int radar_hal_get_instance_size(void)
{
	int size = sizeof(radar_handle_t);
	size += xtio_spi_get_instance_size();
	return size;
}

uint32_t radar_hal_pin_set_enable(radar_handle_t * radar_handle, uint8_t value)
{
	xtio_pin_level_t level = XTIO_PIN_LEVEL_LOW;
	if(value == 1)
		level = XTIO_PIN_LEVEL_HIGH;
		
	return xtio_set_level(XTIO_X4_ENABLE,level);
}

uint32_t radar_hal_spi_write(radar_handle_t * radar_handle, uint8_t* data, uint32_t length)
{
	if (0 == length)
	{
		return XT_SUCCESS;
	}
	if (NULL == data)
	{
		return XT_ERROR;
	}
	xtio_spi_handle_t * spi_handle = (xtio_spi_handle_t *)radar_handle->spi_handle;
	return spi_handle->spi_write_read(spi_handle, data, length, NULL, 0);
}

uint32_t radar_hal_spi_read(radar_handle_t * radar_handle, uint8_t* data, uint32_t length)
{
	if (0 == length)
	{
		return XT_SUCCESS;
	}
	if (NULL == data)
	{
		return XT_ERROR;
	}
	xtio_spi_handle_t * spi_handle = (xtio_spi_handle_t *)radar_handle->spi_handle;
	return spi_handle->spi_write_read(spi_handle, NULL, 0, data, length);
}

uint32_t radar_hal_spi_write_read(radar_handle_t * radar_handle, uint8_t* wdata, uint32_t wlength, uint8_t* rdata, uint32_t rlength)
{
	if ((0 == wlength) && (0 == rlength))
	{
		return XT_SUCCESS;
	}
	if ((NULL == wdata) || (NULL == rdata))
	{
		return XT_ERROR;
	}	
	xtio_spi_handle_t * spi_handle = (xtio_spi_handle_t *)radar_handle->spi_handle;
	return spi_handle->spi_write_read(spi_handle, wdata, wlength, rdata, rlength);
}
