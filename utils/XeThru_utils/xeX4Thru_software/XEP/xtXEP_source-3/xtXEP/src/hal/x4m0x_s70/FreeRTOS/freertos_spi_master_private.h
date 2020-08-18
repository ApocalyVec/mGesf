/**
 * @file
 *
 * @brief Local header file for xtio functions.
 */

#ifndef FRERTOS_SPI_MASTER_PRIVATE_H
#define  FRERTOS_SPI_MASTER_PRIVATE_H

#include "xep_hal.h"
#include "qspi.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @todo Functions in this file should probably not be used.
 *       Most of these function are local to qspi.c and should
 *       probably stay so. Use config/init functions instead.
 */



static inline uint32_t qspi_is_rx_full(Qspi *qspi)
{
	if (qspi->QSPI_SR & QSPI_SR_RDRF) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * \brief Ends ongoing transfer by releasing CS of QSPI peripheral.
 *
 * \param qspi  Pointer to an Qspi instance.
 */
static inline void qspi_end_transfer(Qspi *qspi)
{
	assert(qspi);
	while(!(qspi->QSPI_SR & QSPI_SR_TXEMPTY));
	qspi->QSPI_CR = QSPI_CR_LASTXFER;
}

/**
 * \brief Set QSPI to Memory mode.
 *
 * \param qspi Pointer to an QSPI instance.
 */
static inline void qspi_set_memory_mode(Qspi *qspi)
{
	qspi->QSPI_MR |= QSPI_MR_SMM_MEMORY;
}

/**
 * \brief Set QSPI to SPI mode (Master mode only).
 *
 * \param qspi Pointer to an QSPI instance.
 */
static inline void qspi_set_spi_mode(Qspi *qspi)
{
	qspi->QSPI_MR &= (~QSPI_MR_SMM_SPI);
}

/**
 * \brief Enable waiting RX_EMPTY before transfer starts.
 *
 * \param qspi Pointer to an QSPI instance.
 */
static inline void qspi_enable_wait_data_read_before_transfer(Qspi *qspi)
{
	qspi->QSPI_MR |= QSPI_MR_WDRBT;
}

/**
 * \brief Disable waiting RX_EMPTY before transfer starts.
 *
 * \param qspi Pointer to an QSPI instance.
 */
static inline void qspi_disable_wait_data_read_before_transfer(Qspi *qspi)
{
	qspi->QSPI_MR &= (~QSPI_MR_WDRBT);
}

/**
 * \brief Set Chip Select Mode.
 *
 * \param qspi    Pointer to an QSPI instance.
 * \param csmode  Chip select mode to be set.
 */
static inline void qspi_set_chip_select_mode(Qspi *qspi, uint32_t csmode)
{
	uint32_t mask = qspi->QSPI_MR & (~QSPI_MR_CSMODE_Msk);
	qspi->QSPI_MR = mask | QSPI_MR_CSMODE(csmode);
}

/**
 * \brief Set the number of data bits transferred.
 *
 * \param qspi Pointer to an QSPI instance.
 * \param bits Bits per transfer.
 */
static inline void qspi_set_bits_per_transfer(Qspi *qspi, uint32_t bits)
{
	uint32_t mask = qspi->QSPI_MR & (~QSPI_MR_NBBITS_Msk);
	qspi->QSPI_MR = mask | bits;
}

/**
 * \brief Set qspi minimum inactive QCS delay.
 *
 * \param qspi      Pointer to a Qspi instance.
 * \param uc_dlybct Time to be delay.
 */
static inline void qspi_set_minimum_inactive_qcs_delay(Qspi *qspi, uint8_t uc_dlybct)
{
	assert(qspi);
	uint32_t mask = qspi->QSPI_MR & (~QSPI_MR_DLYBCT_Msk);
	qspi->QSPI_MR = mask | QSPI_MR_DLYBCT(uc_dlybct);
}

/**
 * \brief Set qspi delay between consecutive transfers.
 *
 * \param qspi     Pointer to a Qspi instance.
 * \param uc_dlycs Time to be delay.
 */
static inline void qspi_set_delay_between_consecutive_transfers(Qspi *qspi, uint32_t uc_dlycs)
{
	assert(qspi);
	uint32_t mask = qspi->QSPI_MR & (~QSPI_MR_DLYCS_Msk);
	qspi->QSPI_MR = mask | QSPI_MR_DLYCS(uc_dlycs);
}

/**
 * \brief Set qspi clock transfer delay.
 *
 * \param qspi     Pointer to a Qspi instance.
 * \param uc_dlybs Delay before QSCK.
 */
static inline void qspi_set_transfer_delay(Qspi *qspi, uint8_t uc_dlybs)
{
	assert(qspi);
	uint32_t mask = qspi->QSPI_SCR & (~QSPI_SCR_DLYBS_Msk);
	qspi->QSPI_SCR = mask | QSPI_SCR_DLYBS(uc_dlybs);
}

/**
 * \brief Read QSPI RDR register for SPI mode
 *
 * \param qspi   Pointer to an Qspi instance.
 * \return status Data value read from QSPI.
 */
static inline uint16_t qspi_read_spi(Qspi *qspi)
{
	assert(qspi);
	while(!(qspi->QSPI_SR & QSPI_SR_RDRF));
	return  qspi->QSPI_RDR;
}

/**
 * \brief Write to QSPI Tx register in SPI mode
 *
 * \param qspi     Pointer to an Qspi instance.
 * \param w_data   Data to transmit
 */
static inline void qspi_write_spi(Qspi *qspi, uint16_t w_data)
{
	assert(qspi);
	/** Send data */
	while(!(qspi->QSPI_SR & QSPI_SR_TXEMPTY));
	qspi->QSPI_TDR = w_data ;
	while(!(qspi->QSPI_SR & QSPI_SR_TDRE));
}


#ifdef __cplusplus
}
#endif

#endif //  FRERTOS_SPI_MASTER_PRIVATE_H
