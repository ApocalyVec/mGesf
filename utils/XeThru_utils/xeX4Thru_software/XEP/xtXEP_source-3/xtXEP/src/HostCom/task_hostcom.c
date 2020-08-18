/**
 * @file
 *
 *
 */

#include "task_hostcom.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "xtcompiler.h"
#include "compiler.h"
#include "protocol_target.h"
#include <string.h>
#include <arm_math.h>
#include "xep_dispatch_messages.h"
#include "xep_hal.h"
#include "xtmemory.h"
#include "xttoolbox.h"
#include "xtsemaphore.h"
#include "task_monitor.h"

#define TASK_HOSTCOM_STACK_SIZE            (1000)
#define TASK_HOSTCOM_NOSDRAM_STACK_SIZE    (300)
#define TASK_HOSTCOM_PRIORITY        (tskIDLE_PRIORITY + 4)

#define DISPATCH_QUEUE_SIZE         50
#define DISPATCH_NOSDRAM_QUEUE_SIZE 3

#define MESSAGEBUILD_BUFFER_SIZE 8000
#define INCOMING_BUFFER_SIZE 8000

// baseband iq reuses memorypool block, so no need to think of it here
#define MESSAGEBUILD_NOSDRAM_BUFFER_SIZE 600
#define INCOMING_NOSDRAM_BUFFER_SIZE 100

typedef struct {
    XepDispatch_t* dispatch;
    uint32_t send_tag;
    uint32_t receive_tag;
    xtio_serial_init_t serial_init;
    xtio_host_send_t send_to_host;
} HostcomParams_t;

static void task_hostcom(void *pvParameters);

static void message_send_completed_callback(void* pArg);
static void flush_message_queue(XepDispatch_t* dispatch, QueueHandle_t dispatch_queue);

static uint32_t process_dispatch_message_hostcom_send(HostcomParams_t* hostcom_params, XepDispatchMessage_t* dispatch_message, XepHostComMCPUserReference_t* mcp_user_reference);
static void mcp_handle_protocol_packet(void * userData, const unsigned char * data, unsigned int length);
static void mcp_handle_protocol_error(void * userData, unsigned int error);
static void mcp_protocol_messagebuild_callback(unsigned char byte, void * user_data);

uint32_t task_hostcom_init(XepDispatch_t* dispatch)
{
    uint32_t stack_size =
        xt_external_memory_enabled() ? TASK_HOSTCOM_STACK_SIZE
                                     : TASK_HOSTCOM_NOSDRAM_STACK_SIZE;

    // Spawn primary hostcom task
    TaskHandle_t h_task_hostcom;
    HostcomParams_t *hostcom_params =
        xtmemory_malloc_default(sizeof(HostcomParams_t));
    hostcom_params->dispatch = dispatch;
    hostcom_params->send_tag = XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND;
    hostcom_params->receive_tag = XEP_DISPATCH_MESSAGETAG_HOSTCOM_RX;
    hostcom_params->serial_init = &xtio_serial_com_init;
    hostcom_params->send_to_host = &xtio_host_send;
    xTaskCreate(task_hostcom, (const char * const) "HostCom",
                stack_size, (void*)hostcom_params,
                TASK_HOSTCOM_PRIORITY, &h_task_hostcom);

    if (xt_external_memory_enabled())
    {
        // Spawn debug hostcom task
        TaskHandle_t h_task_dhostcom;
        HostcomParams_t *dhostcom_params =
            xtmemory_malloc_default(sizeof(HostcomParams_t));
        dhostcom_params->dispatch = dispatch;
        dhostcom_params->send_tag = XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND_DEBUG;
        dhostcom_params->receive_tag = XEP_DISPATCH_MESSAGETAG_HOSTCOM_RX_DEBUG;
        dhostcom_params->serial_init = &xtio_serial_com_debug_init;
        dhostcom_params->send_to_host = &xtio_host_send_debug;
        xTaskCreate(task_hostcom, (const char * const) "dHostCom",
                    stack_size, (void*)dhostcom_params,
                    TASK_HOSTCOM_PRIORITY, &h_task_dhostcom);
    }

    return 0;
}

static void task_hostcom(void *pvParameters)
{
    HostcomParams_t* hostcom_params = (HostcomParams_t*)pvParameters;;
    XepDispatch_t* dispatch = hostcom_params->dispatch;

    uint32_t messbuflen, inbuflen, queue_size;
    if (xt_external_memory_enabled())
    {
        messbuflen = MESSAGEBUILD_BUFFER_SIZE;
        inbuflen = INCOMING_BUFFER_SIZE;
        queue_size = DISPATCH_QUEUE_SIZE;
    }
    else
    {
        messbuflen = MESSAGEBUILD_NOSDRAM_BUFFER_SIZE;
        inbuflen = INCOMING_NOSDRAM_BUFFER_SIZE;
        queue_size = DISPATCH_NOSDRAM_QUEUE_SIZE;
    }

    QueueHandle_t dispatch_queue;
    void* lock;
    dispatch_register(&dispatch_queue, &lock, dispatch, queue_size);
    dispatch_subscribe(dispatch, hostcom_params->send_tag, (void*)dispatch_queue, lock);

    // Create protocol instance
    unsigned char mcp_instance_memory[getInstanceSize()];


    void* messagebuild_buffer = xtmemory_malloc_default(messbuflen);
    void* incoming_message_buffer = xtmemory_malloc_slow(inbuflen);

    XepHostComMCPUserReference_t mcp_user_reference;
    mcp_user_reference.hostcom_task_handle = xTaskGetCurrentTaskHandle();
    mcp_user_reference.dispatch = dispatch;
    mcp_user_reference.message_noescape_memoryblock = NULL;
    mcp_user_reference.messagebuild_buffer = messagebuild_buffer;
    mcp_user_reference.messagebuild_index = 0;
    mcp_user_reference.messagebuild_length = messbuflen;
    mcp_user_reference.messagebuild_send_semaphore = xSemaphoreCreateBinary();
    mcp_user_reference.message_tag = hostcom_params->receive_tag;
    xSemaphoreGive(mcp_user_reference.messagebuild_send_semaphore);
    xtProtocol* mcp_protocol_instance =
        createApplicationProtocol(&mcp_handle_protocol_packet,
                                  &mcp_handle_protocol_error,
                                  (void*)&mcp_user_reference,
                                  mcp_instance_memory, incoming_message_buffer,
                                  inbuflen);
    UNUSED(mcp_protocol_instance);

    hostcom_params->serial_init(mcp_protocol_instance);

    monitor_task_t * monitor_task_handle;
    int status = monitor_task_register(&monitor_task_handle, 3000, TASK_HOSTCOM_STACK_SIZE);
    UNUSED(status);

	XepDispatchMessage_t dispatch_message;
	uint32_t notify_value;
	for (;;)
	{
		xTaskNotifyWait( 0x00,      /* Don't clear any notification bits on entry. */
			0xffffffff, /* Reset the notification value to 0 on exit. */
			&notify_value, /* Notified value pass out. */
			500 / portTICK_PERIOD_MS );  /* Block indefinitely. */

        status = monitor_task_alive(monitor_task_handle);

		if (notify_value & dispatch->notify_value)
		{
			while (xQueuePeek( dispatch_queue, &dispatch_message, 0) )
			{
                status = monitor_task_alive(monitor_task_handle);
				if (dispatch_message.tag == hostcom_params->send_tag)
				{
					if (xSemaphoreTake(mcp_user_reference.messagebuild_send_semaphore, 1000 / portTICK_PERIOD_MS) == pdTRUE)
					{
						process_dispatch_message_hostcom_send(hostcom_params, &dispatch_message, &mcp_user_reference);
						xQueueReceive( dispatch_queue, &dispatch_message, 0);
					} else
                    {
                        // Semaphore haven't been given back within time limit.
                        // Indicates that no one is listening (for USB case at least)
                        // Let's flush uninteresting data
                        do
                        {
                            status = monitor_task_alive(monitor_task_handle);
                            if (xtlock_lock(lock, 10 / portTICK_PERIOD_MS) == XTLOCK_OK)
                            {
                                flush_message_queue(dispatch, dispatch_queue);
                                xtlock_unlock(lock);
                            }
                        } while(xSemaphoreTake(mcp_user_reference.messagebuild_send_semaphore, 10) != pdTRUE);
                        xSemaphoreGive(mcp_user_reference.messagebuild_send_semaphore); // We took the semaphore, need to give it again
                    }
				} else
                {
					xQueueReceive( dispatch_queue, &dispatch_message, 0);
                }
            }
		}
		if (notify_value == 0) // Timeout
		{

		}
	}
}

static void flush_message_queue(XepDispatch_t* dispatch, QueueHandle_t dispatch_queue)
{
    int remaining_space = uxQueueSpacesAvailable(dispatch_queue);
    if (remaining_space > (DISPATCH_QUEUE_SIZE/4))
        return;

    int messages_in_queue = uxQueueMessagesWaiting(dispatch_queue);

	XepDispatchMessage_t dispatch_message;
    for (int i = 0; i < messages_in_queue; i++)
    {
        // Fetch from queue
        xQueueReceive(dispatch_queue, &dispatch_message, 0);

        XepDispatchMessageContentCommon_t* message_common = (XepDispatchMessageContentCommon_t*)(dispatch_message.memoryblock->buffer);

        // Check if message should be kept for later
        if ((message_common->content_ref == XDMCR_HOSTCOM_DATA_STRING) ||
            (message_common->content_ref == XDMCR_HOSTCOM_DATA_BYTE) ||
            (message_common->content_ref == XDMCR_HOSTCOM_DATA_FLOAT))
        {
            dispatch_release_message(dispatch, &dispatch_message);
        } else
        {
            xQueueSendToBack(dispatch_queue, &dispatch_message, 0);
        }
    }

    // We should have managed to have at least a quarter of the queue free, if not we have to delete the oldest until we do. 
    messages_in_queue = uxQueueMessagesWaiting(dispatch_queue);
    int free_space_in_queue = DISPATCH_QUEUE_SIZE - messages_in_queue;
    for (int i = free_space_in_queue; i < (DISPATCH_QUEUE_SIZE/4); i++)
    {
		xQueueReceive(dispatch_queue, &dispatch_message, 0);
		dispatch_release_message(dispatch, &dispatch_message);
    }
}

static void message_send_completed_callback(void* pArg)
{
    XepHostComMCPUserReference_t *u = (XepHostComMCPUserReference_t*) pArg;
    if (u->message_noescape_memoryblock) {
        memorypool_free(u->message_noescape_memoryblock);
        u->message_noescape_memoryblock = NULL;
    }
    xSemaphoreGive(u->messagebuild_send_semaphore);
}

static uint32_t process_dispatch_message_hostcom_send(HostcomParams_t* hostcom_params, XepDispatchMessage_t* dispatch_message, XepHostComMCPUserReference_t* mcp_user_reference)
{
    XepDispatch_t* dispatch = hostcom_params->dispatch;
    XepDispatchMessageContentCommon_t* message_common = (XepDispatchMessageContentCommon_t*)dispatch_message->memoryblock->buffer;
	if ((dispatch_message->length == sizeof(XepDispatchMessageContentHostcomAck_t)) && (message_common->content_ref == XDMCR_HOSTCOM_ACK))
	{
		mcp_user_reference->messagebuild_index = 0;
		createAckCommand(mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
		hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
	}
	else if ((dispatch_message->length == sizeof(XepDispatchMessageContentHostcomError_t)) && (message_common->content_ref == XDMCR_HOSTCOM_ERROR))
	{
        XepDispatchMessageContentHostcomError_t* message_content = (XepDispatchMessageContentHostcomError_t*)dispatch_message->memoryblock->buffer;
		mcp_user_reference->messagebuild_index = 0;
		createErrorCommand(message_content->errorcode, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
		hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
	}
	else if ((dispatch_message->length == sizeof(XepDispatchMessageContentHostcomPong_t)) && (message_common->content_ref == XDMCR_HOSTCOM_PONG))
	{
		XepDispatchMessageContentHostcomPong_t* message_content = (XepDispatchMessageContentHostcomPong_t*)dispatch_message->memoryblock->buffer;
		mcp_user_reference->messagebuild_index = 0;
		createPongCommand(message_content->pongval, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
		hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
	}
    else if ((dispatch_message->length == sizeof(XepDispatchMessageContentHostcomSystem_t)) && (message_common->content_ref == XDMCR_HOSTCOM_SYSTEM))
    {
        XepDispatchMessageContentHostcomSystem_t* message_content = (XepDispatchMessageContentHostcomSystem_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createSystemCommand(message_content->contentid, NULL, 0, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_DATA_STRING) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomDataString_t* message_content = (XepDispatchMessageContentHostcomDataString_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createDataStringnCommand(message_content->contentid, message_content->info, message_content->data, message_content->length, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_DATA_BYTE) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomDataByte_t* message_content =
            (XepDispatchMessageContentHostcomDataByte_t*)dispatch_message->memoryblock->buffer;

        uint8_t *reverse_injector = (uint8_t*)message_content->data;
        uint32_t total_length;

        uint32_t status = createDataByteCommandNoEscape(
                &reverse_injector,
                sizeof(XepDispatchMessageContentHostcomDataByte_t),
                &total_length,
                message_content->contentid,
                message_content->info,
                message_content->length);

        if (status == 0) {
            mcp_user_reference->message_noescape_memoryblock =
                dispatch_message->memoryblock;
            dispatch_message->memoryblock = NULL;

            hostcom_params->send_to_host(
                reverse_injector,
                total_length,
                message_send_completed_callback,
                mcp_user_reference);
        }
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_DATA_FLOAT) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomDataFloat_t* message_content = (XepDispatchMessageContentHostcomDataFloat_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createDataFloatCommand(message_content->contentid, message_content->info, message_content->data, message_content->length, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_REPLY_BYTE) && (dispatch_message->length == message_common->message_size))
    {
	    XepDispatchMessageContentHostcomReplyByte_t* message_content = (XepDispatchMessageContentHostcomReplyByte_t*)dispatch_message->memoryblock->buffer;
	    mcp_user_reference->messagebuild_index = 0;
	    createReplyByteCommand(message_content->contentid, message_content->info, message_content->data, message_content->length, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
	    hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_REPLY_INT) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomReplyInt_t* message_content = (XepDispatchMessageContentHostcomReplyInt_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createReplyIntCommand(message_content->contentid, message_content->info, message_content->data, message_content->length, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_REPLY_STRING) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomReplyString_t* message_content = (XepDispatchMessageContentHostcomReplyString_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createReplyStringnCommand(message_content->contentid, message_content->info, message_content->data, message_content->length, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_REPLY_FLOAT) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomReplyFloat_t* message_content = (XepDispatchMessageContentHostcomReplyFloat_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createReplyFloatCommand(message_content->contentid, message_content->info, message_content->data, message_content->length, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_SLEEP) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataSleep_t* message_content = (XepDispatchMessageContentHostcomAppdataSleep_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createAppdataSleepCommand(message_content->counter, message_content->state_code, message_content->state_data, message_content->distance, message_content->signal_quality, message_content->movement_slow, message_content->movement_fast, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_VITALSIGNS) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataVitalSigns_t* message_content = (XepDispatchMessageContentHostcomAppdataVitalSigns_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createAppdataVitalSignsCommand(message_content->counter, message_content->state_code, message_content->respiration_rate, message_content->respiration_distance, message_content->respiration_confidence, message_content->heart_rate, message_content->heart_distance,message_content->heart_confidence,message_content->movement_power_slow,message_content->movement_power_fast,message_content->movement_power_start,message_content->movement_power_end, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_RESPIRATION) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataRespiration_t* message_content = (XepDispatchMessageContentHostcomAppdataRespiration_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createAppdataRespirationCommand(message_content->counter, message_content->state_code, message_content->state_data, message_content->distance, message_content->movement, message_content->signal_quality, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_RESPIRATION_MOVINGLIST) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataRespirationMovinglist_t* message_content =
            (XepDispatchMessageContentHostcomAppdataRespirationMovinglist_t*)
                dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createAppdataRespirationMovinglistCommand(
                message_content->counter,
                message_content->interval_count,
                message_content->movement_slow_item,
                message_content->movement_fast_item,
                mcp_protocol_messagebuild_callback,
                (void*)mcp_user_reference);
        hostcom_params->send_to_host(
                mcp_user_reference->messagebuild_buffer,
                mcp_user_reference->messagebuild_index,
                message_send_completed_callback,
                mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_RESPIRATION_DETECTIONLIST) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataRespirationDetectionlist_t* message_content =
            (XepDispatchMessageContentHostcomAppdataRespirationDetectionlist_t*)
                dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createAppdataRespirationDetectionlistCommand(
                message_content->counter,
                message_content->detection_count,
                message_content->detection_distance_items,
                message_content->detection_radar_cross_section_items,
                message_content->detection_velocity_items,
                mcp_protocol_messagebuild_callback,
                (void*)mcp_user_reference);
        hostcom_params->send_to_host(
                mcp_user_reference->messagebuild_buffer,
                mcp_user_reference->messagebuild_index,
                message_send_completed_callback,
                mcp_user_reference);
    }
    else if ((message_common->content_ref ==
              XDMCR_HOSTCOM_APPDATA_RESPIRATION_NORMALIZEDMOVEMENTLIST) &&
             (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataRespirationNormalizedMovementlist_t* message_content =
            (XepDispatchMessageContentHostcomAppdataRespirationNormalizedMovementlist_t*)
                dispatch_message->memoryblock->buffer;

        mcp_user_reference->messagebuild_index = 0;
        createAppdataRespirationNormalizedmovementlistCommand(
                message_content->frame_counter,
                message_content->start,
                message_content->bin_length,
                message_content->count,
                message_content->normalized_movement_slow_items,
                message_content->normalized_movement_fast_items,
                mcp_protocol_messagebuild_callback,
                (void*)mcp_user_reference);
        hostcom_params->send_to_host(
                mcp_user_reference->messagebuild_buffer,
                mcp_user_reference->messagebuild_index,
                message_send_completed_callback,
                mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_PRESENCE_SINGLE) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataPresenceSingle_t* message_content = (XepDispatchMessageContentHostcomAppdataPresenceSingle_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createAppdataPresenceSingleCommand(message_content->counter, message_content->presence_state, message_content->distance, message_content->direction,  message_content->signal_quality, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_PRESENCE_MOVINGLIST) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataPresenceMovinglist_t* message_content = (XepDispatchMessageContentHostcomAppdataPresenceMovinglist_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createAppdataPresenceMovinglistCommand(message_content->counter, message_content->presence_state, message_content->movement_interval_count, message_content->detection_count, message_content->movement_slow_item, message_content->movement_fast_item, message_content->detection_distance, message_content->detection_radar_cross_section, message_content->detection_velocity, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_PROFILEPARAMETERFILE) && (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataProfileParameterFile_t* message_content = (XepDispatchMessageContentHostcomAppdataProfileParameterFile_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;
        createAppdataProfileParameterFileCommand(message_content->filename_length, message_content->data_length, message_content->filename, message_content->data, mcp_protocol_messagebuild_callback, (void*)mcp_user_reference);
        hostcom_params->send_to_host(mcp_user_reference->messagebuild_buffer, mcp_user_reference->messagebuild_index, message_send_completed_callback, mcp_user_reference);
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_PULSEDOPPLER_FLOAT) && (dispatch_message->length == message_common->message_size)) {
        XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t* message_content =
            (XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t*)dispatch_message->memoryblock->buffer;

        uint8_t *reverse_injector = (uint8_t*)message_content->pwr;
        uint32_t total_length;

        const uint32_t status = createAppdataPulseDopplerFloatCommandNoEscape(
            &reverse_injector,
            sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t),
            &total_length,
            message_content->counter,
            message_content->matrix_counter,
            message_content->range_idx,
            message_content->range_bins,
            message_content->freq_count,
            message_content->pd_instance,
            message_content->fps,
            message_content->fps_decimated,
            message_content->freq_start,
            message_content->freq_step,
            message_content->range);

        if (status == 0) {
            // Null the memory to free it ourselves later in the callback.
            mcp_user_reference->message_noescape_memoryblock =
                dispatch_message->memoryblock;
            dispatch_message->memoryblock = NULL;

            hostcom_params->send_to_host(
                reverse_injector,
                total_length,
                message_send_completed_callback,
                mcp_user_reference);
        }
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_NOISEMAP_FLOAT) && (dispatch_message->length == message_common->message_size)) {
        XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t* message_content =
            (XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t*)dispatch_message->memoryblock->buffer;

        uint8_t *reverse_injector = (uint8_t*)message_content->pwr;
        uint32_t total_length;

        const uint32_t status = createAppdataNoisemapFloatCommandNoEscape(
            &reverse_injector,
            sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t),
            &total_length,
            message_content->counter,
            message_content->matrix_counter,
            message_content->range_idx,
            message_content->range_bins,
            message_content->freq_count,
            message_content->pd_instance,
            message_content->fps,
            message_content->fps_decimated,
            message_content->freq_start,
            message_content->freq_step,
            message_content->range);

        if (status == 0) {
            mcp_user_reference->message_noescape_memoryblock =
                dispatch_message->memoryblock;
            dispatch_message->memoryblock = NULL;

            hostcom_params->send_to_host(
                reverse_injector,
                total_length,
                message_send_completed_callback,
                mcp_user_reference);
        }
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_PULSEDOPPLER_BYTE) && (dispatch_message->length == message_common->message_size)) {
        XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t* message_content =
            (XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t*)dispatch_message->memoryblock->buffer;

        uint8_t *reverse_injector = message_content->pwr;
        uint32_t total_length;

        const uint32_t status = createAppdataPulseDopplerByteCommandNoEscape(
            &reverse_injector,
            sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t),
            &total_length,
            message_content->counter,
            message_content->matrix_counter,
            message_content->range_idx,
            message_content->range_bins,
            message_content->freq_count,
            message_content->pd_instance,
            message_content->step_start,
            message_content->step_size,
            message_content->fps,
            message_content->fps_decimated,
            message_content->freq_start,
            message_content->freq_step,
            message_content->range);

        if (status == 0) {
            mcp_user_reference->message_noescape_memoryblock =
                dispatch_message->memoryblock;
            dispatch_message->memoryblock = NULL;

            hostcom_params->send_to_host(
                reverse_injector,
                total_length,
                message_send_completed_callback,
                mcp_user_reference);
        }
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_NOISEMAP_BYTE) && (dispatch_message->length == message_common->message_size)) {
        XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t* message_content =
            (XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t*)dispatch_message->memoryblock->buffer;

        uint8_t *reverse_injector = message_content->pwr;
        uint32_t total_length;

        const uint32_t status = createAppdataNoisemapByteCommandNoEscape(
            &reverse_injector,
            sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t),
            &total_length,
            message_content->counter,
            message_content->matrix_counter,
            message_content->range_idx,
            message_content->range_bins,
            message_content->freq_count,
            message_content->pd_instance,
            message_content->step_start,
            message_content->step_size,
            message_content->fps,
            message_content->fps_decimated,
            message_content->freq_start,
            message_content->freq_step,
            message_content->range);

        if (status == 0) {
            mcp_user_reference->message_noescape_memoryblock =
                dispatch_message->memoryblock;
            dispatch_message->memoryblock = NULL;

            hostcom_params->send_to_host(
                reverse_injector,
                total_length,
                message_send_completed_callback,
                mcp_user_reference);
        }
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_BASEBAND_AMPLITUDE_PHASE) &&
             (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataBasebandAmplitudePhase_t* message_content =
            (XepDispatchMessageContentHostcomAppdataBasebandAmplitudePhase_t*)dispatch_message->memoryblock->buffer;

        const bool use_escape = false;
        if (use_escape) {
            mcp_user_reference->messagebuild_index = 0;

            createAppdataBasebandAmplitudePhaseCommand(
                message_content->counter,
                message_content->num_of_bins,
                message_content->bin_length,
                message_content->sampling_frequency,
                message_content->carrier_frequency, message_content->range_offset,
                message_content->data_amplitude,
                message_content->data_phase,
                mcp_protocol_messagebuild_callback,
                (void*)mcp_user_reference);

            hostcom_params->send_to_host(
                mcp_user_reference->messagebuild_buffer,
                mcp_user_reference->messagebuild_index,
                message_send_completed_callback,
                mcp_user_reference);
        }
        else {

            const uint32_t counter = message_content->counter;
            const uint32_t num_of_bins = message_content->num_of_bins;
            const float bin_length = message_content->bin_length;
            const float sampling_frequency = message_content->sampling_frequency;
            const float carrier_frequency = message_content->carrier_frequency;
            const float range_offset = message_content->range_offset;

            uint32_t total_packet_length = 0;
            uint8_t * mcp_packet_data =
                ((uint8_t *) dispatch_message->memoryblock->buffer) +
                sizeof(XepDispatchMessageContentHostcomAppdataBasebandAmplitudePhase_t);

            const uint32_t status = createAppdataBasebandAPCommandNoEscape(
                &mcp_packet_data,
                sizeof(XepDispatchMessageContentHostcomAppdataBasebandAmplitudePhase_t),
                &total_packet_length,
                counter,
                num_of_bins,
                bin_length,
                sampling_frequency,
                carrier_frequency,
                range_offset);

            if (status == 0) {
                mcp_user_reference->message_noescape_memoryblock =
                    dispatch_message->memoryblock;
                dispatch_message->memoryblock = NULL;

                hostcom_params->send_to_host(
                    mcp_packet_data,
                    total_packet_length,
                    message_send_completed_callback,
                    mcp_user_reference); // StartSequence + Packet
            }
        }
    }
    else if ((message_common->content_ref == XDMCR_HOSTCOM_APPDATA_BASEBAND_IQ) &&
             (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentHostcomAppdataBasebandIQ_t* message_content =
            (XepDispatchMessageContentHostcomAppdataBasebandIQ_t*)dispatch_message->memoryblock->buffer;

        const bool use_escape = false;
        if (use_escape) {
            mcp_user_reference->messagebuild_index = 0;
            createAppdataBasebandIQCommand(
                message_content->counter,
                message_content->num_of_bins,
                message_content->bin_length,
                message_content->sampling_frequency,
                message_content->carrier_frequency,
                message_content->range_offset,
                message_content->signal_i,
                message_content->signal_q,
                mcp_protocol_messagebuild_callback,
                (void*)mcp_user_reference);

            hostcom_params->send_to_host(
                mcp_user_reference->messagebuild_buffer,
                mcp_user_reference->messagebuild_index,
                message_send_completed_callback,
                mcp_user_reference);
        }
        else {

            const uint32_t counter = message_content->counter;
            const uint32_t num_of_bins = message_content->num_of_bins;
            const float bin_length = message_content->bin_length;
            const float sampling_frequency = message_content->sampling_frequency;
            const float carrier_frequency = message_content->carrier_frequency;
            const float range_offset = message_content->range_offset;

            uint32_t total_packet_length = 0;
            uint8_t * mcp_packet_data =
                ((uint8_t *) dispatch_message->memoryblock->buffer) +
                sizeof(XepDispatchMessageContentHostcomAppdataBasebandIQ_t);

            const uint32_t status = createAppdataBasebandIQCommandNoEscape(
                &mcp_packet_data,
                sizeof(XepDispatchMessageContentHostcomAppdataBasebandIQ_t),
                &total_packet_length,
                counter,
                num_of_bins,
                bin_length,
                sampling_frequency,
                carrier_frequency,
                range_offset);

            if (status == 0) {
                mcp_user_reference->message_noescape_memoryblock =
                    dispatch_message->memoryblock;
                dispatch_message->memoryblock = NULL;

                hostcom_params->send_to_host(
                    mcp_packet_data,
                    total_packet_length,
                    message_send_completed_callback,
                    mcp_user_reference); // StartSequence + Packet
            }
        }
    }
    else if ((message_common->content_ref == XDMCR_RADARDATA_FRAME) &&
             (dispatch_message->length == message_common->message_size))
    {
        XepDispatchMessageContentRadardataFramePacket_t* message_content = (XepDispatchMessageContentRadardataFramePacket_t*)dispatch_message->memoryblock->buffer;
        mcp_user_reference->messagebuild_index = 0;

        const bool use_escape = false;
        if (use_escape) // Use for now. Switch to NoEscape when all are ready.
        {
            createDataFloatCommand(
                ID_RAW_FRAME_BUFFER,
                message_content->framecounter,
                message_content->framedata,
                message_content->bin_count,
                mcp_protocol_messagebuild_callback,
                (void*)mcp_user_reference);

            hostcom_params->send_to_host(
                mcp_user_reference->messagebuild_buffer,
                mcp_user_reference->messagebuild_index,
                message_send_completed_callback,
                mcp_user_reference);
        }
        else
        {
            // Create no eascape packet header and prepend to data. Memory already allocated.
            uint8_t* mcp_packet_data =
                ((uint8_t*) dispatch_message->memoryblock->buffer) +
                sizeof(XepDispatchMessageContentRadardataFramePacket_t);

            const uint32_t content_id = ID_RAW_FRAME_BUFFER;
            const uint32_t frame_counter = message_content->framecounter;
            const uint32_t bin_count = message_content->bin_count;
            uint32_t total_packet_length = 0;
            int status = createDataFloatCommandNoEscape(
                &mcp_packet_data,
                sizeof(XepDispatchMessageContentRadardataFramePacket_t),
                content_id,
                frame_counter,
                bin_count,
                &total_packet_length);

            if (status == 0) {
                mcp_user_reference->message_noescape_memoryblock =
                    dispatch_message->memoryblock;
                dispatch_message->memoryblock = NULL;

                hostcom_params->send_to_host(
                    mcp_packet_data,
                    total_packet_length,
                    message_send_completed_callback,
                    mcp_user_reference); // StartSequence + Packet
            }

        }
    }
    dispatch_release_message(dispatch, dispatch_message);
	return 0;
}

static void mcp_handle_protocol_packet(void * userData, const unsigned char * data, unsigned int length)
{
	XepHostComMCPUserReference_t* mcp_user_reference = (XepHostComMCPUserReference_t*)userData;

    MemoryBlock_t* memoryblock;
    if (XEP_ERROR_OK != dispatch_get_message_memoryblock(&memoryblock, mcp_user_reference->dispatch, length))
	{
		// TODO: Some error handling.
		return;
	}
    memcpy(memoryblock->buffer, data, length);
    dispatch_send_message(NULL, mcp_user_reference->dispatch, mcp_user_reference->message_tag, memoryblock, length);
}

static void mcp_handle_protocol_error(void * userData, unsigned int error)
{
	XepHostComMCPUserReference_t* mcp_user_reference = (XepHostComMCPUserReference_t*)userData;
	UNUSED(mcp_user_reference);

}

static void mcp_protocol_messagebuild_callback(unsigned char byte, void * user_data)
{
	XepHostComMCPUserReference_t* mcp_user_reference = (XepHostComMCPUserReference_t*)user_data;
	if (mcp_user_reference->messagebuild_index >= mcp_user_reference->messagebuild_length)
	{
		// ERROR. Buffer overflow.
		return;
	}
	mcp_user_reference->messagebuild_buffer[mcp_user_reference->messagebuild_index++] = byte;
}
