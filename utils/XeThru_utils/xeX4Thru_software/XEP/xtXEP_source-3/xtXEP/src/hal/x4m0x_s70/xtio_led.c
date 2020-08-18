/**
 * @file
 *
 * @brief Implementation of GPIO functions for X4M0x boards with Atmel SAM S70 MCU.
 *
 * See @ref X4M0x_SAMS70/xtio_gpio.h and @ref xt_XEP_HAL.h for more documentation.
 *
 * @todo Implement PWM mode for LEDs
 */


#include "xtio_led.h"
#include "board.h"
#include "pio.h"
#include "xtio_gpio.h"
#include "xt_config.h"

Pin XPIN_LED_RED = {PIO_PD24, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT};
Pin XPIN_LED_GREEN = {PIO_PD25, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT};
Pin XPIN_LED_BLUE = {PIO_PD26, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT};

#define RED_AVAILABLE() (XPIN_LED_RED.mask != 0)
#define GREEN_AVAILABLE() (XPIN_LED_GREEN.mask != 0)
#define BLUE_AVAILABLE() (XPIN_LED_BLUE.mask != 0)
#define SINGLE_LED() ((RED_AVAILABLE() + GREEN_AVAILABLE() + BLUE_AVAILABLE()) == 1)

float red_state;
float green_state;
float blue_state;

int xtio_decode_led_id(xtio_led_id_t led_id, Pin ** pin);
int xtio_decode_led_id(xtio_led_id_t led_id, Pin ** pin)
{
    // Check hardware configuration
    static bool pins_configured = false;
    if (!pins_configured)
    {
        xtio_update_pin(&XPIN_LED_RED, XT_CONFIG_LED_RED());
        xtio_update_pin(&XPIN_LED_GREEN, XT_CONFIG_LED_GREEN());
        xtio_update_pin(&XPIN_LED_BLUE, XT_CONFIG_LED_BLUE());

        pins_configured = true;
    }

    switch (led_id)
    {
        case XTIO_LED_RED:
            *pin = &XPIN_LED_RED;
            break;
        case XTIO_LED_GREEN:
            *pin = &XPIN_LED_GREEN;
            break;
        case XTIO_LED_BLUE:
            *pin = &XPIN_LED_BLUE;
            break;
        default:
            return XTIO_WRONG_LED_ID;
    }
    return XT_SUCCESS;
}

int xtio_led_init(
    xtio_led_id_t led_id,
    int mode,
    float led_state
)
{
    Pin * pin = NULL;
    int status;

    status = xtio_decode_led_id(led_id, &pin);

    if (0.5 <= led_state)
	{
        pin->type = PIO_OUTPUT_0;
	} else
	{
        pin->type = PIO_OUTPUT_1;
	}
    if (pin->mask != 0)
	    PIO_Configure(pin, 1);

    return status;
}

int xtio_led_set_state(
    xtio_pin_id_t led_id,
    float led_state
)
{
    Pin * pin = NULL;
    int status;

    status = xtio_decode_led_id(led_id, &pin);

    if (SINGLE_LED())
    {
        switch (led_id)
        {
            case XTIO_LED_RED:
                red_state = led_state;
                break;
            case XTIO_LED_GREEN:
                green_state = led_state;
                break;
            case XTIO_LED_BLUE:
                blue_state = led_state;
                break;
            default:
                break;
        }
        
        if (RED_AVAILABLE())
            xtio_decode_led_id(XTIO_LED_RED, &pin);
        else if (GREEN_AVAILABLE())
            xtio_decode_led_id(XTIO_LED_GREEN, &pin);
        else 
            xtio_decode_led_id(XTIO_LED_BLUE, &pin);
        
        if ((red_state > 0.5) || (green_state > 0.5) || (blue_state > 0.5))
            PIO_Clear(pin);
        else
            PIO_Set(pin);
    } else
    {
        if (0.5 <= led_state)
            PIO_Clear(pin);
        else
            PIO_Set(pin);
    }

    return status;
}

int xtio_led_toggle_state(
    xtio_pin_id_t led_id
)
{
    /** @todo Support LEDs in PWM mode */

    int pin_level;
    Pin * pin = NULL;
    int status;

    status = xtio_decode_led_id(led_id, &pin);

    pin_level = PIO_Get(pin);
	if (0 == pin_level)
        PIO_Set(pin);
    else
        PIO_Clear(pin);

    return status;
}
