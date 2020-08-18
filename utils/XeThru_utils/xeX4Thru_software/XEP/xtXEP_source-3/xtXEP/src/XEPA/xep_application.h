/**
 * @file
 *
 * @brief XEP Module application interface
 *
 * Primary setup of module implementation. Creates bridge to application layer.
 */

#ifndef XEP_APPLICATION_H
#define XEP_APPLICATION_H

#include "xep_dispatch.h"
#include "x4driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize module application instance.
 *
 * Create dispatch for current application.
 *
 * @return Status of execution
 */
uint32_t xep_init(XepDispatch_t** dispatch);

/**
 * @brief Initialize Application task.
 *
 * @return Status of execution
 */
uint32_t task_application_init(XepDispatch_t* dispatch, X4Driver_t* x4driver);

#ifdef __cplusplus
}
#endif

#endif // XEP_APPLICATION_H
