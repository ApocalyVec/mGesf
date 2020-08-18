/**
 * @file
 *
 *
 */

#ifndef TASK_HOSTCOM_H
#define TASK_HOSTCOM_H

#include "xep_dispatch.h"
#include "memorypool.h"
#include "FreeRTOS.h"
#include "semphr.h"

typedef struct
{
	void* hostcom_task_handle;
	XepDispatch_t* dispatch;
	MemoryBlock_t* message_noescape_memoryblock;
	uint8_t* messagebuild_buffer;
	uint32_t messagebuild_index;
	uint32_t messagebuild_length;
	SemaphoreHandle_t messagebuild_send_semaphore;
	uint32_t message_tag;
} XepHostComMCPUserReference_t;


#ifdef __cplusplus
extern "C" {
#endif

uint32_t task_hostcom_init(XepDispatch_t* dispatch);

#ifdef __cplusplus
}
#endif

#endif // TASK_HOSTCOM_H
