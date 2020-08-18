/**
 * @file
 *
 *
 */

#include "xtmemory.h"
#include "xep_hal.h"
#include <FreeRTOS.h>
#include "heap_4_default.h"
#include "heap_4_slow.h"
#include "heap_4_fast.h"

// Symbols defined in linker script.
extern int _end_default_heap_memory;
extern int _start_default_heap_memory;
extern int _end_fast_heap_memory;
extern int _start_fast_heap_memory;
extern int _end_slow_heap_memory;
extern int _start_slow_heap_memory;

xtmemory_types_t xtmemory_new_memory_type = XTMEMORY_DEFAULT;

void xtmemory_set_new_memory_type(xtmemory_types_t type)
{
    xtmemory_new_memory_type = type;
}

xtmemory_types_t xtmemory_get_new_memory_type()
{
    return xtmemory_new_memory_type;
}

xtmemory_types_t xtmemory_classify(void *ptr)
{
    intptr_t loc = (intptr_t)ptr;
    if ((intptr_t)&_start_default_heap_memory <= loc &&
            loc < (intptr_t)&_end_default_heap_memory) {
        return XTMEMORY_DEFAULT;
    } else if ((intptr_t)&_start_fast_heap_memory <= loc &&
               loc < (intptr_t)&_end_fast_heap_memory) {
        return XTMEMORY_FAST;
    } else if ((intptr_t)&_start_slow_heap_memory <= loc &&
               loc < (intptr_t)&_end_slow_heap_memory) {
        return XTMEMORY_SLOW;
    } else {
        return XTMEMORY_INVALID;
    }
}

void * xtmemory_malloc_default(uint32_t size)
{
    return pvPortMalloc(size);
}

void xtmemory_free_default(void* ptr)
{
    vPortFree(ptr);
}

uint32_t xtmemory_heap_watermark_default(void)
{
    return xPortGetMinimumEverFreeHeapSize();
}

uint32_t xtmemory_heap_available_default(void)
{
    return xPortGetFreeHeapSize();
}

void * xtmemory_malloc_slow(uint32_t size)
{
    if (xt_external_memory_enabled())
        return pvPortMalloc_slow(size);
    return xtmemory_malloc_default(size);
}

void xtmemory_free_slow(void* ptr)
{
    if (xt_external_memory_enabled())
        vPortFree_slow(ptr);
    else
        xtmemory_free_default(ptr);
}

uint32_t xtmemory_heap_watermark_slow(void)
{
    if (xt_external_memory_enabled())
        return xPortGetMinimumEverFreeHeapSize_slow();
    return 0;
}

uint32_t xtmemory_heap_available_slow(void)
{
    if (xt_external_memory_enabled())
        return xPortGetFreeHeapSize_slow();
    return 0;
}

void* xtmemory_malloc_fast(uint32_t size)
{
    if (xt_fast_memory_enabled())
        return pvPortMalloc_fast(size);
    return xtmemory_malloc_default(size);
}

void xtmemory_free_fast(void* ptr)
{
    if (xt_fast_memory_enabled())
        vPortFree_fast(ptr);
    else
        xtmemory_free_default(ptr);
}

uint32_t xtmemory_heap_watermark_fast(void)
{
    if (xt_fast_memory_enabled())
        return xPortGetMinimumEverFreeHeapSize_fast();
    return 0;
}

uint32_t xtmemory_heap_available_fast(void)
{
    if (xt_fast_memory_enabled())
        return xPortGetFreeHeapSize_fast();
    return 0;
}
