/**
 * @file
 *
 * @brief main() function for Radar Application
 *
 */

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "xep_hal.h"
#include "xep_application.h"
#include "task_monitor.h"
#include "task_hostcom.h"
#include "task_radar.h"

#define USE_TASK_MONITOR

// Adressing linker problem in product projects:
// http://stackoverflow.com/questions/25851138/how-to-include-syscalls-c-from-a-separate-library-file
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic push
#define FORCE_LINK(x) static const void* __ ## x ## _force_link __attribute__((__used__)) =(void*)&x
#pragma GCC diagnostic pop
extern int _getpid(void);

int main(void)
{
	FORCE_LINK(_getpid); // Force early link of syscalls.

	int status = 0;

    status = xtio_led_set_state(XTIO_LED_RED, XTIO_LED_OFF);
    status = xtio_led_set_state(XTIO_LED_GREEN, XTIO_LED_OFF);
    status = xtio_led_set_state(XTIO_LED_BLUE, XTIO_LED_OFF);

	// Make sure to sample mode pins early
	xt_get_operation_mode();

    // Create necessary tasks
	void* dispatch = NULL;
	void* x4driver = NULL;
    status = xep_init((void*)&dispatch);

    status = task_radar_init((void*)&x4driver, dispatch);

#ifdef USE_TASK_MONITOR
	status = task_monitor_init(dispatch);
#else
	WDT_Disable(WDT);
#endif

	status = task_hostcom_init(dispatch);
    status = task_application_init(dispatch, x4driver);

    // Start the RTOS scheduler.
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for (;;) {
	}

	return status;
}

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize );

StaticTask_t xIdleTaskTCB;
StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

StaticTask_t xTimerTaskTCB;
StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */


    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
