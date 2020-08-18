/**
 * @file
 * @brief Memory pool implementation.
 *
 * Provides a flexible yet predictible way to allocate memory.
 * Fixed size memory blocks are allocated. Each block is always present
 * in either of two linked lists, taken or free.
 * Taking a block moves the block from the free list to the taken list in
 * a predictible way, with no memory allocation done.
 * Freeing a block moves it back to the free list.
 *
 */

#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <stdint.h>

/// Return value of a function that was successful
#define XEP_MEM_ERROR_OK					0
/// Return value of a function that failed in some way
#define XEP_MEM_ERROR_FAILED				1
/// Return value of a function that failed due to insufficient resources
#define XEP_MEM_ERROR_FULL  				10


typedef struct MemoryBlock_t MemoryBlock_t;
typedef struct MemoryPool_t MemoryPool_t;

/**
 * Memory block
 *
 * Struct containing reference to memory pool, linked list variables, block size and block memory reference.
 */
struct MemoryBlock_t
{
    MemoryPool_t* pool;
    MemoryBlock_t* next;
    MemoryBlock_t* prev;
    uint32_t block_size;
    void* buffer;			// Buffer for memory block
};

/**
 * Memory pool
 *
 * Container for memory pool.
 * Has references to the two lists, free and taken, information about block size and block count.
 * Also contains status on pool, i.e. amount of free and taken blocks.
 */
struct MemoryPool_t
{
	void* lock;
    MemoryBlock_t* first_free_block;
    MemoryBlock_t* last_free_block;
    MemoryBlock_t* first_taken_block;
    MemoryBlock_t* last_taken_block;
    uint32_t block_size;
    uint32_t block_count;
    uint32_t free_blocks;
    uint32_t min_free_blocks;
    uint32_t taken_blocks;
    void* buffer;			// Buffer for memory pool
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get size needed for memory pool.
 *
 * Uses information about block size and count to calculate memory required for memory pool.
 *
 * @return Status of execution
 */
uint32_t memorypool_get_instance_size(uint32_t block_size, uint32_t block_count);

/**
 * @brief Create memory pool
 *
 * Uses allocated memory reference and information about block size and cound,
 * and creates memory pool.
 *
 * Returns memory pool as pointer.
 *
 * @return Status of execution
 */
uint32_t memorypool_create(MemoryPool_t** memorypool, void* instance_memory, uint32_t block_size, uint32_t block_count);

/**
 * @brief Take memory block
 *
 * Takes first available memory block from pool.
 *
 * Returns memory block as pointer.
 *
 * @return Status of execution
 */
uint32_t memorypool_take(MemoryBlock_t** memoryblock, MemoryPool_t* memorypool);

/**
 * @brief Free memory block
 *
 * @return Status of execution
 */
uint32_t memorypool_free(MemoryBlock_t* memoryblock);


#ifdef __cplusplus
}
#endif


#endif // MEMORYPOOL_H
