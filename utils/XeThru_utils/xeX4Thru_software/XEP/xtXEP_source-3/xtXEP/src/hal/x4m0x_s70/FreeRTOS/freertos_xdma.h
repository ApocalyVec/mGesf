#ifndef FREERTOS_XDMA_H_INCLUDED
#define FREERTOS_XDMA_H_INCLUDED

#include <stdint.h>


void freertos_xdma_init(void);
void freertos_xdma_register(uint8_t ch, void (*fun)(void));
void freertos_xdma_free(uint8_t ch);
#endif /*FREERTOS_H_INCLUDED*/