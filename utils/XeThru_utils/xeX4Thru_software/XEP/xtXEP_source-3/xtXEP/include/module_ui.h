/**
 * @file
 *
 *
 */

#ifndef MODULE_UI_H
#define MODULE_UI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the module LED color.
 *
 * @return Status of execution.
 */
uint32_t module_ui_led_set_color(uint32_t red, uint32_t green, uint32_t blue);

/**
 * @brief LED override to force LED behaviour and block further changes.
 * 
 * @return Status of execution.
 */
uint32_t module_ui_led_override_color(uint32_t red, uint32_t green, uint32_t blue, bool locked);

#ifdef __cplusplus
}
#endif

#endif // MODULE_UI_H
