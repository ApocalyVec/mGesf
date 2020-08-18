/**
 * @file
 *
 * @brief Local header file for heap_4_slow.c that implements alternative functions for FreeRTOS heap allocation.
 */

#ifndef XT_HEAP_4_SLOW_H
#define XT_HEAP_4_SLOW_H

#ifdef __cplusplus
extern "C" {
#endif

void * pvPortMalloc_slow( size_t xSize );
void vPortFree_slow( void *pv );
void vPortInitialiseBlocks_slow( void );
size_t xPortGetFreeHeapSize_slow( void );
size_t xPortGetMinimumEverFreeHeapSize_slow( void );

#ifdef __cplusplus
}
#endif

#endif // XT_HEAP_4_SLOW_H
