/**
 * @file
 *
 *
 */


#ifndef XEP_DISPATCH_H
#define XEP_DISPATCH_H

#include <stdint.h>
#include "memorypoolset.h"
//#include "xep_dispatch_messages.h"

#define XEP_ERROR_OK					0
#define XEP_ERROR_OTHER					1
#define XEP_ERROR_TIMEOUT				2
#define XEP_ERROR_NOT_FOUND				3
#define XEP_ERROR_DISPATCH_UNKNOWN		10
#define XEP_ERROR_DISPATCH_FULL			11

typedef enum
{
	XEP_DISPATCH_MESSAGETAG_DEBUG_AND_TRACE = 0,
	XEP_DISPATCH_MESSAGETAG_REPLY,
	XEP_DISPATCH_MESSAGETAG_HOSTCOM_RX,
	XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND,
	XEP_DISPATCH_MESSAGETAG_RADAR_DATA,
    XEP_DISPATCH_MESSAGETAG_HOSTCOM_RX_DEBUG,
	XEP_DISPATCH_MESSAGETAG_HOSTCOM_SEND_DEBUG,
	XEP_DISPATCH_MESSAGETAG_COUNT,
} XepDispatchMessageTags_t;

typedef struct
{
	uint32_t tag;			// Message group tag
	uint32_t id;			// Unique message autonumber
	uint32_t ref;			// Reference to id when reply message
    MemoryBlock_t* memoryblock; // Memory for message payload
	uint32_t length;		// Message payload length
	void* sender;			// Reference to sender
} XepDispatchMessage_t;

typedef struct
{
	void* task;
	void* queue;
	void* lock;
} XepDispatchSubscriberInfo_t;

typedef struct
{
	uint32_t notify_value;
	uint32_t message_id_counter;
    MemoryPoolSet_t* memorypoolset;
	XepDispatchSubscriberInfo_t subscribers[XEP_DISPATCH_MESSAGETAG_COUNT];

} XepDispatch_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get size of dispatch struct.
 *
 * @return Status of execution
 */
uint32_t dispatch_get_instance_size(void);

/**
 * @brief Create dispatch.
 *
 * @return Status of execution
 */
uint32_t dispatch_create(XepDispatch_t** dispatch, void* instance_memory);

/**
 * @brief Configure which notify value the dispatcher should use to notify tasks.
 *
 * @return Status of execution
 */
uint32_t dispatch_set_notify_value(XepDispatch_t* dispatch, uint32_t notify_value);

/**
 * @brief Register to dispatcher, returning the queue used for dispatch messages.
 *
 * @return Status of execution
 */
uint32_t dispatch_register(void** queue, void** lock, XepDispatch_t* dispatch, uint32_t queue_length);

/**
 * @brief Subscribe to message tag.
 *
 * @return Status of execution
 */
uint32_t dispatch_subscribe(XepDispatch_t* dispatch, XepDispatchMessageTags_t message_tag, void* queue, void* lock);

/**
 * @brief Request a memoryblock from the dispatch memory poolset.
 *
 * @return Status of execution
 */
uint32_t dispatch_get_message_memoryblock(MemoryBlock_t** memoryblock, XepDispatch_t* dispatch, uint32_t length);

/**
 * @brief Release message. Will also release underlying memoryblock from dispatch memory poolset.
 *
 * @return Status of execution
 */
uint32_t dispatch_release_message(XepDispatch_t* dispatch, XepDispatchMessage_t* message);

/**
 * @brief Send message on dispatcher.
 *
 * @return Status of execution
 */
uint32_t dispatch_send_message(uint32_t* message_id, XepDispatch_t* dispatch, XepDispatchMessageTags_t message_tag, MemoryBlock_t* memoryblock, uint32_t length);

/**
 * @brief Forward message to a different message tag.
 *
 * @return Status of execution
 */
uint32_t dispatch_forward_message(XepDispatch_t* dispatch, XepDispatchMessageTags_t message_tag, XepDispatchMessage_t* message);

/**
 * @brief Reply to message. Will send message to sender even if the sender does not subscribe to such messages.
 *
 * @return Status of execution
 */
uint32_t dispatch_reply_message(uint32_t* message_id, XepDispatch_t* dispatch, XepDispatchMessage_t* received_message, XepDispatchMessageTags_t message_tag, MemoryBlock_t* memoryblock, uint32_t length);

#ifdef __cplusplus
}
#endif


#endif // XEP_DISPATCH_H
