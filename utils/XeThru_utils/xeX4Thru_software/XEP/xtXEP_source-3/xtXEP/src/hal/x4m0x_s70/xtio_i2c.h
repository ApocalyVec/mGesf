/**
 * @file
 *
 * @brief Local header file for xtio I2C functions.
 */

#ifndef XTIO_I2C_H
#define XTIO_I2C_H

#include <stdbool.h>

int xtio_i2c_init(void);
bool xtio_i2c_send_completed(void);
int xtio_i2c_send(uint8_t * buffer, uint32_t length, int time_out_definition);
int xtio_i2c_receive(uint8_t * buffer, uint32_t * length);

#endif //  XTIO_I2C_H