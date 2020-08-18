/**
 * @file
 *
 *
 */

#include "xttoolbox.h"
#include <FreeRTOS.h>
#include "task.h"
#include "xep_hal.h"

uint64_t xttb_tick_os(void)
{
    uint64_t ostick = xTaskGetTickCount();

    return ostick;
}

uint64_t xttb_systimer_us(void)
{
    return xt_get_system_timer_us();
}

uint8_t* xttb_append_little_endian_uint32(uint8_t* buffer, uint32_t value)
{
    buffer[0] = value&0xFF;
    buffer[1] = (value>>8)&0xFF;
    buffer[2] = (value>>16)&0xFF;
    buffer[3] = (value>>24)&0xFF;

    return &buffer[4];
}

uint8_t* xttb_append_little_endian_uint16(uint8_t* buffer, uint16_t value)
{
    buffer[0] = value&0xFF;
    buffer[1] = (value>>8)&0xFF;

    return &buffer[2];
}

uint8_t* xttb_append_little_endian_uint8(uint8_t* buffer, uint8_t value)
{
    buffer[0] = value;

    return &buffer[1];
}