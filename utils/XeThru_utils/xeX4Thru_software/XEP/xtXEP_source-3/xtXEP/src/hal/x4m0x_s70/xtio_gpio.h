/**
 * @file
 *
 * @brief Local header file for xtio functions.
 */

#ifndef XTIO_GPIO_H
#define XTIO_GPIO_H

#include "xep_hal.h"
#include "pio.h"

void xtio_update_pin(Pin* pin, uint8_t pin_index);
int xtio_irq_init(void);

#endif //  XTIO_GPIO_H
