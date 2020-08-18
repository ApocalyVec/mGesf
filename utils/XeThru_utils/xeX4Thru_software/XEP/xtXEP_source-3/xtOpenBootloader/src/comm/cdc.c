/**
 * @file
 *
 * @brief USB CDC interface functions
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <asf.h>

#include <comm/cdc.h>
#include <comm/comm_iface.h>

static bool isPortEnabled=false;

/**
 * @brief Callback called by CDC driver to indicate that interface is enabled
 * @return true
 */
bool cdc_port_enabled(void){
	isPortEnabled=true;
	return true;
}
/**
 * @brief Callback called by CDC driver to indicate that interface is disabled
 * @return true
 */
 bool cdc_port_disabled(void){
	isPortEnabled=false;
	return true;
}
/**
 * @brief Flushes cdc TX buffer
 *
 */
void cdc_flush(void)
{
	while((isPortEnabled) && (!CommIface_is_empty(IFACE_OUT))) {
		udi_cdc_putc(CommIface_get(IFACE_OUT));	
	}
}

/**
 * @brief Initializes cdc interface 
 *
 */
void cdc_init(void)
{
	udc_start();
}

/**
 * @brief Stops cdc interface 
 *
 */
void cdc_deinit(void)
{
	udc_detach();
	udc_stop();
}

/**
 * @brief Receives byte from cdc interface and feeds the RX fifo
 *
 */
void cdc_data_ready(void)
{
	while (udi_cdc_is_rx_ready()) {
		CommIface_put(IFACE_IN, (uint8_t)udi_cdc_getc());
	}
}

/**
 * @brief Feeds cdc interface with new data from TX fifo
 *
 */
void cdc_tx_empty(void)
{
	if (!CommIface_is_empty(IFACE_OUT)) {
		if (!udi_cdc_is_tx_ready()) {
			// Fifo full
			udi_cdc_signal_overrun();
		} else {
			udi_cdc_putc(CommIface_get(IFACE_OUT));
		}	
	}
}
