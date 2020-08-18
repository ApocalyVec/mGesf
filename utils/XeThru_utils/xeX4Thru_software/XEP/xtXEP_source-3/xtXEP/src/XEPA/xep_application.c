/**
 * @file
 *
 *
 */

#include "xep_application.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include <task.h>
#include <queue.h>
#include "xtcompiler.h"
#include "protocol.h"
#include "xep_dispatch_messages.h"
#include "module_ui.h"
#include "xep.h"
#include "xep_hal.h"
#include "xtmemory.h"
#include "protocol_parser.h"
#include "xep_application_mcp_callbacks.h"
#include "string.h"
#include "xttoolbox.h"
#include "string.h"
#include "version.h"
#include "task_monitor.h"
#include "xthousekeeping.h"
#include "xtfile.h"

StaticTask_t app_task_buffer;
StackType_t* app_task_stack;

#define TASK_APP_STACK_SIZE            (3000)
#define TASK_APP_NOSDRAM_STACK_SIZE    (1000)
#define TASK_APP_QUEUE_SIZE            (200)
#define TASK_APP_NOSDRAM_QUEUE_SIZE    (20)

#define TASK_APP_PRIORITY        (tskIDLE_PRIORITY + 3)

#define TASK_UITICK_STACK_SIZE            (100)
#define TASK_UITICK_PRIORITY        (tskIDLE_PRIORITY + 6)

int xep_init_memorypoolset(XepHandle_t* xep) __attribute__ ((weak, alias("xep_init_memorypoolset_default")));
int xep_app_init(XepHandle_t* xep) __attribute__ ((weak, alias("xep_app_init_default")));
int xep_app_iterate(XepHandle_t* xep, uint32_t notify) __attribute__ ((weak, alias("xep_app_iterate_default")));
int xep_app_100ms_tick(XepHandle_t* xep) __attribute__ ((weak, alias("xep_app_100ms_tick_default")));
int xep_app_storage_section(uint32_t* address, uint32_t* size) __attribute__ ((weak, alias("xep_app_storage_section_default")));
int xep_process_dispatch_message(XepHandle_t* xep, XepDispatchMessage_t* dispatch_message) __attribute__ ((weak, alias("xep_process_dispatch_message_default")));

int xep_init_memorypoolset_default(XepHandle_t* xep);
int xep_app_init_default(XepHandle_t* xep);
int xep_app_iterate_default(XepHandle_t* xep, uint32_t notify);
void xep_app_100ms_tick_default(XepHandle_t* xep);
void xep_app_storage_section_default(uint32_t* address, uint32_t* size);

static void task_application(void *pvParameters);
static void task_ui_tick(void *pvParameters);

typedef struct
{
    TaskHandle_t application_task_handle;

} XepInternalInfo_t;

int xep_debug_control(XepHandle_t* xep, uint32_t debug_control)
{
    XepInternalInfo_t* xep_internal_info = xep->internal;
    if (debug_control == XDC_APPLICATION_TASK_HIGH_PRIORITY)
    {
        vTaskPrioritySet(xep_internal_info->application_task_handle, tskIDLE_PRIORITY + 7);
    }
    else
    {
        vTaskPrioritySet(xep_internal_info->application_task_handle, TASK_APP_PRIORITY);
    }
    return 0;
}

int xep_get_systeminfo_versionlist(char* destination, uint32_t max_length)
{
    uint32_t add_length = strlen(PRODUCT_NAME) + 1 + strlen(VERSION_STRING) + 1 + strlen(X4C51FW_PRODUCT_NAME) + 1 + strlen(X4C51FW_VERSION_STRING);
    if (max_length < add_length) return 0;
    sprintf(destination, "%s:%s;%s:%s", PRODUCT_NAME, VERSION_STRING, X4C51FW_PRODUCT_NAME, X4C51FW_VERSION_STRING);
    return add_length;
}

uint32_t xep_init(XepDispatch_t** dispatch)
{
	int status = 0;
	uint32_t size = dispatch_get_instance_size();
	void* instance_memory = xtmemory_malloc_default(size);
	status = dispatch_create(dispatch, instance_memory);
	status = dispatch_set_notify_value(*dispatch, 0x10000000);

    // Set up memory poolset
    MemoryPoolSet_t* memorypoolset;
    size = memorypoolset_get_instance_size();
    instance_memory = xtmemory_malloc_default(size);
    status = memorypoolset_create(&memorypoolset, instance_memory);

	(*dispatch)->memorypoolset = memorypoolset;

	return status;
}

uint32_t task_application_init(XepDispatch_t* dispatch, X4Driver_t* x4driver)
{
    // Initialize file system
    uint32_t address;
    uint32_t size;
    xep_app_storage_section(&address, &size);
    xtfile_init(dispatch, address, size);

    // Initialize parser and callbacks
    XepHandle_t* xep = xtmemory_malloc_default(sizeof(XepHandle_t));
    xep->dispatch = dispatch;
    xep->x4driver = x4driver;
    xep->user_reference = NULL;
    xep->internal = xtmemory_malloc_default(sizeof(XepInternalInfo_t));
    void* instance_memory = xtmemory_malloc_default(getMcpParserInstanceSize());
    xep->mcp_parser = createMcpParser((void*)xep, instance_memory);
    xep->mcp_parser->user_data = (void*)xep;

    xep->mcp_parser->onMcpPing = onMcpPing;
    xep->mcp_parser->onMcpUnknown = onMcpUnknown;
    xep->mcp_parser->onMcpSystemRunTest = onMcpSystemRunTest;
    xep->mcp_parser->onMcpModuleReset = onMcpModuleReset;
    xep->mcp_parser->onMcpStartBootloader = onMcpStartBootloader;
    xep->mcp_parser->onMcpGetSystemInfo = onMcpGetSystemInfo;
    xep->mcp_parser->onMcpGetCrashDumpLength = onMcpGetCrashDumpLength;
    xep->mcp_parser->onMcpGetCrashDump = onMcpGetCrashDump;
    xep->mcp_parser->onMcpClearCrashDump = onMcpClearCrashDump;
    xep->mcp_parser->onMcpSetBaudrate = onMcpSetBaudrate;

    xep->mcp_parser->onMcpSetIOPinControl = onMcpSetIOPinControl;
    xep->mcp_parser->onMcpSetIOPinValue = onMcpSetIOPinValue;
    xep->mcp_parser->onMcpGetIOPinValue = onMcpGetIOPinValue;

    /* file API*/
    xep->mcp_parser->onMcpFindAllFiles = onMcpFindAllFiles;
    xep->mcp_parser->onMcpCreateNewFile = onMcpCreateFile;
    xep->mcp_parser->onMcpOpenFile = onMcpOpenFile;
    xep->mcp_parser->onMcpDeleteFile = onMcpDeleteFile;
    xep->mcp_parser->onMcpFormatFilesystem = onMcpFormatFilesystem;
    xep->mcp_parser->onMcpCommitFile = onMcpCommitFile;
    xep->mcp_parser->onMcpGetFileLength = onMcpGetFileLength;
    xep->mcp_parser->onMcpSearchFileType = onMcpSearchFileType;
    xep->mcp_parser->onMcpGetFileData = onMcpGetFileData;
    xep->mcp_parser->onMcpSetFileData = onMcpSetFileData;

    xep->mcp_parser->onMcpX4DriverSetFps = onMcpX4DriverSetFps;
    xep->mcp_parser->onMcpX4DriverSetIterations = onMcpX4DriverSetIterations;
    xep->mcp_parser->onMcpX4DriverGetIterations = onMcpX4DriverGetIterations;
    xep->mcp_parser->onMcpX4DriverSetPulsesPerStep = onMcpX4DriverSetPulsesPerStep;
    xep->mcp_parser->onMcpX4DriverGetPulsesPerStep = onMcpX4DriverGetPulsesPerStep;
    xep->mcp_parser->onMcpX4DriverSetDownconversion = onMcpX4DriverSetDownconversion;
    xep->mcp_parser->onMcpX4DriverGetDownconversion = onMcpX4DriverGetDownconversion;
    xep->mcp_parser->onMcpX4DriverSetFrameArea = onMcpX4DriverSetFrameArea;
    xep->mcp_parser->onMcpX4DriverInit = onMcpX4DriverInit;
    xep->mcp_parser->onMcpX4DriverSetDacStep = onMcpX4DriverSetDacStep;
    xep->mcp_parser->onMcpX4DriverGetDacStep = onMcpX4DriverGetDacStep;
    xep->mcp_parser->onMcpX4DriverSetDacMin = onMcpX4DriverSetDacMin;
    xep->mcp_parser->onMcpX4DriverGetDacMin = onMcpX4DriverGetDacMin;
    xep->mcp_parser->onMcpX4DriverSetDacMax = onMcpX4DriverSetDacMax;
    xep->mcp_parser->onMcpX4DriverGetDacMax = onMcpX4DriverGetDacMax;
    xep->mcp_parser->onMcpX4DriverSetFrameAreaOffset = onMcpX4DriverSetFrameAreaOffset;
    xep->mcp_parser->onMcpX4DriverSetEnable = onMcpX4DriverSetEnable;
    xep->mcp_parser->onMcpX4DriverSetTxCenterFrequency = onMcpX4DriverSetTxCenterFrequency;
    xep->mcp_parser->onMcpX4DriverGetTxCenterFrequency = onMcpX4DriverGetTxCenterFrequency;
    xep->mcp_parser->onMcpX4DriverSetTxPower = onMcpX4DriverSetTxPower;
    xep->mcp_parser->onMcpX4DriverGetTxPower = onMcpX4DriverGetTxPower;
    xep->mcp_parser->onMcpX4DriverGetFrameBinCount = onMcpX4DriverGetFrameBinCount;
    xep->mcp_parser->onMcpX4DriverGetFps = onMcpX4DriverGetFps;
    xep->mcp_parser->onMcpX4DriverSetSpiRegister = onMcpX4DriverSetSpiRegister;
    xep->mcp_parser->onMcpX4DriverGetSpiRegister = onMcpX4DriverGetSpiRegister;
    xep->mcp_parser->onMcpX4DriverWriteToSpiRegister = onMcpX4DriverWriteToSpiRegister;
    xep->mcp_parser->onMcpX4DriverReadFromSpiRegister = onMcpX4DriverReadFromSpiRegister;
    xep->mcp_parser->onMcpX4DriverSetPifRegister = onMcpX4DriverSetPifRegister;
    xep->mcp_parser->onMcpX4DriverGetPifRegister = onMcpX4DriverGetPifRegister;
    xep->mcp_parser->onMcpX4DriverSetXifRegister = onMcpX4DriverSetXifRegister;
    xep->mcp_parser->onMcpX4DriverGetXifRegister = onMcpX4DriverGetXifRegister;
    xep->mcp_parser->onMcpX4DriverSetPrfDiv = onMcpX4DriverSetPrfDiv;
    xep->mcp_parser->onMcpX4DriverGetPrfDiv = onMcpX4DriverGetPrfDiv;
    xep->mcp_parser->onMcpX4DriverGetFrameArea = onMcpX4DriverGetFrameArea;
    xep->mcp_parser->onMcpX4DriverGetFrameAreaOffset = onMcpX4DriverGetFrameAreaOffset;

    int status = xep_init_memorypoolset((void*)xep);
    UNUSED(status);

    TaskHandle_t app_task_handle;
    if (xt_external_memory_enabled())
    {
        app_task_stack =
            (uint32_t*)xtmemory_malloc_fast(TASK_APP_STACK_SIZE*4);
        xTaskCreateStatic(task_application, (const char * const) "App",
                          TASK_APP_STACK_SIZE, (void*)xep, TASK_APP_PRIORITY,
                          app_task_stack, &app_task_buffer);
    }
    else
    {
        xTaskCreate(task_application, (const char * const) "App",
                    TASK_APP_NOSDRAM_STACK_SIZE, (void*)xep, TASK_APP_PRIORITY,
                    &app_task_handle);
    }
    XepInternalInfo_t* xep_internal_info = (XepInternalInfo_t*)xep->internal;
    xep_internal_info->application_task_handle = (TaskHandle_t)&app_task_buffer;

	return 0;
}

static void task_application(void *pvParameters)
{
    XepHandle_t* xep = (XepHandle_t*)pvParameters;

    uint32_t status;
    UNUSED(status);

    QueueHandle_t dispatch_queue;
    void* lock;
    uint32_t queue_size;
    if (xt_external_memory_enabled())
    {
        queue_size = TASK_APP_QUEUE_SIZE;
    }
    else
    {
        queue_size = TASK_APP_NOSDRAM_QUEUE_SIZE;
    }

    dispatch_register(&dispatch_queue, &lock, xep->dispatch, queue_size);
    dispatch_subscribe(xep->dispatch, XEP_DISPATCH_MESSAGETAG_HOSTCOM_RX, (void*)dispatch_queue, lock);
    dispatch_subscribe(xep->dispatch, XEP_DISPATCH_MESSAGETAG_HOSTCOM_RX_DEBUG, (void*)dispatch_queue, lock);
    dispatch_subscribe(xep->dispatch, XEP_DISPATCH_MESSAGETAG_RADAR_DATA, (void*)dispatch_queue, lock);

    // Add delay to make sure comms are up.
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    monitor_task_t * monitor_task_handle;
    status = monitor_task_register(&monitor_task_handle, 5000, TASK_APP_STACK_SIZE);

    // Send system booting message
    dispatch_message_hostcom_send_system(xep->dispatch, XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND, XTS_SPRS_BOOTING);

    bool safe_mode = xthousekeeping_in_safe_mode();

    if (!safe_mode)
        xep_app_init(xep);

    TaskHandle_t uitick_task_handle;


    if (xt_get_operation_mode() == XT_OPMODE_CERTIFICATION)
    {
        module_ui_led_set_color(1,1,0);
    } else
    {
        if (!safe_mode)
            xTaskCreate(task_ui_tick, (const char * const) "AppUITick", TASK_UITICK_STACK_SIZE, (void*)xep, TASK_UITICK_PRIORITY, &uitick_task_handle);
        else
            module_ui_led_set_color(1, 1, 1);
    }

    // Send system ready message
    dispatch_message_hostcom_send_system(xep->dispatch, XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND, XTS_SPRS_READY);

    XepDispatchMessage_t dispatch_message;
    uint32_t notify_value;
    for (;;)
    {
        status = monitor_task_alive(monitor_task_handle);

        xTaskNotifyWait( 0x00,      /* Don't clear any notification bits on entry. */
            0xffffffff, /* Reset the notification value to 0 on exit. */
            &notify_value, /* Notified value pass out. */
            500 / portTICK_PERIOD_MS );  /* Block indefinitely. */

        if (notify_value & xep->dispatch->notify_value)
        {
            // Message from dispatch
            while ( xQueueReceive( dispatch_queue, &dispatch_message, 0 ) )
            {
                if (!safe_mode)
                    xep_process_dispatch_message(xep, &dispatch_message);
                else
                    xep_process_dispatch_message_default(xep, &dispatch_message);

                status = monitor_task_alive(monitor_task_handle);
            }
        }
        if (notify_value == 0) // Timeout
        {

        }
        xep_app_iterate(xep, notify_value);
    }
}

static void task_ui_tick(void *pvParameters)
{
    XepHandle_t* xep = (XepHandle_t*)pvParameters;

    TickType_t last_wake_time = xTaskGetTickCount();
    for (;;)
    {
        xep_app_100ms_tick(xep);
        vTaskDelayUntil(&last_wake_time, 100 / portTICK_PERIOD_MS);
    }
}


int xep_init_memorypoolset_default(XepHandle_t* xep)
{
	// Default memory pool set. Facilitate basic dispatch messages, and radar data to host.
	int status = 0;
	uint32_t block_size;
	uint32_t block_count;
	uint32_t size;
	void* instance_memory;

    block_size = 7000;
    block_count = 2;
    size = memorypool_get_instance_size(block_size, block_count);
    instance_memory = xtmemory_malloc_slow(size);
    status = memorypoolset_add_memorypool(NULL, xep->dispatch->memorypoolset, instance_memory, block_size, block_count);

    block_size = 100;
    block_count = 10;
    size = memorypool_get_instance_size(block_size, block_count);
    instance_memory = xtmemory_malloc_slow(size);
    status = memorypoolset_add_memorypool(NULL, xep->dispatch->memorypoolset, instance_memory, block_size, block_count);

	return status;
}

int xep_app_init_default(XepHandle_t* xep)
{
    // Initialize housekeeping
	char xep_list[60];
	xep_get_systeminfo_versionlist(xep_list, 60);
	
	xthousekeeping_init(xep->dispatch, xep_list);
	
    // Green LED
    module_ui_led_set_color(0, 1, 0);
	return 0;
}

void xep_app_storage_section_default(uint32_t* address, uint32_t* size)
{
    *address = 0x4E0000;
    *size = 0x18000;
    if (xtio_get_flash_size() == 2*1024*1024)
    {
        *address = 0x5B0000;
        *size = 0x48000;
    }
}


int xep_app_iterate_default(XepHandle_t* xep, uint32_t notify)
{
    return 0;
}

void xep_app_100ms_tick_default(XepHandle_t* xep)
{
    static uint32_t led_state = 1;
    if ((xep->x4driver->initialized == 0) && led_state)
    {
        led_state = 0;
        module_ui_led_set_color(1, 0, 0);
    } else if (xep->x4driver->initialized && (led_state == 0))
    {
        led_state = 1;
        module_ui_led_set_color(0, 1, 0);
    }
}

int xep_process_dispatch_message_default(XepHandle_t* xep, XepDispatchMessage_t* dispatch_message)
{
    if ((dispatch_message->tag == XEP_DISPATCH_MESSAGETAG_HOSTCOM_RX) || (dispatch_message->tag == XEP_DISPATCH_MESSAGETAG_HOSTCOM_RX_DEBUG))
    {
        xep->route = (dispatch_message->tag == XEP_DISPATCH_MESSAGETAG_HOSTCOM_RX) ? 
                        XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND : 
                        XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND_DEBUG;
        mcpParseMessage(xep->mcp_parser, (uint8_t*)dispatch_message->memoryblock->buffer, dispatch_message->length);
        dispatch_release_message(xep->dispatch, dispatch_message);
    }
    else if (dispatch_message->tag == XEP_DISPATCH_MESSAGETAG_RADAR_DATA)
    {
        XepDispatchMessageContentCommon_t* message_common = (XepDispatchMessageContentCommon_t*)dispatch_message->memoryblock->buffer;
        if ((message_common->content_ref == XDMCR_RADARDATA_FRAME) && (dispatch_message->length == message_common->message_size))
        {
            XepDispatchMessageContentRadardataFramePacket_t* frame_packet = (XepDispatchMessageContentRadardataFramePacket_t*)dispatch_message->memoryblock->buffer;
            UNUSED(frame_packet);

            dispatch_message_hostcom_forward_radardata_frame_packet(xep->dispatch, XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND, dispatch_message);
            module_ui_led_set_color(0, frame_packet->framecounter%2, 0);
        }
        else
        {
            dispatch_release_message(xep->dispatch, dispatch_message);
        }
    }
    else
    {
        dispatch_release_message(xep->dispatch, dispatch_message);
    }

    return 0;
}
