#include "chip.h"
#include "xtio_uart.h"
#include "xep_hal.h"
#include "xtmemory.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "xdmad.h"
#include "pmc.h"
#include "string.h"

#define USART_SERIAL                USART0
#define USART_SERIAL_ID             ID_USART0  //USART0 for sam4l
#define USART_SERIAL_IRQ            USART0_IRQn

#define SPI_SERIAL_BUFFER_COUNT     2
#define SERIAL_RX_BUFFER_SIZE       (1024)
#define UART_SERIAL_BAUDRATE        115200U // MAC vs WIN // 921600
#define UART_BAUDRATE_MIN           600U
#define UART_BAUDRATE_MAX           10000000U

volatile uint8_t* spi_dma_buffer = NULL;
volatile uint8_t* spi_buffer = NULL;
volatile uint32_t spi_buffer_count = 0;

static int uart_baudrate = UART_SERIAL_BAUDRATE;

static UsartDma usartDma;
static UsartChannel usartRxChannel;
static UsartChannel usartTxChannel;
extern sXdmad dmad;

static const Pin spi_pins[] = PINS_USART_SPI;
static const Pin usart_pins[] = PINS_USART;
static const Pin uart_pins[] = PINS_UART;
static const Pin uart_rx_pin[] = PINS_UART_RX;

bool synchronous_mode = false;

bool waiting_for_rising_edge = false;

static void xtio_uart_cb(uint8_t channel, TaskHandle_t pArg);
static void USART0_DMA_RX_Handler(uint32_t channel, void *pArg);

typedef struct _LinkedListDescriptorView1 {
	// Next Descriptor Address number.
	uint32_t mbr_nda;
	// Microblock Control Member.
	uint32_t mbr_ubc;
	// Source Address Member.
	uint32_t mbr_sa;
	// Destination Address Member.
	uint32_t mbr_da;
} LinkedListDescriptorView1 __attribute__ ((aligned (4)));

typedef enum { WRITING, NEXT, FULL } buffer_state_t;

typedef struct {
    volatile uint8_t* data;
    uint32_t length;
    uint32_t read;
    
    volatile buffer_state_t state;
    
    LinkedListDescriptorView1 descriptor;
} dma_rx_buffer;

volatile dma_rx_buffer receive_buffer[2];

volatile TaskHandle_t task_handle = NULL;
volatile TaskHandle_t _receive_task_handle = NULL;
comm_mode_t _mode;

volatile bool nss_rising_edge = true;
volatile bool transmitting = false;

int xtio_uart_init(comm_mode_t mode)
{
    int32_t status = XTIO_SUCCESS;

    _receive_task_handle = NULL;
    
    // Fill buffer descriptors, using double buffering
    for (int i = 0; i < 2; i++)
    {
        receive_buffer[i].data = (uint8_t *)xtmemory_malloc_default(SERIAL_RX_BUFFER_SIZE);
        receive_buffer[i].length = SERIAL_RX_BUFFER_SIZE;
        
        // Calculate DMA alignment
        if ((((uint32_t)receive_buffer[i].data) % 32) != 0)
        {
            int alignment_diff = 32 - (((uint32_t)receive_buffer[i].data) % 32);
            receive_buffer[i].data += alignment_diff;
            receive_buffer[i].length -= alignment_diff;
        }
        receive_buffer[i].length -= receive_buffer[i].length % 32;
        
        receive_buffer[i].read = 0;
        receive_buffer[i].state = i ? NEXT : WRITING;
        receive_buffer[i].descriptor.mbr_nda = (uint32_t) &(receive_buffer[!i].descriptor);
        receive_buffer[i].descriptor.mbr_ubc =  XDMA_UBC_NVIEW_NDV1 |
        XDMA_UBC_NDE_FETCH_EN |
        XDMA_UBC_NSEN_UPDATED |
        XDMA_UBC_NDEN_UPDATED |
        receive_buffer[i].length;
        receive_buffer[i].descriptor.mbr_sa = (uint32_t) &(USART0->US_RHR);
        receive_buffer[i].descriptor.mbr_da = (uint32_t) receive_buffer[i].data;
        
        // Clean and invalidate cache of memory for descriptor for DMA, and data buffers
        SCB_CleanInvalidateDCache_by_Addr ((uint32_t*)receive_buffer[i].data, receive_buffer[i].length);
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)&(receive_buffer[i].descriptor), sizeof(LinkedListDescriptorView1));
    }
    usartDma.pXdmad = &dmad;
    usartTxChannel.dmaProgress = 1;
    usartRxChannel.dmaProgress = 1;

    status = xtio_uart_configure(mode);

    return status;
}

int xtio_uart_set_baudrate(int baudrate)
{
    if ((baudrate < UART_BAUDRATE_MIN) || (baudrate > UART_BAUDRATE_MAX))
        return XT_ERROR;

    uart_baudrate = baudrate;
    
    return xtio_uart_configure(UNCHANGED_MODE);
}

int xtio_uart_configure(comm_mode_t mode)
{
    if (!usartTxChannel.dmaProgress)
        return XT_ERROR;
    
    if (mode != UNCHANGED_MODE)
        _mode = mode;
    if (_mode == USRT_MODE)
    {
        PIO_Configure(usart_pins, PIO_LISTSIZE(usart_pins));
    } else if (_mode == UART_MODE)
    {
        PIO_Configure(uart_pins, PIO_LISTSIZE(uart_pins));
    } else if (_mode == UART_RX_MODE) 
    { 
        PIO_Configure(uart_rx_pin, PIO_LISTSIZE(uart_rx_pin));
    } else
    {
        PIO_Configure(spi_pins, PIO_LISTSIZE(spi_pins));
    }

    ENABLE_PERIPHERAL(USART_SERIAL_ID);
    NVIC_DisableIRQ(USART0_IRQn);
    PMC_EnablePeripheral(ID_USART0);

    uint32_t uart_config;
    if (_mode == SPI_MODE)
    {
        uart_config =   US_MR_USART_MODE_SPI_SLAVE | \
                        US_MR_USCLKS_SCK | \
                        US_MR_CHRL_8_BIT | \
                        (1<<8);
    } else if (_mode == USRT_MODE) 
    {
        uart_config =   US_MR_USART_MODE_NORMAL | \
                        US_MR_USCLKS_SCK | \
                        US_MR_CHRL_8_BIT | \
                        US_MR_PAR_NO | \
                        US_MR_NBSTOP_1_BIT | \
                        US_MR_CHMODE_NORMAL | \
                        US_MR_SYNC;
    } else
    {
        uart_config =   US_MR_USART_MODE_NORMAL | \
                        US_MR_USCLKS_MCK | \
                        US_MR_CHRL_8_BIT | \
                        US_MR_PAR_NO | \
                        US_MR_NBSTOP_1_BIT | \
                        US_MR_CHMODE_NORMAL;
    }

    XDMAD_StopTransfer(&dmad, 1);
    XDMAD_FreeChannel(&dmad, 1);
    
    if (transmitting)
    {
        USART_DisableIt(USART_SERIAL, US_IER_TXEMPTY);
        
        transmitting = false;
        
        xTaskNotifyGive(task_handle);
    }
    
    // Reset DMA buffers
    for (int i = 0; i < 2; i++)
    {
        receive_buffer[i].read = 0;
        receive_buffer[i].state = i ? NEXT : WRITING;
    }

    USARTD_Configure(&usartDma,
                       USART_SERIAL_ID,
                       uart_config,
                       uart_baudrate,
                       BOARD_MCK);
    
    if (_mode == SPI_MODE)
    {
        USART0->US_CR = US_CR_TXDIS | US_CR_RXDIS | US_CR_RSTRX | US_CR_RSTTX | US_CR_RSTSTA;
        
        USART0->US_IER = (1<<19); // Activate NSSE interrupt
        
        USART0->US_CR = US_CR_TXEN | US_CR_RXEN;
    }

    NVIC_ClearPendingIRQ(USART_SERIAL_IRQ);
    NVIC_SetPriority(USART_SERIAL_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(USART_SERIAL_IRQ);

    PMC_EnablePeripheral(ID_XDMAC);

    if (XDMAD_AllocateChannel(&dmad, XDMAD_TRANSFER_MEMORY, ID_USART0, 1) == XDMAD_ALLOC_FAILED)
        return XT_ERROR;
    XDMAD_SetCallback(&dmad, 1, USART0_DMA_RX_Handler, NULL);
    XDMAC_EnableGIt(dmad.pXdmacs, 1);

    if (_mode != SPI_MODE)
    {
        XDMAC->XDMAC_GD = XDMAC_GD_DI1;
        
        XDMAC->XDMAC_CHID[1].XDMAC_CC = XDMAC_CC_TYPE_PER_TRAN |
                                        XDMAC_CC_MBSIZE_SINGLE |
                                        XDMAC_CC_DSYNC_PER2MEM |
                                        XDMAC_CC_CSIZE_CHK_1 |
                                        XDMAC_CC_DWIDTH_BYTE |
                                        XDMAC_CC_SIF_AHB_IF1 |
                                        XDMAC_CC_DIF_AHB_IF1 |
                                        XDMAC_CC_SAM_FIXED_AM |
                                        XDMAC_CC_DAM_INCREMENTED_AM |
                                        XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber(USART_SERIAL_ID, XDMAD_TRANSFER_RX));
        
        XDMAC->XDMAC_CHID[1].XDMAC_CNDA = ((uint32_t)&(receive_buffer[0].descriptor)) | XDMAC_CNDA_NDAIF;
        XDMAC->XDMAC_CHID[1].XDMAC_CNDC =   XDMAC_CNDC_NDE_DSCR_FETCH_EN | 
                                            XDMAC_CNDC_NDSUP_SRC_PARAMS_UPDATED | 
                                            XDMAC_CNDC_NDDUP_DST_PARAMS_UPDATED |
                                            XDMAC_CNDC_NDVIEW_NDV1;
        XDMAC->XDMAC_CHID[1].XDMAC_CBC = 0;
        XDMAC->XDMAC_CHID[1].XDMAC_CSUS = 0;
        XDMAC->XDMAC_CHID[1].XDMAC_CDUS = 0;
        XDMAC->XDMAC_CHID[1].XDMAC_CID = 0xFFFFFFFF;
        XDMAC->XDMAC_CHID[1].XDMAC_CIE = XDMAC_CIE_BIE;

        XDMAC->XDMAC_GE = XDMAC_GE_EN1;
    } else
    {
        XDMAC->XDMAC_GD = XDMAC_GD_DI1;
        
        // In SPI mode, there is a limitation that commands must be shorter 
        // than the buffer size and contained within an asserted chip select.
        // One of the buffers will be used as a holding space for waiting commands. 
        spi_buffer = NULL;
        spi_buffer_count = 0;

        spi_dma_buffer = receive_buffer[0].descriptor.mbr_da;
    
        XDMAC->XDMAC_CHID[1].XDMAC_CSA = receive_buffer[0].descriptor.mbr_sa;
        XDMAC->XDMAC_CHID[1].XDMAC_CDA = receive_buffer[0].descriptor.mbr_da;
        XDMAC->XDMAC_CHID[1].XDMAC_CUBC = receive_buffer[0].descriptor.mbr_ubc;
        XDMAC->XDMAC_CHID[1].XDMAC_CC = XDMAC_CC_TYPE_PER_TRAN |
                                        XDMAC_CC_MBSIZE_SINGLE |
                                        XDMAC_CC_DSYNC_PER2MEM |
                                        XDMAC_CC_CSIZE_CHK_1 |
                                        XDMAC_CC_DWIDTH_BYTE |
                                        XDMAC_CC_SIF_AHB_IF1 |
                                        XDMAC_CC_DIF_AHB_IF1 |
                                        XDMAC_CC_SAM_FIXED_AM |
                                        XDMAC_CC_DAM_INCREMENTED_AM |
                                        XDMAC_CC_SWREQ_HWR_CONNECTED | 
                                        XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber(USART_SERIAL_ID, XDMAD_TRANSFER_RX));
        
        XDMAC->XDMAC_CHID[1].XDMAC_CNDA = 0;
        XDMAC->XDMAC_CHID[1].XDMAC_CNDC = 0;
        XDMAC->XDMAC_CHID[1].XDMAC_CBC = 0;
        XDMAC->XDMAC_CHID[1].XDMAC_CDS_MSP = 0;
        XDMAC->XDMAC_CHID[1].XDMAC_CSUS = 0;
        XDMAC->XDMAC_CHID[1].XDMAC_CDUS = 0;
        XDMAC->XDMAC_CHID[1].XDMAC_CID = 0xFFFFFFFF;
        XDMAC->XDMAC_CHID[1].XDMAC_CIE = XDMAC_CIE_BIE;

        XDMAC->XDMAC_GE = XDMAC_GE_EN1;
    }

    return XTIO_SUCCESS;
}

int xtio_uart_send(uint8_t * buffer, uint32_t length, int time_out_definition)
{
    if (_mode == UART_RX_MODE)
        return XT_ERROR;
    if (!usartTxChannel.dmaProgress)
        return XT_ERROR;
        
    int status = XTIO_SUCCESS;
    
    task_handle = xTaskGetCurrentTaskHandle();
    usartTxChannel.callback = xtio_uart_cb;
    usartTxChannel.pArgument = (void*)task_handle;
    
    uint32_t clean_addr = ((uint32_t)buffer) & ~(32-1);
    uint32_t clean_length = length + (buffer - clean_addr);
    if ((clean_length % 32) != 0)
        clean_length = clean_length + (32 - (clean_length % 32));
    SCB_CleanInvalidateDCache_by_Addr ((uint32_t*)clean_addr, clean_length);

    usartTxChannel.pBuff = buffer;
    usartTxChannel.BuffSize = length;
    
    usartTxChannel.dmaProgrammingMode = XDMAD_SINGLE;
    
    usartDma.pTxChannel = &usartTxChannel;

    if (_mode == SPI_MODE)
    {
        // For SPI, the setup of transmission can not be done while clocking.
        // If it is done while clocking, it could cause the data to be bit-shifted and thereby corrupted.
        // Set a flag to indicate that we wish to setup transmission in the interrupt at rising edge of CS. 
        waiting_for_rising_edge = true;
    } else
    {
        USART0->US_CR = US_CR_RSTTX | US_CR_RSTSTA;
        USART0->US_CR = US_CR_TXEN;

        USARTD_EnableTxChannels(&usartDma, &usartTxChannel);
        USARTD_SendData(&usartDma);
        transmitting = true;
    }

    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(2000);
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    waiting_for_rising_edge = false;

    USART_DisableIt(USART_SERIAL, US_IER_TXEMPTY);
    USARTD_DisableTxChannels(&usartDma, &usartTxChannel);
    
    if (ulNotificationValue == 0)
    {
        status = XT_ERROR;
        
        USART0->US_CR = US_CR_RSTTX;
        USART0->US_CR = US_CR_TXEN;
        USART0->US_THR = 0xFF;
    }
    
    return status;
}

int xtio_uart_receive(uint8_t * buffer, uint32_t * length)
{
    int status = XTIO_SUCCESS;

    if (_receive_task_handle == NULL)
        _receive_task_handle = xTaskGetCurrentTaskHandle();

    int max_buffer_size = *length;
    *length = 0;

    if (_mode == SPI_MODE)
    {
        if (spi_buffer_count)
        {
            int to_read = spi_buffer_count;
            if (to_read > max_buffer_size)
                to_read = max_buffer_size;
                
                
            int start_address_of_aligned_section = ((uint32_t)spi_buffer) & ~(32-1); // Mask out lower bits to achieve 32B alignment
            int length_to_clean = (((uint32_t) (spi_buffer)) - start_address_of_aligned_section) + to_read;

            int remainder_after_word_aligned_accesses = length_to_clean % 32;
            int number_of_bytes_to_read_using_word_aligned_accesses = length_to_clean + (32 - remainder_after_word_aligned_accesses);
                
            SCB_CleanInvalidateDCache_by_Addr ((uint32_t*)start_address_of_aligned_section, number_of_bytes_to_read_using_word_aligned_accesses);

            for (int i = 0; i < to_read; i++)
                buffer[i] = spi_buffer[i];
            *length = to_read;
            spi_buffer_count -= to_read;
        }

        return XT_SUCCESS;
    }

    // Flush DMA
    XDMAC->XDMAC_GSWF = XDMAC_GSWF_SWF1;
    uint32_t remaining_to_read = XDMAC->XDMAC_CHID[1].XDMAC_CUBC;

    irqflags_t flags = cpu_irq_save();
    if ((receive_buffer[0].state == FULL) || (receive_buffer[1].state == FULL))
    {
        // Empty the full buffer, from read pointer
        int full_index = 0;
        if (receive_buffer[1].state == FULL)
            full_index = 1;
        cpu_irq_restore(flags);
        
        int read_by_dma = receive_buffer[full_index].length;
        int read_by_comm = receive_buffer[full_index].read;

        int to_read = read_by_dma - read_by_comm;
        if (to_read > max_buffer_size)
            to_read = max_buffer_size;
    
        int start_address_of_aligned_section = ((uint32_t)&(receive_buffer[full_index].data[read_by_comm])) & ~(32-1); // Mask out lower bits to achieve 32B alignment
        int length_to_clean = (uint32_t) (&(receive_buffer[full_index].data[read_by_comm]) - start_address_of_aligned_section) + to_read;

        int remainder_after_word_aligned_accesses = length_to_clean % 32;
        int number_of_bytes_to_read_using_word_aligned_accesses = length_to_clean + (32 - remainder_after_word_aligned_accesses);
    
        SCB_CleanInvalidateDCache_by_Addr ((uint32_t*)start_address_of_aligned_section, number_of_bytes_to_read_using_word_aligned_accesses);

        for (int i = 0; i < to_read; i++)
            buffer[i] =  receive_buffer[full_index].data[read_by_comm + i];
        *length = to_read;
        receive_buffer[full_index].read += to_read;
        
        if (receive_buffer[full_index].read == receive_buffer[full_index].length)
        {
            receive_buffer[full_index].read = 0;
            receive_buffer[full_index].state = NEXT;
        }

        return XTIO_REPEAT;
    } else
    {
        // Read unread data in writing buffer
        int writing_index = 0;
        if (receive_buffer[1].state == WRITING)
            writing_index = 1;
        cpu_irq_restore(flags);
        
        int read_by_dma = receive_buffer[writing_index].length - remaining_to_read;
        int read_by_comm = receive_buffer[writing_index].read;
        int to_read = read_by_dma - read_by_comm;
        if (to_read > max_buffer_size)
            to_read = max_buffer_size;
    
        if (to_read)
        {            
            int start_address_of_aligned_section = ((uint32_t)&(receive_buffer[writing_index].data[read_by_comm])) & ~(32-1); // Mask out lower bits to achieve 32B alignment
            int length_to_clean = (uint32_t) (&(receive_buffer[writing_index].data[read_by_comm]) - start_address_of_aligned_section) + to_read;
            
            int remainder_after_word_aligned_accesses = length_to_clean % 32;
            int number_of_bytes_to_read_using_word_aligned_accesses = length_to_clean + (32 - remainder_after_word_aligned_accesses);
            
            SCB_CleanInvalidateDCache_by_Addr ((uint32_t*)start_address_of_aligned_section, number_of_bytes_to_read_using_word_aligned_accesses);

            for (int i = 0; i < to_read; i++)
                buffer[i] = receive_buffer[writing_index].data[read_by_comm + i];
            *length = to_read;
            
            receive_buffer[writing_index].read += to_read;
        }
    }

    return status;
}

static void USART0_DMA_RX_Handler(uint32_t channel, void *pArg)
{
    int writing_index = 0;
    if (receive_buffer[1].state == WRITING)
        writing_index = 1;

    receive_buffer[writing_index].state = FULL;

    if (receive_buffer[!writing_index].state == FULL)
    {
        // Overflow, buffer not ready...
    }
    receive_buffer[writing_index^1].state = WRITING;

    if ((_mode != SPI_MODE) &&
        (_receive_task_handle != NULL))
    {
        portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(_receive_task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}

// Callback from DMA on completed transfer
// Note that the DMA thinks it is completed when all bytes have been transferred to module,
// but does not know if the bytes have been transferred out of the module. 
static void xtio_uart_cb(uint8_t channel, TaskHandle_t pArg)
{
    //task_handle = pArg;

    // Wait for entire transfer to be completed
    //USART_EnableIt(USART_SERIAL, US_IER_TXRDY);
    
    USART_EnableIt(USART_SERIAL, US_IER_TXEMPTY);
}

void USART0_Handler(void)
{
    uint32_t status = USART0->US_CSR;
    
    // Check chip select for SPI mode
    if ((_mode == SPI_MODE) && (status & (1<<19)))
    {
        if (status & (1<<23)) // Rising edge
        {
            nss_rising_edge = true;
            
            if (waiting_for_rising_edge)
            {
                waiting_for_rising_edge = false;
                transmitting = true;

                USART0->US_CR = US_CR_RSTTX | US_CR_RSTSTA;
                USART0->US_CR = US_CR_TXEN;
                USARTD_EnableTxChannels(&usartDma, &usartTxChannel);
                USARTD_SendData(&usartDma);
            } else if (transmitting && ((status & US_CSR_TXEMPTY) == 0)) // If transmitting, Bytes have been cleared from the shift register and THR
            {
                // Flush DMA, Suspend channel and read remaining count in DMA
                XDMAC->XDMAC_GSWF = XDMAC_GSWF_SWF0;
                uint32_t remaining_to_read = XDMAC->XDMAC_CHID[0].XDMAC_CUBC + 2;
                
                // Disable DMA channel and reset USART
                XDMAC->XDMAC_GD = 1<<0;
                USARTD_DisableTxChannels(&usartDma, &usartTxChannel);
                USART0->US_CR = US_CR_RSTTX | US_CR_RSTSTA;
                USART0->US_CR = US_CR_TXEN;
                
                // When no bytes are waiting for DMA transfer, we need to find out how much data is waiting in USART
                if (remaining_to_read == 2)
                {
                    if ((status & US_CSR_TXRDY) != 0) // Only one byte remaining, in shift register
                        remaining_to_read--;
                    USART0->US_IDR = US_IDR_TXRDY; // DMA transfer has been falsely marked as completed, need to unset TXRDY
                }
                
                // Reinitialize DMA
                uint32_t offset = usartTxChannel.BuffSize - remaining_to_read;
                usartTxChannel.pBuff = &usartTxChannel.pBuff[offset];
                usartTxChannel.BuffSize = remaining_to_read;
                USARTD_EnableTxChannels(&usartDma, &usartTxChannel);
                USARTD_SendData(&usartDma);
            }

            // Flush DMA
            uint32_t remaining_to_read = 0;
            uint32_t read_by_dma = 0;
            if (XDMAC->XDMAC_GS & (1<<1))
            {
                XDMAC->XDMAC_GSWF = XDMAC_GSWF_SWF1;
                while (XDMAC->XDMAC_CHID[1].XDMAC_CIS & (1<<3));
                remaining_to_read = XDMAC->XDMAC_CHID[1].XDMAC_CUBC;
                read_by_dma = receive_buffer[0].length - remaining_to_read;
            
                XDMAC->XDMAC_GD = XDMAC_GD_DI1;
                while (XDMAC->XDMAC_CHID[1].XDMAC_CIS & (1<<2));
            }

            // Check received data
            if ((receive_buffer[0].state == FULL) || (receive_buffer[1].state == FULL))
            {
                // Buffer has been filled, ignore data and reset DMA
                USART0->US_CR = US_CR_RSTRX;
                USART0->US_CR = US_CR_RXEN;
            } else
            {
                // Partial fill means we might have valid data, check first byte
                USART0->US_CR = US_CR_RSTRX;
                USART0->US_CR = US_CR_RXEN;
                
                if (read_by_dma)
                {
                    if (read_by_dma > 8)
                        for (volatile int i = 0; i < 10; i++);
                        
                    SCB_CleanInvalidateDCache_by_Addr ((uint32_t*)spi_dma_buffer, 1);
                    if (spi_buffer_count)
                    {
                        // OVERFLOW...
                    } else if ((spi_dma_buffer[0] != 0xFF) && (spi_dma_buffer[0] != 0x00))
                    {                        
                        // First byte indicates this is not dummy data, swap buffers and wait for command to be read.
                        spi_buffer = spi_dma_buffer;
                        spi_buffer_count = read_by_dma;
                    }
                }
            }

            int index = 0;
            if (spi_buffer_count)
                if (receive_buffer[index].data == spi_buffer)
                    index = 1;
            
            receive_buffer[0].state = WRITING;
            receive_buffer[1].state = NEXT;
            
            spi_dma_buffer = receive_buffer[index].descriptor.mbr_da;

            XDMAC->XDMAC_CHID[1].XDMAC_CSA = receive_buffer[index].descriptor.mbr_sa;
            XDMAC->XDMAC_CHID[1].XDMAC_CDA = receive_buffer[index].descriptor.mbr_da;
            XDMAC->XDMAC_CHID[1].XDMAC_CUBC = receive_buffer[index].descriptor.mbr_ubc;
            /*XDMAC->XDMAC_CHID[1].XDMAC_CC = XDMAC_CC_TYPE_PER_TRAN |
                                            XDMAC_CC_MBSIZE_SINGLE |
                                            XDMAC_CC_DSYNC_PER2MEM |
                                            XDMAC_CC_CSIZE_CHK_1 |
                                            XDMAC_CC_DWIDTH_BYTE |
                                            XDMAC_CC_SIF_AHB_IF1 |
                                            XDMAC_CC_DIF_AHB_IF1 |
                                            XDMAC_CC_SAM_FIXED_AM |
                                            XDMAC_CC_DAM_INCREMENTED_AM |
                                            XDMAC_CC_SWREQ_HWR_CONNECTED | 
                                            XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber(USART_SERIAL_ID, XDMAD_TRANSFER_RX));
        
            XDMAC->XDMAC_CHID[1].XDMAC_CNDA = 0;
            XDMAC->XDMAC_CHID[1].XDMAC_CNDC = 0;
            XDMAC->XDMAC_CHID[1].XDMAC_CBC = 0;
            XDMAC->XDMAC_CHID[1].XDMAC_CDS_MSP = 0;
            XDMAC->XDMAC_CHID[1].XDMAC_CSUS = 0;
            XDMAC->XDMAC_CHID[1].XDMAC_CDUS = 0;
            XDMAC->XDMAC_CHID[1].XDMAC_CID = 0xFFFFFFFF;
            XDMAC->XDMAC_CHID[1].XDMAC_CIE = XDMAC_CIE_BIE;*/
            
            XDMAC->XDMAC_GE = XDMAC_GE_EN1;
        } else
        {
            nss_rising_edge = false;
        }
    }
    
    // When TXEMPTY is set, transfer is fully completed
    if ((USART_GetItMask(USART_SERIAL) & US_IMR_TXEMPTY) && (status & US_CSR_TXEMPTY))
    {
        USART_DisableIt(USART_SERIAL, US_IER_TXEMPTY);
        
        transmitting = false;
        
        portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}