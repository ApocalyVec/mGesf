/**
 * @file
 *
 * Callback functions used by FreeRTOS
 */

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_hooks.h"
#include "xep_hal.h"
#include "xt_system.h"

// Since FreeRTOS 7.5.3 uxTopUsedPriority is no longer present in the
// kernel so it has to be supplied by other means for JLinkGDBServer's
// thread awareness.
#ifdef __GNUC__
#define USED __attribute__((used))
#else
#define USED
#endif
const int USED uxTopUsedPriority = configMAX_PRIORITIES;

void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	volatile uint32_t block_var = 0;

	taskDISABLE_INTERRUPTS();
	xt_trigger_crash(XT_SWRST_MALLOC_FAILED, "vApplicationMallocFailedHook, FreeRTOS_HOOKS.c", true);

	while (block_var == 0)
	{
		/* Set block_var to a non-zero value in the debugger to
		step out of this function. */
	}
}

void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */

	xt_idle_sleep();

#if 0
	// Added code to make idle task take cpu load. Correctly measure idle time during profiling.
	static uint32_t idle_tick=0;
	for(;;)
	{
		idle_tick++;
		if (idle_tick > 42)
		{
			idle_tick = 0;
			
		}
	}
#endif
	/*
	The code below can be used to check how much of each tasks stack is unused.
	Let the program run until all (or almost all) combinations of function calls and interrupts have occurred.
	Set a breakpoint at "if (test == 1)" and change the value of test to 1 (in the debugger).
	Then step through and check unused stack size.
	*/
	//## xtsUserRadarPerformanceStatus.idleTicks++;

/*
	volatile int test = 0;
	if (test == 1)
	{
		unsigned portBASE_TYPE taskUnusedStack_word = 0;
		unsigned portBASE_TYPE totalUnusedStack_word = 0;

		size_t freesrotHeapFreeBytes = xPortGetFreeHeapSize();
		unsigned long totalUnusedStack_bytes = 0;

		taskUnusedStack_word = uxTaskGetStackHighWaterMark(pTaskRadar);
		totalUnusedStack_word += taskUnusedStack_word;
		taskUnusedStack_word = uxTaskGetStackHighWaterMark(pTaskAppEnginePrimary);
		totalUnusedStack_word += taskUnusedStack_word;
		taskUnusedStack_word = uxTaskGetStackHighWaterMark(pTaskAppEngineSecondary);
		totalUnusedStack_word += taskUnusedStack_word;
		taskUnusedStack_word = uxTaskGetStackHighWaterMark(pTaskSystem);
		totalUnusedStack_word += taskUnusedStack_word;
		taskUnusedStack_word = uxTaskGetStackHighWaterMark(pTaskUartRx);
		totalUnusedStack_word += taskUnusedStack_word;
		taskUnusedStack_word = uxTaskGetStackHighWaterMark(pTaskUartTx);
		totalUnusedStack_word += taskUnusedStack_word;

#if defined(CONF_BOARD_USB_PORT)
		taskUnusedStack_word = uxTaskGetStackHighWaterMark(pTaskUsbRx);
		totalUnusedStack_word += taskUnusedStack_word;
		taskUnusedStack_word = uxTaskGetStackHighWaterMark(pTaskUsbTx);
		totalUnusedStack_word += taskUnusedStack_word;
#endif

		totalUnusedStack_bytes = totalUnusedStack_word * sizeof(portBASE_TYPE);
	}
*/
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask,
		signed char *pcTaskName)
{
	(void) pcTaskName;
	(void) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();

	xt_set_reset_reason(XT_SWRST_STACK_OVERFLOW);
	volatile int test = 0;
	if (test == 0)
	{
		//ioport_toggle_pin_level(XPIN_LED0);
	}
	vApplicationIdleHook();
}

void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */

	//rtcclock_systick_ms_inc(); // Increase ms counter in rtcclock.
}

void assert_triggered(const char *file, uint32_t line)
{
	volatile uint32_t block_var = 0, line_in;
	const char *file_in;

	/* These assignments are made to prevent the compiler optimizing the
	values away. */
	file_in = file;
	line_in = line;
	(void) file_in;
	(void) line_in;

	taskENTER_CRITICAL();
	{
		while (block_var == 0) {
			/* Set block_var to a non-zero value in the debugger to
			step out of this function. */
		}
	}
	taskEXIT_CRITICAL();
}
