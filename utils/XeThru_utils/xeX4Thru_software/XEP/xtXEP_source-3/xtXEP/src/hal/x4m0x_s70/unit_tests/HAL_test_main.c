/**
 * @file
 *
 * @brief main() function that can be used to create a test firmware for
 * XEP HAL.
 *
 */

#include <asf.h>
#include "hal_board_init.h"
#include "xt_XEP_HAL.h"

#define TASK_TEST_STACK_SIZE            (150)
#define TASK_TEST_PRIORITY        (tskIDLE_PRIORITY + 3)
static void task_test(void *pvParameters);
xTaskHandle pTaskTest;

int main(void)
{
	int status;

    HAL_board_init();

    status = xtio_set_direction(XTIO_XETHRU_IO1, XTIO_OUTPUT, XTIO_PIN_LEVEL_LOW);
    status = xtio_set_direction(XTIO_XETHRU_IO2, XTIO_OUTPUT, XTIO_PIN_LEVEL_LOW);

    status = xtio_set_level(XTIO_XETHRU_IO1, XTIO_PIN_LEVEL_HIGH);
    status = xtio_set_level(XTIO_XETHRU_IO1, XTIO_PIN_LEVEL_LOW);
    status = xtio_set_level(XTIO_XETHRU_IO2, XTIO_PIN_LEVEL_HIGH);
    status = xtio_set_level(XTIO_XETHRU_IO2, XTIO_PIN_LEVEL_LOW);

    status = xtio_set_direction(XTIO_XETHRU_IO1, XTIO_INPUT, XTIO_PIN_LEVEL_LOW);

    int level = 0;
    status = xtio_get_level(XTIO_XETHRU_IO1, &level);
    status = xtio_get_level(XTIO_XETHRU_IO1, &level);
    status = xtio_get_level(XTIO_XETHRU_IO1, &level);
    status = xtio_get_level(XTIO_XETHRU_IO1, &level);

    status = xtio_led_set_state(XTIO_LED_RED, 1);
    status = xtio_led_set_state(XTIO_LED_GREEN, 1);
    status = xtio_led_set_state(XTIO_LED_BLUE, 1);


	xTaskCreate(task_test, (const signed char *const) "tskTest", TASK_TEST_STACK_SIZE, NULL, TASK_TEST_PRIORITY, &pTaskTest);

    /* Start the RTOS scheduler. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for (;;) {
	}

	return 0;
}


static void task_test(void *pvParameters)
{
    int led_status = 0;
	int status;

	for (;;)
	{
		vTaskDelay(500 / portTICK_RATE_MS);
        status = xtio_led_set_state(XTIO_LED_RED, led_status);
        if (0 == led_status)
        {
            led_status = 1;
        }
        else
        {
            led_status = 0;
        }

	}
}
