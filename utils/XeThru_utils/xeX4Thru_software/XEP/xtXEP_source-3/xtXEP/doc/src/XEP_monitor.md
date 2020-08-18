XEP system health monitoring {#xep_monitor}
============================

To monitor the system health of an application using XEP, the MCU watchdog and a
monitor task is used.

When an error is found, the monitor task will do a system reset.

A special FreeRTOS task called @ref monitor_task has the code to check that the
other tasks are alive. To get the monitor_task recognize which tasks to monitor,
a register function @ref monitor_task_register is provided to register the task
to the monitor system. This register task has a timeout parameter to set the
maximum time between two adjacent calls of an "alive" function call
(@ref monitor_task_alive) before the monitor task will trigger and do a system
reset.

The monitor task will execute with an interval defined by
@ref MONITOR_TASK_CYCLE_TIME.

Example of using the monitor task to monitor a FreeRTOS task:

    #include "monitor_task.h"

    static void task_application(void *pvParameters)
    {
        // Your init code

        // Register the current task to the monitor system.
        // Timeout is set to 1000ms
        monitor_task_t * monitor_task_handle;
        status = monitor_task_register(&monitor_task_handle, 1000);

        // The task loop
        for(;;)
        {
            // Signal alive to the monitor system
            status = monitor_task_alive(monitor_task_handle);

            // Your task code
        }
    }

If the monitor task do not catch the error, the MCU watchdog timer is enabled
and will do a system reset when the watchdog timer expires.

The reason for a reset is stored in a non volatile MCU register that can be
examined after a reset. To get the reason for the last reset, the function
@ref xt_get_reset_reason will return the last reason for a reset.
See @ref xt_swreset_reason_t for a description of reset reasons.
