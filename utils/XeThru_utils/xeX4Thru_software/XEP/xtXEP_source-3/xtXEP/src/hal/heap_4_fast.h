/**
 * @file
 *
 * @brief Local header file for heap_4_fast.c that implements alternative functions for FreeRTOS heap allocation.
 */

#ifndef XT_HEAP_4_FAST_H
#define XT_HEAP_4_FAST_H

#ifdef __cplusplus
extern "C" {
#endif

void * pvPortMalloc_fast( size_t xSize );
void vPortFree_fast( void *pv );
void vPortInitialiseBlocks_fast( void );
size_t xPortGetFreeHeapSize_fast( void );
size_t xPortGetMinimumEverFreeHeapSize_fast( void );

#ifdef __cplusplus
}
#endif

#endif // XT_HEAP_4_FAST_H
