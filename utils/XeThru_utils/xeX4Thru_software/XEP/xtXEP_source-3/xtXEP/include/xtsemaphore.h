/**
 * @file
 * @brief Platform independent mutex mechanism.
 *
 * This is a generic interface to a mutex mechanism, providing methods to
 * create, lock and unlock the mutex.
 *
 */

#ifndef XTSEMAPHORE_H
#define XTSEMAPHORE_H

#include <stdint.h>

/// Return value of a function that was successful
#define XTLOCK_OK			0
/// Return value of a function where an unspecified error occured
#define XTLOCK_FAILED		1


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Lock mutex.
 *
 * Uses handle from @ref xtlock_create to lock mutex, within time given by timeout.
 *
 * @return Status of execution
 */
int xtlock_lock(void* lock, uint32_t timeout);

/**
 * Unlock mutex.
 *
 * @return Status of execution
 */
void xtlock_unlock(void* lock);

/**
 * Create handle to mutex.
 *
 * @return Handle to mutex.
 */
void* xtlock_create(void);


/**
 * Take semaphore.
 *
 * Uses handle from @ref xtlock_create to lock mutex, within time given by timeout.
 *
 * @return Status of execution
 */
int xtsemaphore_take(void* semaphore, uint32_t timeout);

/**
 * Unlock mutex.
 *
 * @return Status of execution
 */
void xtsemaphore_release(void* semaphore);

/**
 * Create handle to mutex.
 *
 * @return Handle to mutex.
 */
void* xtsemaphore_create(uint32_t max_count, uint32_t initial_count);

#ifdef __cplusplus
}
#endif


#endif // XTSEMAPHORE_H
