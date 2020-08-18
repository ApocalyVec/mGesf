/**
 * @file
 *
 * 
 */

#include "xep_dispatch.h"
#include "xtsemaphore.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <string.h>
#include <task.h>

uint32_t task_to_queue(void** queue, XepDispatch_t* dispatch, void* task);

uint32_t task_to_queue(void** queue, XepDispatch_t* dispatch, void* task)
{
	*queue = NULL;
	for (int i=0; i<XEP_DISPATCH_MESSAGETAG_COUNT; i++)
	{
		if (dispatch->subscribers[i].task == task)
		{
			*queue = dispatch->subscribers[i].queue;
			return XEP_ERROR_OK;
		}
	}
	return XEP_ERROR_NOT_FOUND;
}

uint32_t dispatch_get_instance_size(void)
{
	return sizeof(XepDispatch_t);
}

uint32_t dispatch_create(XepDispatch_t** dispatch, void* instance_memory)
{
	XepDispatch_t* d = (XepDispatch_t*)instance_memory;
	memset(d, 0, sizeof(XepDispatch_t));
	*dispatch = d;
	d->message_id_counter = 0;
	d->notify_value = 0;
    d->memorypoolset = NULL;
	return XEP_ERROR_OK;
}

uint32_t dispatch_set_notify_value(XepDispatch_t* dispatch, uint32_t notify_value)
{
	dispatch->notify_value = notify_value;
	return 0;
}

uint32_t dispatch_register(void** queue, void** lock, XepDispatch_t* dispatch, uint32_t queue_length)
{
	QueueHandle_t q = xQueueCreate(queue_length, sizeof(XepDispatchMessage_t));
	*queue = (void*)q;
	*lock = xtlock_create();
	return XEP_ERROR_OK;
}

uint32_t dispatch_subscribe(XepDispatch_t* dispatch, XepDispatchMessageTags_t message_tag, void* queue, void* lock)
{
	dispatch->subscribers[message_tag].queue = queue;
	dispatch->subscribers[message_tag].lock = lock;
	dispatch->subscribers[message_tag].task = (void*)xTaskGetCurrentTaskHandle();
	return XEP_ERROR_OK;
}

uint32_t dispatch_get_message_memoryblock(MemoryBlock_t** memoryblock, XepDispatch_t* dispatch, uint32_t length)
{
    uint32_t status = memorypoolset_take_auto(memoryblock, dispatch->memorypoolset, length);
    return status;
}

uint32_t dispatch_release_message(XepDispatch_t* dispatch, XepDispatchMessage_t* message)
{
    if (message->memoryblock)
	{
        memorypool_free(message->memoryblock);
	}
    message->memoryblock = NULL;
	return XEP_ERROR_OK;
}

uint32_t dispatch_send_message(uint32_t* message_id, XepDispatch_t* dispatch, XepDispatchMessageTags_t message_tag, MemoryBlock_t *memoryblock, uint32_t length)
{
	
	XepDispatchMessage_t message;
	message.id = dispatch->message_id_counter++;
	message.tag = message_tag;
	message.ref = 0;
    message.memoryblock = memoryblock;
	message.length = length;
	message.sender = (void*)xTaskGetCurrentTaskHandle();

	TickType_t timeout = 10 / portTICK_PERIOD_MS;

	QueueHandle_t queue = dispatch->subscribers[message_tag].queue;
	if ( queue == NULL)
	{
		dispatch_release_message(dispatch, &message);
		return XEP_ERROR_DISPATCH_UNKNOWN;
	}
	
	if (message_id != NULL)
	{
		*message_id = message.id;
	}

	void* lock = dispatch->subscribers[message_tag].lock;
	if (xtlock_lock(lock, timeout) != XTLOCK_OK)
	{
		dispatch_release_message(dispatch, &message);
		return XEP_ERROR_DISPATCH_UNKNOWN;
	}
	
	BaseType_t ret = xQueueSend(queue, &message, timeout);
	xtlock_unlock(lock);
	if (ret == errQUEUE_FULL)
	{
		dispatch_release_message(dispatch, &message);
		return XEP_ERROR_DISPATCH_FULL;
	}
	else if (ret != pdTRUE)
	{
		dispatch_release_message(dispatch, &message);
		return XEP_ERROR_TIMEOUT;
	}
	
	// Data sent. Now notify task.
	xTaskNotify(dispatch->subscribers[message_tag].task, dispatch->notify_value, eSetBits);
	
	return XEP_ERROR_OK;
}

uint32_t dispatch_forward_message(XepDispatch_t* dispatch, XepDispatchMessageTags_t message_tag, XepDispatchMessage_t* message)
{
/*
    XepDispatchMessage_t message;
    message.id = dispatch->message_id_counter++;
    message.tag = message_tag;
    message.ref = 0;
    message.memoryblock = memoryblock;
    message.length = length;
    message.sender = (void*)xTaskGetCurrentTaskHandle();
*/
	message->tag = message_tag; // New tag.

    QueueHandle_t queue = dispatch->subscribers[message_tag].queue;
    if ( queue == NULL)
    {
        dispatch_release_message(dispatch, message);
        return XEP_ERROR_DISPATCH_UNKNOWN;
    }

    TickType_t timeout = 500 / portTICK_PERIOD_MS;
    BaseType_t ret = xQueueSend(queue, message, timeout);
    if (ret == errQUEUE_FULL)
    {
		dispatch_release_message(dispatch, message);
        return XEP_ERROR_DISPATCH_FULL;
    }
    else if (ret != pdTRUE)
    {
		dispatch_release_message(dispatch, message);
        return XEP_ERROR_TIMEOUT;
    }

    // Data sent. Now notify task.
    xTaskNotify(dispatch->subscribers[message_tag].task, dispatch->notify_value, eSetBits);

    return XEP_ERROR_OK;
}

uint32_t dispatch_reply_message(uint32_t* message_id, XepDispatch_t* dispatch, XepDispatchMessage_t* received_message, XepDispatchMessageTags_t message_tag, MemoryBlock_t *memoryblock, uint32_t length)
{
	
	XepDispatchMessage_t message;
	message.id = dispatch->message_id_counter++;
	message.tag = message_tag;
	message.ref = received_message->id;
    message.memoryblock = memoryblock;
	message.length = length;
	message.sender = (void*)xTaskGetCurrentTaskHandle();

	QueueHandle_t queue = NULL; 
	task_to_queue(&queue, dispatch, received_message->sender);
	if ( queue == NULL)
	{
		dispatch_release_message(dispatch, &message);
		return XEP_ERROR_DISPATCH_UNKNOWN;
	}
	
	if (message_id != NULL)
	{
		*message_id = message.id;
	}
	
	TickType_t timeout = 500 / portTICK_PERIOD_MS;
	BaseType_t ret = xQueueSend(queue, &message, timeout);
	if (ret == errQUEUE_FULL)
	{
		dispatch_release_message(dispatch, &message);
		return XEP_ERROR_DISPATCH_FULL;
	}
	else if (ret != pdTRUE)
	{
		dispatch_release_message(dispatch, &message);
		return XEP_ERROR_TIMEOUT;
	}
	
	// Data sent. Now notify task.
	xTaskNotify(received_message->sender, dispatch->notify_value, eSetBits);
	
	return XEP_ERROR_OK;
}
