#ifndef XTHOUSEKEEPING_H
#define XTHOUSEKEEPING_H

#include "xep_hal.h"
#include "xep_dispatch.h"

int xthousekeeping_init(XepDispatch_t* dispatch, char* app_info);

int xthousekeeping_trigger_crashdump(xt_swreset_reason_t crash_reason, const char* info, bool full_dump);

/**
 * @brief Function to check if safe mode is entered. Other tasks should take care to limit
 * activity to critical functionality only.
 *
 * @return True for safe mode, false otherwise. 
 */
bool xthousekeeping_in_safe_mode(void);

#endif