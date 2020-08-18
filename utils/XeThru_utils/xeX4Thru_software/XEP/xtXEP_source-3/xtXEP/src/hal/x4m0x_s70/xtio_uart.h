/**
 * @file
 *
 * @brief Local header file for xtio UART functions.
 */

#ifndef XTIO_UART_H
#define  XTIO_UART_H

#include "xep_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

typedef enum { UART_MODE, USRT_MODE, SPI_MODE, UART_RX_MODE, UNCHANGED_MODE } comm_mode_t;

int xtio_uart_init(comm_mode_t mode);
int xtio_uart_configure(comm_mode_t mode);
int xtio_uart_send(uint8_t * buffer, uint32_t length, int time_out_definition);
int xtio_uart_receive(uint8_t * buffer, uint32_t * length);

#endif //  XTIO_UART_H
