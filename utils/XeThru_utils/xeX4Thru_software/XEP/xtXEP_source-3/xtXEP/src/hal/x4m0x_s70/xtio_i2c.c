#include "board.h"
#include "xtio_i2c.h"
#include "xep_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "xtmemory.h"

uint8_t * i2c_receive_buffer;
uint32_t i2c_buffer_counter = 0;

uint8_t* transmit_buffer = NULL;
uint32_t transmit_length = 0;

TaskHandle_t i2c_task_handle = NULL;

#define I2C_RX_BUFFER_SIZE		(3000)
#define SLAVE_ADDRESS   0x59

// PINS_I2C is defined in BOARD_X4M0x_SAMS70
static const Pin irq_pin = {PIO_PA28, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_PULLUP | PIO_OPENDRAIN};
static const Pin i2c_pins[] = PINS_I2C;

#define I2C_IRQ_ASSERT()	PIO_Clear(&irq_pin)
#define I2C_IRQ_DEASSERT()	PIO_Set(&irq_pin)

int xtio_i2c_init(void)
{
    PIO_Configure(i2c_pins, PIO_LISTSIZE(i2c_pins));
    PIO_Configure(&irq_pin, 1);
    
    i2c_receive_buffer = (uint8_t *)xtmemory_malloc_default(I2C_RX_BUFFER_SIZE);

    PMC_EnablePeripheral(ID_TWIHS2);
    TWI_ConfigureSlave(TWIHS2, SLAVE_ADDRESS);

    TWI_ReadByte(TWIHS2);

    NVIC_ClearPendingIRQ(TWIHS2_IRQn);
	NVIC_SetPriority(TWIHS2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(TWIHS2_IRQn);

    TWI_EnableIt(TWIHS2, TWIHS_SR_SVACC);
    
    return XT_SUCCESS;
}

int xtio_i2c_send(uint8_t * buffer, uint32_t length, int time_out_definition)
{
    i2c_task_handle = xTaskGetCurrentTaskHandle();
    taskENTER_CRITICAL();
    transmit_buffer = buffer;
    transmit_length = length;
    taskEXIT_CRITICAL();

    I2C_IRQ_ASSERT();

    return XT_SUCCESS;
}

bool xtio_i2c_send_completed(void)
{
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);

    return (ulNotificationValue != 0);
}

int xtio_i2c_receive(uint8_t * buffer, uint32_t * length)
{
    int status = XTIO_SUCCESS;

    taskENTER_CRITICAL();
    if ((i2c_buffer_counter > 0) && (*length > i2c_buffer_counter))
    {
        *length = i2c_buffer_counter;
        for (uint32_t i = 0; i < i2c_buffer_counter; i++)
        {
            buffer[i] = i2c_receive_buffer[i];
        }
        i2c_buffer_counter = 0;
    }
    else
    {
        // Error - buffer not big enough
        i2c_buffer_counter = 0;
        *length = i2c_buffer_counter;
        status = XT_ERROR;
    }
    taskEXIT_CRITICAL();

	return XTIO_SUCCESS;
}

bool addressed = false;
bool send_in_progress = false;
bool receive_in_progress = false;
void TWIHS2_Handler( void )
{
    volatile uint32_t status;

    status = TWI_GetStatus(TWIHS2);
    if (((status & TWIHS_SR_SVACC) == TWIHS_SR_SVACC)) {
        if (!addressed) {
            TWI_DisableIt(TWIHS2, TWIHS_IDR_SVACC);
            TWI_EnableIt(TWIHS2, TWIHS_IER_RXRDY
                        | TWIHS_IER_EOSACC
                        | TWIHS_IER_SCL_WS );
            addressed = true;
        }

        if ((status & TWIHS_SR_GACC) == TWIHS_SR_GACC) {
            // No general call action
        } else {
            if ((status & TWIHS_SR_SVREAD) == TWIHS_SR_SVREAD) {
                /*Slave Read */
                if (((status & TWIHS_SR_TXRDY) == TWIHS_SR_TXRDY)
                    && ((status & TWIHS_SR_NACK) == 0)) {
                    // Write one byte of data from slave to master device
                    if (transmit_buffer != NULL)
                    {
                        if (transmit_length > 0)
                        {
                            send_in_progress = true;
                            TWI_WriteByte( TWIHS2, *transmit_buffer);
                            
                            transmit_length--;
                            transmit_buffer++;
                        } else
                        {
                            TWI_WriteByte( TWIHS2, 0xFF);
                            // Set IRQ low again
                            I2C_IRQ_DEASSERT();
                        }
                    } else
                    {
                        // Nothing to send, but has to send something...
                        TWI_WriteByte( TWIHS2, 0xFF);
                    }
                }
            } else {
                /*Slave Write*/
                if ((status & TWIHS_SR_RXRDY) == TWIHS_SR_RXRDY) {
                    uint8_t data_byte = TWI_ReadByte(TWIHS2);
                    if (!receive_in_progress)
                    {
                        if (data_byte == 0x10)
                            receive_in_progress = true;
                    } else
                    {
                        if (i2c_buffer_counter < I2C_RX_BUFFER_SIZE)
                        {
                            i2c_receive_buffer[i2c_buffer_counter++] = data_byte;
                        } else
                        {
                            // Buffer overflow. Some error handling?
                        }
                    }
                }
            }
        }
    } else {
        if ((status & TWIHS_SR_EOSACC) == TWIHS_SR_EOSACC) {
            if ((status & TWIHS_SR_TXCOMP) == TWIHS_SR_TXCOMP) {
                /* End of transfer, end of slave access */
                addressed = false;
                TWI_EnableIt( TWIHS2, TWIHS_SR_SVACC);
                TWI_DisableIt( TWIHS2, TWIHS_IDR_RXRDY
                                        | TWIHS_IDR_EOSACC
                                        | TWIHS_IDR_SCL_WS );
                
                if (send_in_progress)
                {
                    TWIHS2->TWIHS_CR |= (1<<24); // THRCLR, transmit hold register clear
                    I2C_IRQ_DEASSERT();
                    
                    send_in_progress = false;
                    transmit_buffer = NULL;
                    
                    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
                    vTaskNotifyGiveFromISR(i2c_task_handle, &xHigherPriorityTaskWoken);
                    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                } else if (receive_in_progress)
                {
                    receive_in_progress = false;
                }
            }
        }
    }
}