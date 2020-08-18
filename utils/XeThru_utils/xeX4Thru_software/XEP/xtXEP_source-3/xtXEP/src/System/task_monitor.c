/**
 * @file
 *
 *
 */

#include "task_monitor.h"
#include <FreeRTOS.h>
#include <string.h>
#include <stdio.h>
#include <task.h>
#include <queue.h>
#include "xtcompiler.h"
#include "xtmemory.h"
#include "xep_hal.h"
#include "xthousekeeping.h"
#include "xttoolbox.h"
#include "xep_dispatch_messages.h"
#include "board.h"

#define MONITOR_TASK_CYCLE_TIME 100UL // 100ms
// #define MONITOR_SEND_STATS
#define MONITOR_STATS_TICKS 100 // Send period as a multiple of cycle time.

// #define DISABLE_MONITOR_ON_NOEXTRAM

#define TASK_MONITOR_STACK_SIZE            (1500)

#ifdef MONITOR_SEND_STATS
#define TASK_MONITOR_NOSDRAM_STACK_SIZE    (250)
#else
#define TASK_MONITOR_NOSDRAM_STACK_SIZE    (100)
#endif

#define TASK_MONITOR_PRIORITY        (tskIDLE_PRIORITY + 7)


monitor_task_t * monitor_task_table[MAX_TASKS];

XepDispatch_t* p_dispatch = NULL;

static void task_monitor(void *pvParameters);

/**
* Register task to monitor
*/
int monitor_task_register(monitor_task_t ** monitor_task_handle, const uint32_t timeout_ms, uint32_t stack_size)
{
    uint32_t status = XT_ERROR;
#ifdef DISABLE_MONITOR_ON_NOEXTRAM
    if (!xt_external_memory_enabled()) {
        return status;
    }
#endif

    // Find next empty table entry
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (monitor_task_table[i] == NULL)
        {
            // Create task table entry
            *monitor_task_handle = (monitor_task_t *)xtmemory_malloc_default(sizeof(monitor_task_t));
            (*monitor_task_handle)->id = i;
			(*monitor_task_handle)->task_handle = xTaskGetCurrentTaskHandle();
            (*monitor_task_handle)->stack_size = stack_size;
            (*monitor_task_handle)->timeout_ms = timeout_ms;
            (*monitor_task_handle)->tick_counter = (*monitor_task_handle)->timeout_ms / portTICK_PERIOD_MS / MONITOR_TASK_CYCLE_TIME;
            monitor_task_table[i] = *monitor_task_handle;
            status = XT_SUCCESS;
            break;
        }
    }
    return status;
}

/**
* Get task information
*/
uint32_t monitor_task_get_info(monitor_task_t* monitor_task, uint32_t i)
{
#ifdef DISABLE_MONITOR_ON_NOEXTRAM
    if (!xt_external_memory_enabled())
        return XT_ERROR;
#endif

    if (i >= MAX_TASKS)
        return XT_ERROR;

    if (monitor_task_table[i] == NULL)
        return XT_ERROR;

    *monitor_task = *(monitor_task_table[i]);

    return XT_SUCCESS;
}

/**
* Reset counter to indicate that task is alive
*/
uint32_t monitor_task_alive(monitor_task_t * monitor_task_handle)
{
#ifdef DISABLE_MONITOR_ON_NOEXTRAM
    if (!xt_external_memory_enabled())
        return XT_SUCCESS;
#endif

    uint32_t status = XT_SUCCESS;
    if ((monitor_task_handle->id < MAX_TASKS) && (monitor_task_handle->id >= 0))
    {
        monitor_task_table[monitor_task_handle->id]->tick_counter = monitor_task_handle->timeout_ms / portTICK_PERIOD_MS / MONITOR_TASK_CYCLE_TIME;
    }
    else
    {
        status = XT_ERROR;
    }
    return status;
}

uint32_t task_monitor_init(XepDispatch_t* dispatch)
{
#ifdef DISABLE_MONITOR_ON_NOEXTRAM
    if (!xt_external_memory_enabled())
        return 0;
#endif

    uint32_t stack_size;
    if (!xt_external_memory_enabled())
        stack_size = TASK_MONITOR_NOSDRAM_STACK_SIZE;
    else
        stack_size = TASK_MONITOR_STACK_SIZE;


    TaskHandle_t h_task_monitor;

    for (int i = 0; i < MAX_TASKS; i++)
    {
        monitor_task_table[i] = NULL;
    }

    p_dispatch = dispatch;

    xTaskCreate(task_monitor, (const char * const) "System", stack_size,
                (void*)dispatch, TASK_MONITOR_PRIORITY, &h_task_monitor);

    return 0;
}

static void task_monitor(void *pvParameters)
{
    XepDispatch_t* dispatch = (XepDispatch_t*)pvParameters;
    UNUSED(dispatch);

    TickType_t last_wake_time = xTaskGetTickCount();

    xt_swreset_reason_t reason;
    xt_get_reset_reason(&reason);
    if (reason == XT_SWRST_HARD_WDT)
    {
        xthousekeeping_trigger_crashdump(XT_SWRST_SYSTEM_MONITOR,
                                         "Hard watchdog timeout", false);
        // Store reset reason. and reset
        xt_software_reset(XT_SWRST_SYSTEM_MONITOR);
    }

    bool crash_count_has_been_reset = false;
#ifdef MONITOR_SEND_STATS
    uint32_t stats_countdown = MONITOR_STATS_TICKS;
#endif
    for (;;)
    {
        if (!crash_count_has_been_reset)
        {
            // Wait until 5 min = 5*60*1000*1000 us has passed
            if (xttb_systimer_us() > 5*60*1000*1000)
            {
                // No problem in 5 min, reset crash counter
                xt_reset_crash_count();

                crash_count_has_been_reset = true;
            }
        }

        // Update tick counter for each task
        for (int i = 0; i < MAX_TASKS; i++)
        {
            if (monitor_task_table[i] != NULL)
            {
                if (monitor_task_table[i]->tick_counter < 0)
                {
                    // task timed out
                    xtio_led_set_state(XTIO_LED_RED, XTIO_LED_ON);
                    xtio_led_set_state(XTIO_LED_GREEN, XTIO_LED_OFF);
                    xtio_led_set_state(XTIO_LED_BLUE, XTIO_LED_OFF);

                    xthousekeeping_trigger_crashdump(XT_SWRST_SYSTEM_MONITOR, "Task timeout", true);

                    // Store reset reason. and reset
                    xt_software_reset(XT_SWRST_SYSTEM_MONITOR);

                    while(1)
                    {
                        // If reset works, we should not come here
                        int dummy = 1;
                        (void)dummy;
                    }
                }
                monitor_task_table[i]->tick_counter--;
            }
        }

#ifdef MONITOR_SEND_STATS
        if (!--stats_countdown)
        {
            stats_countdown = MONITOR_STATS_TICKS;
            char buf[70];
            const char pool_prefix[] = "Pools: ";
            memcpy(buf, pool_prefix, sizeof(pool_prefix));
            uint32_t size = memorypoolset_print_statistics(
                                &buf[sizeof(pool_prefix)],
                                sizeof(buf) - sizeof(pool_prefix),
                                dispatch->memorypoolset);
            dispatch_message_hostcom_send_data_stringn(dispatch,
                XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND, 0, 0, buf,
                size + sizeof(pool_prefix));

            for (uint32_t i = 0; i < MAX_TASKS; ++i)
            {
                if (monitor_task_table[i] != NULL)
                {
                    size = snprintf(
                        buf,
                        sizeof(buf),
                        "Stack: %s: %lu/%lu",
                        pcTaskGetName(monitor_task_table[i]->task_handle),
                        uxTaskGetStackHighWaterMark(monitor_task_table[i]->task_handle),
                        monitor_task_table[i]->stack_size
                    );
                    dispatch_message_hostcom_send_data_stringn(dispatch,
                        XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND, 0, 0, buf,
                        size);
                }
            }

            size = snprintf(buf, sizeof(buf),
                "Heaps: f: %lu (%lu), d: %lu (%lu), s: %lu (%lu)",
                xtmemory_heap_available_fast(), xtmemory_heap_watermark_fast(),
                xtmemory_heap_available_default(), xtmemory_heap_watermark_default(),
                xtmemory_heap_available_slow(), xtmemory_heap_watermark_slow()
            );
            dispatch_message_hostcom_send_data_stringn(dispatch,
                XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND, 0, 0, buf, size);
        }
#endif

        xt_feed_watchdog();
        vTaskDelayUntil(&last_wake_time, MONITOR_TASK_CYCLE_TIME / portTICK_PERIOD_MS);
    }
}
