/**
 * @file
 * @brief Platform independent mutex mechanism.
 *
 * This is a generic interface to a mutex mechanism, providing methods to
 * create, lock and unlock the mutex.
 *
 */

#ifndef XTMEMORY_H
#define XTMEMORY_H

#include <stdint.h>



#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    XTMEMORY_DEFAULT,
    XTMEMORY_SLOW,
    XTMEMORY_FAST,
    XTMEMORY_INVALID = -1
} xtmemory_types_t;

/**
 * @brief Set which memory store to use for C++ new operator
 */
void xtmemory_set_new_memory_type(xtmemory_types_t type);

/**
 * @brief Get which memory store is used for C++ new operator
 */
xtmemory_types_t xtmemory_get_new_memory_type(void);

/**
 * @brief Get which memory type the pointer has.
 */
xtmemory_types_t xtmemory_classify(void *ptr);

/**
 * @brief Allocate memory from default location
 *
 * Use operating system mechanism to allocate default memory. Normally internal
 * SRAM.
 *
 * Size of memory segment is defined in linker script - @ref xep_memory.
 *
 * @return Pointer to memory
 */
void* xtmemory_malloc_default(uint32_t size);

/**
 * @brief Free memory allocated by xtmemory_malloc_default.
 *
 * @param ptr Pointer to memory
 */
void xtmemory_free_default(void* ptr);

/**
 * Get the default memory usage watermark indicated by historically lowest amount of 
 * available memory. 
 *
 * @return Historical minimum amount of free memory. 
 */
uint32_t xtmemory_heap_watermark_default(void);

/**
 * Get amount of default heap memory available.
 *
 * @return  Number of bytes available.
 */
uint32_t xtmemory_heap_available_default(void);


/**
 * @brief Allocate memory from slow location
 *
 * Use operating system mechanism to allocate slow memory. Normally external
 * SRAM/SDRAM.
 *
 * Size of memory segment is defined in linker script - @ref xep_memory.
 *
 * @return Pointer to memory
 */
void* xtmemory_malloc_slow(uint32_t size);

/**
 * @brief Free memory allocated by xtmemory_malloc_slow.
 *
 * @param ptr Pointer to memory
 */
void xtmemory_free_slow(void* ptr);

/**
 * Get the slow memory usage watermark indicated by historically lowest amount of 
 * available memory. 
 *
 * @return Historical minimum amount of free memory. 
 */
uint32_t xtmemory_heap_watermark_slow(void);

/**
 * Get amount of slow heap memory available.
 *
 * @return  Number of bytes available.
 */
uint32_t xtmemory_heap_available_slow(void);


/**
 * @brief Allocate memory from fast location
 *
 * Use operating system mechanism to allocate special fast memory. This can be
 * Tightly Coupled Memory.
 *
 * Size of memory segment is defined in linker script - @ref xep_memory.
 *
 * @return Pointer to memory
 */
void* xtmemory_malloc_fast(uint32_t size);

/**
 * @brief Free memory allocated by xtmemory_malloc_fast.
 *
 * @param ptr Pointer to memory
 */
void xtmemory_free_fast(void* ptr);

/**
 * Get the fast memory usage watermark indicated by historically lowest amount of 
 * available memory. 
 *
 * @return Historical minimum amount of free memory. 
 */
uint32_t xtmemory_heap_watermark_fast(void);

/**
 * Get amount of fast heap memory available.
 *
 * @return  Number of bytes available.
 */
uint32_t xtmemory_heap_available_fast(void);



#ifdef __cplusplus
}
#endif


#endif // XTMEMORY_H
