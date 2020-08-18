/**
 * @file
 *
 *
 */

#ifndef FREERTOS_HOOKS_H
#define FREERTOS_HOOKS_H
#include "FreeRTOS.h"

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char *pcTaskName);
void vApplicationTickHook(void);


#endif // FREERTOS_HOOKS_H
