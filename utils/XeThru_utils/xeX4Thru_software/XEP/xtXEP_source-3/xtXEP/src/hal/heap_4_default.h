/**
 * @file
 *
 * @brief Local header file for heap_4_default.c that implements alternative functions for FreeRTOS heap allocation.
 */

#ifndef XT_HEAP_4_DEFAULT_H
#define XT_HEAP_4_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Already declared in portable.h
void * pvPortMalloc( size_t xSize );
void vPortFree( void *pv );
void vPortInitialiseBlocks( void );
size_t xPortGetFreeHeapSize( void );
size_t xPortGetMinimumEverFreeHeapSize( void );
*/

#ifdef __cplusplus
}
#endif

#endif // XT_HEAP_4_DEFAULT_H
