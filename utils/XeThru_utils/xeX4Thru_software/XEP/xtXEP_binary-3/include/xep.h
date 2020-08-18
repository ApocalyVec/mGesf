/**
 * @file
 * @brief XEP interface.
 *
 *
 */

#ifndef XEP_H
#define XEP_H

#include <stdint.h>
#include "xep_dispatch.h"
#include "x4driver.h"
#include "protocol_parser.h"


typedef struct
{
	XepDispatch_t* dispatch;
	X4Driver_t* x4driver;
    McpParser_t* mcp_parser;
    void* user_reference;
    void* internal;
    uint32_t route;
} XepHandle_t;


typedef enum
{
    XDC_NORMAL,
    XDC_APPLICATION_TASK_HIGH_PRIORITY,
} XepDebugControl_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Process dispatch messages
 *
 * @return Status of execution
 */
int xep_process_dispatch_message_default(XepHandle_t* xep, XepDispatchMessage_t* dispatch_message);

/**
 * @brief Control special XEP debug features
 *
 * @return Status of execution
 */
int xep_debug_control(XepHandle_t* xep, uint32_t debug_control);

/**
 * @brief Append XEP components version list to destination.
 *
 * @return Number of chars added
 */
int xep_get_systeminfo_versionlist(char* destination, uint32_t max_length);

#ifdef __cplusplus
}
#endif


#endif // XEP_H
