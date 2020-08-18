#include "xthousekeeping.h"
#include "xtfile.h"
#include "xfs.h"
#include "xtmemory.h"
#include "task_monitor.h"
#include <FreeRTOS.h>
#include <task.h>
#include "xt_system.h"

// Enabling crashdumps in noextram also requires increasing task monitor
// stack size enough to save a file.
#define DISABLE_CRASHDUMPS_ON_NOEXTRAM
#define MAX_CRASH_DUMP_SIZE     (16*1024)
uint8_t* crashdump_buffer;

extern int _estack;
extern int __ram_end__;

char application_info[100];

bool safe_mode_initialized = false;
bool safe_mode = false;

XepDispatch_t* pdispatch = NULL;

int xthousekeeping_init(XepDispatch_t* dispatch, char* app_info)
{
#ifdef DISABLE_CRASHDUMPS_ON_NOEXTRAM
    if (!xt_external_memory_enabled())
        return XT_ERROR;
#endif

    crashdump_buffer = xtmemory_malloc_slow(MAX_CRASH_DUMP_SIZE);

    pdispatch = dispatch;

    // Copy application info string
    int i = 0;
    while (app_info[i])
    {
        application_info[i] = app_info[i];
        i++;
    }
    application_info[i] = 0;

    // Register callback for system critical errors
    xt_register_crashdump_callback(xthousekeeping_trigger_crashdump);

    return XT_SUCCESS;
}

bool xthousekeeping_in_safe_mode(void)
{
    if (!safe_mode_initialized)
    {
        // Go into safe mode if reset reason indicates trouble,
        // and reset count is large
        xt_swreset_reason_t reason;
        xt_get_reset_reason(&reason);
        if (((reason != XT_SWRST_HW_PIN) && 
            (reason != XT_SWRST_INTENDED) &&
            (reason != XT_SWRST_HOSTCOMMAND) && 
            (reason != XT_SWRST_NONE)) &&
            (xt_get_crash_count() >= 10))
        {
            if ((xt_get_crash_count() > 15) &&
                (reason != XT_SWRST_BOOTLOADER))
                xt_software_reset(XT_SWRST_BOOTLOADER);

            safe_mode = true;
        }

        safe_mode_initialized = true;
    }
    return safe_mode;
}

int xthousekeeping_trigger_crashdump(xt_swreset_reason_t crash_reason, const char* info, bool full_dump)
{
#ifdef DISABLE_CRASHDUMPS_ON_NOEXTRAM
    if (!xt_external_memory_enabled())
        return XT_ERROR;
#endif

    taskENTER_CRITICAL_FROM_ISR();

	// Uninitialize the file system handler
	xtfile_uninit();

    // Ignore lock since tasks have been stopped
    xfs_ignore_lock(true);
	
    uint32_t free_size_in_file_system = 0;
	if (xfs_get_largest_free_size(&free_size_in_file_system) != XFS_OK)
		free_size_in_file_system = 0;
    uint32_t buffer_size = MAX_CRASH_DUMP_SIZE;
    if (buffer_size > free_size_in_file_system)
	{
        buffer_size = free_size_in_file_system;
	}

	uint32_t pool_count = 0;
    if (full_dump) 
        pool_count = pdispatch->memorypoolset->set_count;

    // Is there enough space to get any info into the file system?
    if ( ((212 + pool_count*3)*4) > buffer_size)
        return XT_ERROR;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
    uint32_t used_size = 0;
    // Build header
	crashdump_buffer[used_size++] = 0x10; // Type
	crashdump_buffer[used_size++] = 0x02; // Version
	xt_swreset_reason_t reset_reason;
	xt_get_reset_reason(&reset_reason);
	crashdump_buffer[used_size++] = reset_reason;
	crashdump_buffer[used_size++] = xt_get_operation_mode();

    // Version string
    uint32_t i = 0;
    for (i = 0; i < 100; i++)
        crashdump_buffer[used_size++] = application_info[i];

    // Crash info string
	i = 0;
    while ((i < 100) && (info[i]))
        crashdump_buffer[used_size++] = info[i++];

    while(i < 100)
    {
        crashdump_buffer[used_size++] = 0;
        i++;
    }

    // Overall system information
	*((uint32_t*) &crashdump_buffer[used_size]) = crash_reason;
	used_size += 4;
	*((uint32_t*) &crashdump_buffer[used_size]) = xt_get_reset_count();
	used_size += 4;
	*((uint64_t*) &crashdump_buffer[used_size]) = xt_get_system_timer_us();
	used_size +=8;
	*((uint32_t*) &crashdump_buffer[used_size]) = xtmemory_heap_available_default();
	used_size += 4;
	*((uint32_t*) &crashdump_buffer[used_size]) = xtmemory_heap_watermark_default();
	used_size += 4;
	*((uint32_t*) &crashdump_buffer[used_size]) = xtmemory_heap_available_slow();
	used_size += 4;
	*((uint32_t*) &crashdump_buffer[used_size]) = xtmemory_heap_watermark_slow();
	used_size += 4;
	*((uint32_t*) &crashdump_buffer[used_size]) = xtmemory_heap_available_fast();
	used_size += 4;
	*((uint32_t*) &crashdump_buffer[used_size]) = xtmemory_heap_watermark_fast();
	used_size += 4;

    if (full_dump)
    {
        *((uint32_t*) &crashdump_buffer[used_size]) = pool_count;
        used_size += 4;
        for (i = 0; i < pool_count; i++)
        {
            *((uint32_t*) &crashdump_buffer[used_size]) = pdispatch->memorypoolset->set[i]->min_free_blocks;
            used_size += 4;
        }

        // Current task information
        uint32_t* stack_pointer;
        uint32_t stack_depth;
        
        void* handle = xTaskGetCurrentTaskHandle();
        xt_get_stack_area_from_task_handle(handle,
                                            0, // Can't know the stack size
                                            (void**)&stack_pointer,
                                            &stack_depth);
        stack_depth = 512; // Read out the last 512B, should be enough to get the latest call stack for most tasks
        
        // Make sure to not read outside of RAM section
        if (((uint32_t)stack_pointer + stack_depth) > ((uint32_t)&__ram_end__))
            stack_depth = (uint32_t*)&__ram_end__ - stack_pointer;
        
        if ( (16 + used_size + (2*4) + stack_depth) > buffer_size)
            return XT_ERROR;

        // Name of task
        char* name = pcTaskGetName(handle);
        i = 0;
        if (name != NULL)
        {
            while ((name[i]) && (i < 15))
            {
                crashdump_buffer[used_size++] = name[i];
                i++;
            }
        }
        while (i < 16)
        {
            crashdump_buffer[used_size++] = '\0';
            i++;
        }
        
        *((uint32_t*) &crashdump_buffer[used_size]) = (uint32_t)stack_pointer;
        used_size += 4;
        *((uint32_t*) &crashdump_buffer[used_size]) = stack_depth;
        used_size += 4;
        
        for (i = 0; (i*sizeof(uint32_t)) < stack_depth; i++)
        {
            *((uint32_t*) &crashdump_buffer[used_size]) = stack_pointer[i];
            used_size += 4;
        }

        // Per-task information
        monitor_task_t task_info;
        uint32_t task_count = 0;
        for (i = 0; i < MAX_TASKS; i++)
        {
            if (monitor_task_get_info(&task_info, i) == XT_SUCCESS)
            {
                task_count++;
            }
        }

        *((uint32_t*) &crashdump_buffer[used_size]) = task_count; 
        used_size += 4;

        for (i = 0; i < MAX_TASKS; i++)
        {
            if (monitor_task_get_info(&task_info, i) == XT_SUCCESS)
            {
                if ((used_size + 16 + 5*4) > (buffer_size-1))
                    break;
                
                // Name of task
                name = pcTaskGetName(task_info.task_handle);
                uint32_t j = 0;
                while ((name[j]) && (j < 15))
                {
                    crashdump_buffer[used_size++] = name[j];
                    j++;
                }
                while (j < 16)
                {
                    crashdump_buffer[used_size++] = '\0';
                    j++;
                }
                
                // Tick value
                *((uint32_t*) &crashdump_buffer[used_size]) = task_info.tick_counter; 
                used_size += 4;
                
                // Last known program counter value
                void* task_program_counter = xt_get_program_counter_from_task_handle(task_info.task_handle);
                *((uint32_t*) &crashdump_buffer[used_size]) = (uint32_t) task_program_counter;
                used_size += 4;

                // Stack watermark
                *((uint32_t*) &crashdump_buffer[used_size]) = (uint32_t) uxTaskGetStackHighWaterMark(task_info.task_handle);
                used_size += 4;

                xt_get_stack_area_from_task_handle(task_info.task_handle, 
                                                    task_info.stack_size, 
                                                    (void**)&stack_pointer, 
                                                    &stack_depth);
                
                *((uint32_t*) &crashdump_buffer[used_size]) = (uint32_t)stack_pointer;
                used_size += 4;
                
                *((uint32_t*) &crashdump_buffer[used_size]) = stack_depth;
                used_size += 4;

                if ((used_size + stack_depth) > (buffer_size-1))
                    break;

                for (j = 0; (j*sizeof(uint32_t)) < stack_depth; j++)
                {
                    *((uint32_t*) &crashdump_buffer[used_size]) = ((uint32_t*)stack_pointer)[j];
                    used_size += 4;
                }
            }
        }
    }
    #pragma GCC diagnostic pop
    
    uint32_t dummy_size = 0;
    if (xfs_get_size(XTFILE_TYPE_CRASHDUMP, 0, &dummy_size) != XFS_FILE_NOT_FOUND)
        xfs_delete_file(XTFILE_TYPE_CRASHDUMP, 0);
    
    if (xfs_create_file(XTFILE_TYPE_CRASHDUMP, 0, used_size) != XFS_OK)
        return XT_ERROR;
    
    if (xfs_commit_file(XTFILE_TYPE_CRASHDUMP, 0, crashdump_buffer, used_size) != XFS_OK)
        return XT_ERROR;

    return XT_SUCCESS;
}
