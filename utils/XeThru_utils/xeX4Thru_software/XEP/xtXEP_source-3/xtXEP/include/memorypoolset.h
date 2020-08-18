/**
 * @file
 * @brief Set of memory pools.
 *
 * Provides handling of a set of memory pools.
 * Can configure several memory pools with different block size.
 * Automatically choose optimal memory pool for use.
 *
 */


#ifndef MEMORYPOOLSET_H
#define MEMORYPOOLSET_H

#include <stdint.h>
#include <stddef.h>
#include "memorypool.h"

/// Return value of a function that was successful
#define XEP_MEM_ERROR_OK					0
/// Return value of a function that failed in some way
#define XEP_MEM_ERROR_FAILED				1
/// Return value of a function that failed due to insufficient resources
#define XEP_MEM_ERROR_FULL  				10


/// Maximum number of memory pools managed by memory pool set.
#define MEMORYPOOLSET_MAX_POOLS 5

typedef struct MemoryPoolSet_t MemoryPoolSet_t;
/**
 * Memory pool set
 *
 * Struct containing reference to set of memory pools and the number of pools active.
 */
struct MemoryPoolSet_t
{
    uint32_t set_count;
    MemoryPool_t* set[MEMORYPOOLSET_MAX_POOLS];
};


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get size needed for memory pool set structure.
 *
 * @return Instance size
 */
uint32_t memorypoolset_get_instance_size(void);

/**
 * @brief Create memory pool set
 *
 * Return memory pool set as pointer.
 *
 * @return Status of execution
 */
uint32_t memorypoolset_create(MemoryPoolSet_t** memorypoolset, void* instance_memory);

/**
 * @brief Add memory pool
 *
 * Add a new memory pool to the set.
 *
 * Return memory pool as pointer.
 *
 * @return Status of execution
 */
uint32_t memorypoolset_add_memorypool(MemoryPool_t** memorypool, MemoryPoolSet_t* memorypoolset, void* instance_memory, uint32_t block_size, uint32_t block_count);

/**
 * @brief Select pool in set based on size
 *
 * Uses information about block size to find the best suited pool in set.
 *
 * Return memory pool as pointer.
 *
 * @return Status of execution
 */
uint32_t memorypoolset_select_memorypool(MemoryPool_t** memorypool, MemoryPoolSet_t* memorypoolset, uint32_t size);

/**
 * @brief Take memory block from automatically chosen pool
 *
 * Uses information about block size to find the best suited pool in set.
 * Then takes the first free memory block from that pool
 *
 * Return memory block as pointer.
 *
 * @return Status of execution
 */
uint32_t memorypoolset_take_auto(MemoryBlock_t** memoryblock, MemoryPoolSet_t* memorypoolset, uint32_t size);

/**
 * @brief Prints statistics about available memoryblocks.
 *
 * @return Status of execution
 */
uint32_t memorypoolset_print_statistics(char *buf, size_t max_len,
                                        MemoryPoolSet_t *memorypoolset);

#ifdef __cplusplus
}
#endif


#endif // MEMORYPOOLSET_H
