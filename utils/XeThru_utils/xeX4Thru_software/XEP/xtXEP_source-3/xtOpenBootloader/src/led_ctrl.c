/**
 * @file
 *
 * @brief LED control and initialization functions
 *
 */
#include "led_ctrl.h"
#include <asf.h>
#include "board.h"
#include "xt_config/xt_config.h"

uint32_t red_led_pin = PIO_PD24_IDX;         ///< Definition of Red LED's pin
uint32_t green_led_pin = PIO_PD25_IDX;       ///< Definition of Green LED's pin
uint32_t blue_led_pin = PIO_PD26_IDX;        ///< Definition of Blue LED's pin

#define HAS_RED_LED()       (XPIN_LED_RED != 0xFF)                                 ///< Returns true if Red LED is configured
#define HAS_GREEN_LED()     (XPIN_LED_GREEN != 0xFF)                               ///< Returns true if Green LED is configured
#define HAS_BLUE_LED()      (XPIN_LED_BLUE != 0xFF)                                ///< Returns true if Blue LED is configured
#define HAS_RGB_LED()       (HAS_RED_LED() && HAS_GREEN_LED() && HAS_BLUE_LED())   ///< Returns true if RGB LED is configured

uint32_t vbus_pin = PIO_PA2_IDX;             ///< Definition of VBUS pin

/**
 * @brief Initializes LED HW
 *
 */
void led_ctrl_init(void)
{
    red_led_pin = XT_CONFIG_LED_RED();
    green_led_pin = XT_CONFIG_LED_GREEN();
    blue_led_pin = XT_CONFIG_LED_BLUE();
    vbus_pin = XT_CONFIG_VBUS();

    // Configure LED pins
    if (HAS_RED_LED())
    {
        ioport_set_pin_dir(XPIN_LED_RED,     IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(XPIN_LED_RED, !XPIN_LEVEL_LED_ACTIVE);
    }        
    if (HAS_GREEN_LED())
    {
        ioport_set_pin_dir(XPIN_LED_GREEN,   IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(XPIN_LED_GREEN, !XPIN_LEVEL_LED_ACTIVE);
    }    
    if (HAS_BLUE_LED())
    {
        ioport_set_pin_dir(XPIN_LED_BLUE,    IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(XPIN_LED_BLUE, !XPIN_LEVEL_LED_ACTIVE);
    }
}

void led_ctrl_single(led_status_t status, uint32_t led_pin);
void led_ctrl_rgb(led_status_t status);

/**
 * @brief Controls LED
 *
 */
void led_ctrl(led_status_t status)
{
    if (HAS_RGB_LED())
        led_ctrl_rgb(status);
    else if (HAS_RED_LED())
        led_ctrl_single(status, XPIN_LED_RED);
    else if (HAS_GREEN_LED())
        led_ctrl_single(status, XPIN_LED_GREEN);
    else if (HAS_BLUE_LED())
        led_ctrl_single(status, XPIN_LED_BLUE);
}
/**
 * @brief Controls single colour LED
 *
 */
void led_ctrl_single(led_status_t status, uint32_t led_pin)
{
    switch (status)
    {
        case OFF:
        case ERROR:
            XLED_Off(led_pin);
            break;
        case DONE:
        case IN_BOOT:
        case PROG:
        case CERT:
            XLED_On(led_pin);
            break;
        default:
            break;
    }
}
/**
 * @brief Controls RGB LED
 *
 */
void led_ctrl_rgb(led_status_t status)
{
    switch (status)
    {
        case OFF:
            XLED_Off(XPIN_LED_RED);
            XLED_Off(XPIN_LED_GREEN);
            XLED_Off(XPIN_LED_BLUE);
            break;
        case ERROR:
            XLED_On(XPIN_LED_RED);
            XLED_Off(XPIN_LED_GREEN);
            XLED_Off(XPIN_LED_BLUE);
            break;
        case DONE:
            XLED_On(XPIN_LED_RED);
            XLED_On(XPIN_LED_GREEN);
            XLED_On(XPIN_LED_BLUE);
            break;
        case IN_BOOT:
            XLED_Off(XPIN_LED_RED);
            XLED_On(XPIN_LED_GREEN);
            XLED_On(XPIN_LED_BLUE);
            break;
        case PROG:
            XLED_On(XPIN_LED_RED);
            XLED_Off(XPIN_LED_GREEN);
            XLED_On(XPIN_LED_BLUE);
            break;
        case CERT:
            XLED_Off(XPIN_LED_RED);
            XLED_Off(XPIN_LED_GREEN);
            XLED_On(XPIN_LED_BLUE);
            break;
        default:
            break;
    }
}