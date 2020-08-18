/**
 * @file
 *
 * 
 */

#include "module_ui.h"
#include "xep_hal.h"

static bool module_ui_locked = false;
static uint32_t _module_ui_led_set_color(uint32_t red, uint32_t green, uint32_t blue);

uint32_t module_ui_led_set_color(uint32_t red, uint32_t green, uint32_t blue)
{
    if (module_ui_locked)
        return 0;
    
    _module_ui_led_set_color(red, green, blue);

    return 0;
}

uint32_t _module_ui_led_set_color(uint32_t red, uint32_t green, uint32_t blue)
{
    if (red > 0)
        xtio_led_set_state(XTIO_LED_RED, XTIO_LED_ON);
    else
        xtio_led_set_state(XTIO_LED_RED, XTIO_LED_OFF);

    if (green > 0)
        xtio_led_set_state(XTIO_LED_GREEN, XTIO_LED_ON);
    else
        xtio_led_set_state(XTIO_LED_GREEN, XTIO_LED_OFF);

    if (blue > 0)
        xtio_led_set_state(XTIO_LED_BLUE, XTIO_LED_ON);
    else
        xtio_led_set_state(XTIO_LED_BLUE, XTIO_LED_OFF);
    
    return 0;
}

uint32_t module_ui_led_override_color(uint32_t red, uint32_t green, uint32_t blue, bool locked)
{
    module_ui_locked = locked;
    _module_ui_led_set_color(red, green, blue);
}