/**
 * @file
 *
 * @brief USB CDC interface functions
 *
 */
#ifndef CDC_H_INCLUDED
#define CDC_H_INCLUDED


/**
 * @brief Callback called by CDC driver to indicate that interface is enabled
 * @return true
 */
bool cdc_port_enabled(void);
/**
 * @brief Callback called by CDC driver to indicate that interface is disabled
 * @return true
 */
bool cdc_port_disabled(void);
/**
 * @brief Initializes cdc interface 
 *
 */
void cdc_init(void);
/**
 * @brief Stops cdc interface 
 *
 */
void cdc_deinit(void);
/**
 * @brief Receives byte from cdc interface and feeds the RX fifo
 *
 */
void cdc_data_ready(void);
/**
 * @brief Feeds cdc interface with new data from TX fifo
 *
 */
void cdc_tx_empty(void);
/**
 * @brief Flushes cdc TX buffer
 *
 */
void cdc_flush(void);
#endif // CDC_H_INCLUDED
