/**
 * @file
 *
 * 
 */

#include "memorypool.h"
#include <FreeRTOS.h>
#include "xtsemaphore.h"

uint32_t memorypool_get_instance_size(uint32_t block_size, uint32_t block_count)
{
    uint32_t size = sizeof(MemoryPool_t) + (sizeof(MemoryBlock_t)+block_size)*block_count;
    return size;
}

uint32_t memorypool_create(MemoryPool_t** memorypool, void* instance_memory, uint32_t block_size, uint32_t block_count)
{
	uint32_t sizeof_memorypool = sizeof(MemoryPool_t);
	uint32_t sizeof_memoryblock = sizeof(MemoryBlock_t);
    MemoryPool_t* mp = (MemoryPool_t*)instance_memory;
    mp->buffer = (void*)((uint32_t)(void*)instance_memory + sizeof_memorypool);
    mp->block_count = block_count;
    mp->block_size = block_size;
    mp->free_blocks = block_count;
    mp->min_free_blocks = block_count;
    mp->taken_blocks = 0;
	mp->lock = xtlock_create();

    MemoryBlock_t* memoryblock = NULL;
    MemoryBlock_t* prev_memoryblock = NULL;

    for (uint32_t i = 0; i<block_count; i++)
    {
        memoryblock = (void*)((uint32_t)(void*)mp->buffer + i*(sizeof_memoryblock+block_size));
        memoryblock->prev = prev_memoryblock;
        memoryblock->pool = mp;
        memoryblock->block_size = block_size;
        memoryblock->buffer = (void*)((uint32_t)(void*)memoryblock + sizeof_memoryblock);
        if (prev_memoryblock)
        {
            prev_memoryblock->next = memoryblock;
        }
        prev_memoryblock = memoryblock;
    }
    memoryblock->next = NULL; // Last block.

    mp->first_free_block = mp->buffer;
    mp->last_free_block = memoryblock;
    mp->first_taken_block = NULL;
    mp->last_taken_block = NULL;

    *memorypool = mp;
    return XEP_MEM_ERROR_OK;
}

uint32_t memorypool_take(MemoryBlock_t** memoryblock, MemoryPool_t* memorypool)
{
    if (memorypool->free_blocks == 0) 
		return XEP_MEM_ERROR_FULL;

	if (XTLOCK_OK != xtlock_lock(memorypool->lock, 10 / portTICK_PERIOD_MS))
		return XEP_MEM_ERROR_FULL;

    if (memorypool->free_blocks == 0) // Make sure situation is still OK, in case of race condition.
    {
        xtlock_unlock(memorypool->lock);
        return XEP_MEM_ERROR_FULL;
    }

    *memoryblock = memorypool->first_free_block;

    memorypool->first_free_block = memorypool->first_free_block->next;
    if (memorypool->first_free_block)
        memorypool->first_free_block->prev = NULL;

	if (memorypool->taken_blocks == 0)
	{
		memorypool->first_taken_block = *memoryblock;
		memorypool->first_taken_block->prev = NULL;
	}
	else
	{
		(*memoryblock)->prev = memorypool->last_taken_block;
		memorypool->last_taken_block->next = *memoryblock;		
	}
	memorypool->last_taken_block = *memoryblock;
	memorypool->last_taken_block->next = NULL;

    memorypool->free_blocks--;

    if (memorypool->free_blocks < memorypool->min_free_blocks)
        memorypool->min_free_blocks = memorypool->free_blocks;

    memorypool->taken_blocks++;
	
	xtlock_unlock(memorypool->lock);

    return XEP_MEM_ERROR_OK;
}

uint32_t memorypool_free(MemoryBlock_t* memoryblock)
{
    MemoryPool_t* memorypool = memoryblock->pool;

	if (XTLOCK_OK != xtlock_lock(memorypool->lock, 10 / portTICK_PERIOD_MS))
        return XEP_MEM_ERROR_FAILED;

    memorypool->free_blocks++;
    memorypool->taken_blocks--;

    MemoryBlock_t* memoryblock_prev = memoryblock->prev;
    MemoryBlock_t* memoryblock_next = memoryblock->next;

    if (memoryblock_prev) memoryblock_prev->next = memoryblock_next;
    if (memoryblock_next) memoryblock_next->prev = memoryblock_prev;

    if (memorypool->first_taken_block == memoryblock)
        memorypool->first_taken_block = memoryblock_next;
    if (memorypool->last_taken_block == memoryblock)
        memorypool->last_taken_block = memoryblock_prev;

    if (memorypool->last_free_block)
    {
        if (memoryblock->prev)
            memoryblock->prev = memorypool->last_free_block;
        if (memorypool->last_free_block)
            memorypool->last_free_block->next = memoryblock;
    }
    memorypool->last_free_block = memoryblock;
    memorypool->last_free_block->next =NULL;
    if (memorypool->first_free_block == NULL) memorypool->first_free_block = memoryblock;

	xtlock_unlock(memorypool->lock);

    return XEP_MEM_ERROR_OK;
}
