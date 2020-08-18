/**
 * @file
 * @brief Misc tools.
 *
 */

#ifndef XTTOOLBOX_H
#define XTTOOLBOX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get system os time tick.
 *
 * @return Tick value.
 */
uint64_t xttb_tick_os(void);

/**
 * @brief Get accurate timer in microseconds.
 *
 * @return Timer value.
 */
uint64_t xttb_systimer_us(void);

/**
 * @brief Appends an uint32_t value to a uint8_t buffer without requiring buffer alignment.
 *
 * @return Pointer to buffer offset by appended data size.
 */
uint8_t* xttb_append_little_endian_uint32(uint8_t* buffer, uint32_t value);

/**
 * @brief Appends an uint16_t value to a uint8_t buffer without requiring buffer alignment.
 *
 * @return Pointer to buffer offset by appended data size.
 */
uint8_t* xttb_append_little_endian_uint16(uint8_t* buffer, uint16_t value);

/**
 * @brief Appends an uint8_t value to a uint8_t buffer without requiring buffer alignment.
 *
 * @return Pointer to buffer offset by appended data size.
 */
uint8_t* xttb_append_little_endian_uint8(uint8_t* buffer, uint8_t value);


#ifdef __cplusplus
}
#endif

#endif // XTTOOLBOX_H
