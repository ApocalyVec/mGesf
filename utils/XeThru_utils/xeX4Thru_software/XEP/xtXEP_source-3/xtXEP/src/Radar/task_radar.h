/**
 * @file
 *
 * @brief Radar event task
 *
 * Serves radar events, such as data ready, reads data and sends it to subscriber.
 */

#ifndef TASK_RADAR_H
#define TASK_RADAR_H

#include "xep_dispatch.h"
#include "x4driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Radar task.
 *
 * @return Status of execution
 */
uint32_t task_radar_init(X4Driver_t** x4driver, XepDispatch_t* dispatch);

/**
 * @brief Enter TX only mode
 *
 * @return None
 */
void radar_certification_enter_tx_only(X4Driver_t* x4driver);

/**
 * @brief Enter TX only mode, with TX off. This means the digital logic is active, but no power to transmitter. 
 *
 * @return None
 */
void radar_certification_enter_tx_only_tx_off(X4Driver_t* x4driver);

#ifdef __cplusplus
}
#endif

#endif // TASK_RADAR_H
