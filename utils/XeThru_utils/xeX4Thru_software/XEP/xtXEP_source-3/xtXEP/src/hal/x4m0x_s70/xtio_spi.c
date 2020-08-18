#include "board.h"
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "xep_hal.h"
#include "xtio_spi.h"

#define SPI_CLOCK_POLARITY          0			// Clock polarity.
#define SPI_CLOCK_PHASE             0			// Clock phase.
#define SPI_DELAY_BEFORE            0x10		// Delay before SPCK. (0x40)
#define SPI_DELAY_BETWEEN           0x00		// Delay between consecutive transfers. (0x10)
#define SPI_BAUD_RATE               50000000	// SPI clock speed.

static QspiInstFrame_t *qspi_frame;

static QspiDma_t qspiDma;
extern sXdmad dmad;

// PINS_QSPI is defined in BOARD_X4M0x_SAMS70
static const Pin cs_pin = {PIO_PA11A_QCS, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_PULLUP | PIO_OPENDRAIN};
static const Pin qspi_pins[] = PINS_QSPI;

#define QSPI_CS_ASSERT()	PIO_Clear(&cs_pin)
#define QSPI_CS_DEASSERT()	PIO_Set(&cs_pin)

// Prototypes of local functions
int xtio_set_spi_mode(void * spi_handle, xtio_spi_mode_t xtio_spi_mode);
int xtio_get_spi_mode(void * spi_handle, xtio_spi_mode_t * xtio_spi_mode);
int xtio_spi_write(void * spi_handle, uint8_t * write_buffer, uint32_t number_of_bytes_to_write);
int xtio_spi_read(void * spi_handle, uint8_t * read_buffer, uint32_t number_of_bytes_to_read);
int xtio_spi_write_read(void * spi_handle, uint8_t * write_buffer, uint32_t number_of_bytes_to_write, uint8_t * read_buffer, uint32_t number_of_bytes_to_read);

static void xtio_spi_cb(uint32_t channel, TaskHandle_t pArg)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR(pArg, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

int xtio_spi_init(
    xtio_spi_handle_t ** spi_handle,     ///<
    void* instance_memory,
    xtio_spi_callbacks_t * spi_callbacks,
    void * user_reference,
    xtio_spi_mode_t default_xtio_spi_mode
)
{
	int status; 
	
	xtio_spi_handle_t * spi_handle_local = (xtio_spi_handle_t *)instance_memory;
	memset(spi_handle_local, 0, sizeof(xtio_spi_handle_t));
	instance_memory = (void*) (((uintptr_t)instance_memory) + sizeof(xtio_spi_handle_t));

    spi_handle_local->user_reference = user_reference;
    if (spi_callbacks == NULL)
    {
        spi_handle_local->set_spi_mode = xtio_set_spi_mode;
        spi_handle_local->get_spi_mode = xtio_get_spi_mode;
        spi_handle_local->spi_write = xtio_spi_write;
        spi_handle_local->spi_read = xtio_spi_read;
        spi_handle_local->spi_write_read = xtio_spi_write_read;
    }

    PIO_Configure(qspi_pins, PIO_LISTSIZE(qspi_pins));
	PIO_Configure(&cs_pin, 1);
	
	ENABLE_PERIPHERAL(ID_QSPI);

	status = QSPID_Configure(&qspiDma, 
							QspiMemMode,
							QSPI_MR_CSMODE_LASTXFER| QSPI_MR_DLYCS (SPI_DELAY_BETWEEN), 
							&dmad);
	qspiDma.Qspid.qspiMode = (QspiMode_t)QSPI_MR_SMM_MEMORY;

	qspi_frame = (QspiInstFrame_t *)instance_memory;
	memset(qspi_frame, 0, sizeof(QspiInstFrame_t));
	qspi_frame->InstFrame.bm.bwidth = QSPI_IFR_WIDTH_SINGLE_BIT_SPI;
	
	instance_memory = (void*)(((uintptr_t)instance_memory) + sizeof(QspiInstFrame_t));

	QSPI_ConfigureClock(QSPI, ClockMode_00, QSPI_SCR_SCBR((BOARD_MCK / SPI_BAUD_RATE) - 1)|QSPI_SCR_DLYBS(SPI_DELAY_BEFORE)); // CPOL = 0, PHASE = 0, Clock = 150MHz / 5 = 30MHz

	QSPI_Enable(QSPI);

    *spi_handle = spi_handle_local;

	return status;
}

uint32_t xtio_spi_get_instance_size(void)
{
    uint32_t total_size = 0;
    total_size += sizeof(xtio_spi_handle_t);
    total_size += sizeof(QspiInstFrame_t);
    return total_size;
}

int xtio_set_spi_mode(void * spi_handle, xtio_spi_mode_t xtio_spi_mode)
{
    uint32_t status = XT_SUCCESS;

    return status;
}

int xtio_get_spi_mode(void * spi_handle, xtio_spi_mode_t * xtio_spi_mode)
{
    uint32_t status = XT_SUCCESS;

    return status;
}

int xtio_spi_write(void * spi_handle, uint8_t * write_buffer, uint32_t number_of_bytes_to_write)
{
    uint32_t status = XT_SUCCESS;

    return status;
}

int xtio_spi_read(void * spi_handle, uint8_t * read_buffer, uint32_t number_of_bytes_to_read)
{
    uint32_t status = XT_SUCCESS;

    return status;
}

uint32_t use_quad_spi = 0;

int xtio_spi_write_read(void * spi_handle, uint8_t * write_buffer, uint32_t number_of_bytes_to_write, uint8_t * read_buffer, uint32_t number_of_bytes_to_read)
{
	if ((0 == number_of_bytes_to_read) && (0 == number_of_bytes_to_write))
	{
		return XT_SUCCESS;
	}

	int status = XT_SUCCESS;

	qspiDma.fCallbackTx = NULL;
	qspiDma.pArgTx = NULL;
	qspiDma.fCallbackRx = NULL;
	qspiDma.pArgRx = NULL;
		
	qspiDma.Qspid.qspiCommand.Instruction = write_buffer[0];
	qspiDma.Qspid.qspiBuffer.pDataTx = (uint32_t*)&write_buffer[1];
	qspiDma.Qspid.qspiBuffer.TxDataSize = number_of_bytes_to_write - 1;
		
	qspiDma.Qspid.qspiBuffer.pDataRx = (uint32_t*)read_buffer;
	qspiDma.Qspid.qspiBuffer.RxDataSize = number_of_bytes_to_read;

	qspi_frame->InstFrame.bm.bInstEn = 1;
	qspi_frame->InstFrame.bm.bDataEn = 1;
	qspi_frame->InstFrame.bm.bAddrEn = 0;
	qspi_frame->InstFrame.bm.bOptEn = 0;
	
	if (use_quad_spi == 0)
	{
		qspi_frame->InstFrame.bm.bwidth = QSPI_IFR_WIDTH_SINGLE_BIT_SPI;
	}
	else
	{
		qspi_frame->InstFrame.bm.bwidth = QSPI_IFR_WIDTH_QUAD_CMD;
	}
	
	qspiDma.Qspid.pQspiFrame =  qspi_frame;
	qspiDma.Qspid.qspiCommand.Option = 0;

	Access_t qspi_access = ReadAccess;
	uint32_t ulNotificationValue;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200);

	QSPI_CS_ASSERT();
	xt_delay_us(1);
	
	if ((number_of_bytes_to_read == 0) && (number_of_bytes_to_write == 1)) // Instruction only
	{
		qspi_access = CmdAccess;
		
		qspi_frame->InstFrame.bm.bXfrType = (QSPI_IFR_TFRTYP_TRSFR_READ >> QSPI_IFR_TFRTYP_Pos);
		
		qspi_frame->InstFrame.bm.bDataEn = 0;
		
		if (QSPI_SendCommand(&qspiDma.Qspid, 0) != QSPI_SUCCESS)
		{
			status = XT_ERROR;
		}
	} else if (number_of_bytes_to_read == 0) // Write instruction
	{
		qspi_access = WriteAccess;

		qspi_frame->InstFrame.bm.bXfrType = (QSPI_IFR_TFRTYP_TRSFR_WRITE >> QSPI_IFR_TFRTYP_Pos);
		
		if (QSPI_SendCommandWithData(&qspiDma.Qspid, 0) != QSPI_SUCCESS)
		{
			status = XT_ERROR;
		}
	} else if (number_of_bytes_to_write != 0) 
	{
		qspi_access = ReadAccess;

		qspi_frame->InstFrame.bm.bXfrType = (QSPI_IFR_TFRTYP_TRSFR_READ >> QSPI_IFR_TFRTYP_Pos);
				
		// Use DMA if read buffer is word-aligned and read size is sufficient 
		if ((number_of_bytes_to_read >= 32) && ((((uint32_t)read_buffer) % 32) == 0))
		{
			// Register callback for completed DMA transfer
			TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();
			qspiDma.fCallbackRx = xtio_spi_cb;
			qspiDma.pArgRx = task_handle;
			
			int remainder_after_word_aligned_accesses = number_of_bytes_to_read % 32;
			int number_of_bytes_to_read_using_word_aligned_accesses = number_of_bytes_to_read - remainder_after_word_aligned_accesses;
			
			qspiDma.Qspid.qspiBuffer.RxDataSize = number_of_bytes_to_read_using_word_aligned_accesses;
			
			// Need to make sure there are no dirty flags set in cache when going into the DMA transfer
			// otherwise it might cause memory to be corrupted by write-back after transfer.
			SCB_CleanInvalidateDCache_by_Addr ((uint32_t*)read_buffer, number_of_bytes_to_read_using_word_aligned_accesses);

			if (QSPID_EnableQspiRxChannel(&qspiDma) != QSPI_SUCCESS)
			{
				status = XT_ERROR;
			}

			if (QSPI_EnableMemAccess(&qspiDma.Qspid, 1, 0) != QSPI_SUCCESS)
			{
				status = XT_ERROR;
			}
			if (QSPID_ReadWriteQSPI(&qspiDma, qspi_access) != QSPI_SUCCESS)
			{
				status = XT_ERROR;
			}

			ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);

			QSPID_DisableQspiRxChannel(&qspiDma);

			if (ulNotificationValue == 0)
			{
				status = XT_ERROR;
			}
			
			// Setup for remainder of request, no new instruction
			qspi_frame->InstFrame.bm.bInstEn = 0;
			qspiDma.Qspid.qspiBuffer.pDataRx = (uint32_t*)&read_buffer[number_of_bytes_to_read_using_word_aligned_accesses];
			qspiDma.Qspid.qspiBuffer.RxDataSize = remainder_after_word_aligned_accesses;
			qspiDma.fCallbackRx = NULL;
			qspiDma.pArgRx = NULL;
		}
		
		// Read rest of request
		if (qspiDma.Qspid.qspiBuffer.RxDataSize)
		{
			QSPI_ReadCommand(&qspiDma.Qspid, 0);
		}
	} else
	{
		status = XT_ERROR;
	}

	QSPI_CS_DEASSERT();
	xt_delay_us(1);

	return status;
}
