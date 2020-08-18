/**
 * @file
 *
 * 
 */

#include "xtsemaphore.h"
#include <FreeRTOS.h>
#include <semphr.h>

int xtlock_lock(void* lock, uint32_t timeout)
{
	if (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)lock, timeout))
		return XTLOCK_OK;
	else
		return XTLOCK_FAILED;
}

void xtlock_unlock(void* lock)
{
	xSemaphoreGive((SemaphoreHandle_t)lock);
}

void* xtlock_create(void)
{
	return (void*)xSemaphoreCreateMutex();
}

int xtsemaphore_take(void* semaphore, uint32_t timeout)
{
    if (!semaphore) return XTLOCK_FAILED;

    if (xSemaphoreTake(semaphore, timeout) != pdTRUE)
        return XTLOCK_OK;
    else
        return XTLOCK_FAILED;
}

void xtsemaphore_release(void* semaphore)
{
    xSemaphoreGive(semaphore);
}

void* xtsemaphore_create(uint32_t max_count, uint32_t initial_count)
{
    return xSemaphoreCreateCounting(max_count, initial_count);
}

