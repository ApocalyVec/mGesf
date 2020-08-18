/**
 * @file
 *
 * @brief LED control and initialization interface
 *
 */
#ifndef LED_CTRL_H_
#define LED_CTRL_H_

/**
 * LED status mode
 */
typedef enum { OFF, IN_BOOT, PROG, ERROR, DONE, CERT } led_status_t;

/**
 * @brief Initializes LED HW
 *
 */
void led_ctrl_init(void);
/**
 * @brief Controls LED
 */
void led_ctrl(
    led_status_t status            ///< status LED status to display
);

#endif