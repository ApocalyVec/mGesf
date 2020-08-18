/**
 * @file
 *
 * @brief Implement USB serial communication
 *
 * Using FreeRTOS
 */


#include <stdbool.h>
#include "xep_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "protocol.h"
#include "xtio_serial_com.h"
#include "xtio_usb.h"
#include "xtio_uart.h"
#include "xtio_i2c.h"
#include "compiler.h"
#include "USBD.h"
#include "CDCDSerialDriver.h"
#include "task_monitor.h"
#include "xt_config.h"

#define DEBUG_TOKEN_SPI 0x21495053
#define DEBUG_TOKEN_UART 0x54524155
#define DEBUG_TOKEN_USRT 0x54525355

/// @todo Check size of UART_RX_READ_BUFFER_SIZE buffer
#define UART_RX_READ_BUFFER_SIZE 512
/** Size in bytes of the packet used for reading data from the USB & USART */
#define DATAPACKETSIZE (512)

/** Size in bytes of the buffer used for reading data from the USB & USART */
#define DATABUFFERSIZE (DATAPACKETSIZE+2)

/** Buffer for storing incoming USB data. */
COMPILER_ALIGNED(32) static uint8_t usbBuffer[DATABUFFERSIZE];
static volatile int usbReceived = 0;

/** USB Tx flag */
static volatile uint8_t USB_txDoneFlag = 0;

static xtProtocol * xt_protocol_local;
static xtProtocol * xt_protocol_debug_local;

#define TASK_USB_RX_PRIORITY  (tskIDLE_PRIORITY + 4)
#define TASK_USB_RX_STACK_SIZE (150)
#define TASK_USB_TX_PRIORITY  (tskIDLE_PRIORITY + 5)
#define TASK_USB_TX_STACK_SIZE (150)

#define TASK_UART_RX_PRIORITY  (tskIDLE_PRIORITY + 4)
#define TASK_UART_RX_STACK_SIZE (320)
#define TASK_UART_TX_PRIORITY  (tskIDLE_PRIORITY + 5)
#define TASK_UART_TX_STACK_SIZE (150)

#define TASK_I2C_RX_PRIORITY  (tskIDLE_PRIORITY + 4)
#define TASK_I2C_RX_STACK_SIZE (2048)
#define TASK_I2C_TX_PRIORITY  (tskIDLE_PRIORITY + 5)
#define TASK_I2C_TX_STACK_SIZE (2048)

typedef struct {
        uint8_t * buf;
        uint32_t length;
        xtio_host_send_completed_t callback;
        void* callback_arg;
} com_tx_item_t;


static volatile bool usbEnabled = false;
static volatile bool usbIdle = true;
static volatile bool usbDebug = false;
static TaskHandle_t pTaskUsbRx;
static TaskHandle_t pTaskUsbTx;
static int _usb_task_init(xtProtocol * xt_protocol, bool primary);
static void _usb_task_deinit(void);
static void _usb_task_rx(void *pvParameters);
static void _usb_task_tx(void *pvParameters);
static bool _usb_available(void);
static bool _i2c_available(void);

static volatile bool uartEnabled = false;
static volatile bool uartIdle = true;
static volatile bool uartDebug = false;
static TaskHandle_t pTaskUartRx;
static TaskHandle_t pTaskUartTx;
static int _uart_task_init(xtProtocol * xt_protocol, bool primary);
static void _uart_task_rx(void *pvParameters);
static void _uart_task_tx(void *pvParameters);

static TaskHandle_t pTaskI2cRx;
static TaskHandle_t pTaskI2cTx;
static int _i2c_task_init(xtProtocol * xt_protocol, bool primary);
static void _i2c_task_rx(void *pvParameters);
static void _i2c_task_tx(void *pvParameters);



QueueHandle_t com_tx_queue;
QueueHandle_t com_tx_debug_queue;

bool _i2c_available(void)
{
    if ((communication_config[CONFIG_I2C_INDEX].module < 0x40000000) ||
        (communication_config[CONFIG_I2C_INDEX].module > 0x40100000))
    {
        return false;
    }

    // No I2C in factory mode
    if (xt_get_operation_mode() == XT_OPMODE_FACTORY)
    {
        return false;
    }

    return true;
}

bool _usb_available(void)
{
    bool usbPresent = true;
    xtio_pin_level_t pin_level = XTIO_PIN_LEVEL_LOW;
    int status;

    // Check config
    if (communication_config[CONFIG_USB_INDEX].module == 0xFFFFFFFF)
    {
        return false;
    }

    // No USB in factory mode
    if (xt_get_operation_mode() == XT_OPMODE_FACTORY)
    {
        return false;
    }

    for (int i = 0; i < 100; i++)
    {
        status = xtio_get_level(XTIO_USB_VBUS, &pin_level);
        if (XT_SUCCESS != status || 0 == pin_level)
        {
            usbPresent = false;
            break;
        }
        for (volatile int j = 0; j < 10000; j++);
    }
    return usbPresent;
}

int xtio_serial_com_init(xtProtocol * xt_protocol)
{
    int status = XT_SUCCESS;

    xt_protocol_local = xt_protocol;

    // Queue of single tx item. Buffering messages is done prior to this stage, using the dispatch'er and memory pool mechanisms.
    com_tx_queue = xQueueCreate(1, sizeof(com_tx_item_t));

    // Always select USB if VBUS is available on boot, use UART/USRT/SPI as debug interface. 
    // In case of no USB, check if I2C is present. If it is, use it, and use UART/USRT/SPI as debug interface. 
    // If neither USB or I2C is available, use UART/USRT/SPI as primary interface. Use USB as debug interface, wait for VBUS. 
    if (_usb_available())
    {
        // Init USB
        status = _usb_task_init(xt_protocol, true);
		UNUSED(_usb_task_deinit);
    } else if (_i2c_available())
        status = _i2c_task_init(xt_protocol, true);
    else
        status = _uart_task_init(xt_protocol, true);

    return status;
}

int xtio_serial_com_debug_init(xtProtocol * xt_protocol)
{
    int status = XT_SUCCESS;

    xt_protocol_debug_local = xt_protocol;

    // Queue of single tx item. Buffering messages is done prior to this stage, using the dispatch'er and memory pool mechanisms.
    com_tx_debug_queue = xQueueCreate(1, sizeof(com_tx_item_t));

    if (_usb_available())
        status = _uart_task_init(xt_protocol, false);
    else if (_i2c_available())
        status = _uart_task_init(xt_protocol, false);
    /*else if (communication_config[CONFIG_USB_INDEX].module != 0xFFFFFFFF)
        status = _usb_task_init(xt_protocol, false);*/
        
    return status;
}

int xtio_host_send(uint8_t * buffer, uint32_t length, xtio_host_send_completed_t callback, void* callback_arg)
{
    com_tx_item_t tx_item;
    tx_item.buf = buffer;
    tx_item.length = length;
    tx_item.callback = callback;
    tx_item.callback_arg = callback_arg;

    if (pdTRUE == xQueueSend(com_tx_queue, &tx_item, 100 / portTICK_PERIOD_MS))
    {
        return XT_SUCCESS;
    }
    else
    {
        return XT_ERROR;
    }
}

int xtio_host_send_debug(uint8_t * buffer, uint32_t length, xtio_host_send_completed_t callback, void* callback_arg)
{
    com_tx_item_t tx_item;
    tx_item.buf = buffer;
    tx_item.length = length;
    tx_item.callback = callback;
    tx_item.callback_arg = callback_arg;

    if (pdTRUE == xQueueSend(com_tx_debug_queue, &tx_item, 100 / portTICK_PERIOD_MS))
    {
        return XT_SUCCESS;
    }
    else
    {
        return XT_ERROR;
    }
}

int _usb_task_init(xtProtocol * xt_protocol, bool primary)
{
    usbEnabled = false;
    usbIdle = true;

    usbDebug = !primary;

    // Setup USB stack
    
    // No USB in factory mode
    if (xt_get_operation_mode() == XT_OPMODE_FACTORY)
    {
        return XT_ERROR;
    }
    
    xtio_usb_init();

    xTaskCreate(_usb_task_rx, (const char * const) "tskUsbRx", TASK_USB_RX_STACK_SIZE, NULL, TASK_USB_RX_PRIORITY, &pTaskUsbRx);
    xTaskCreate(_usb_task_tx, (const char * const) "tskUsbTx", TASK_USB_TX_STACK_SIZE, NULL, TASK_USB_TX_PRIORITY, &pTaskUsbTx);

    return XT_SUCCESS;
}

void _usb_task_deinit(void)
{
    // Signal that we intend to shutdown the USB stack and wait until
    // ongoing read/write operations are completed before shutdown.
    usbEnabled = false;
    while (!usbIdle)
    {
        vTaskDelay(10UL / portTICK_PERIOD_MS);
    }
}

/**
 * Callback invoked when data has been sent.
 */
static void _UsbDataSent(void* unused,
						 uint8_t status,
						 uint32_t received,
						 uint32_t remaining)
{
	(void) unused;
	(void) received;
	(void) remaining;
	if (status == USBD_STATUS_SUCCESS)
	{
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(pTaskUsbTx, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
	else
	{
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(pTaskUsbTx, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

/*----------------------------------------------------------------------------
 * Callback invoked when data has been received on the USB.
 *----------------------------------------------------------------------------*/
static void _UsbDataReceived(uint32_t unused,
							 uint8_t status,
							 uint32_t received,
							 uint32_t remaining)
{
	unused = unused;

	/* Check that data has been received successfully */
	if (status == USBD_STATUS_SUCCESS) 
    {
        usbReceived = received;
        portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(pTaskUsbRx, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	} else {
		//## TRACE_WARNING("_UsbDataReceived: Transfer error\n\r");
	}
}
uint8_t in_progress = 0;
static void _usb_task_tx(void *pvParameters)
{
    (void)pvParameters;
    com_tx_item_t tx_item;
    uint32_t status = XT_SUCCESS;

    vTaskDelay(2000UL / portTICK_PERIOD_MS); //##KIH
	USB_txDoneFlag = 1;
    monitor_task_t * monitor_task_handle;
    status = monitor_task_register(&monitor_task_handle, 1000, TASK_USB_TX_STACK_SIZE);

	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
	
	uint32_t ulNotificationValue = 0;
	
    while (true)
    {
        monitor_task_alive(monitor_task_handle);
        if (USBD_GetState() < USBD_STATE_CONFIGURED)
        {
            usbIdle = true;
            in_progress = 0;
            vTaskDelay(10UL / portTICK_PERIOD_MS);
            continue;
        }
        usbIdle = false;

        if (xQueueGenericReceive(usbDebug ? com_tx_debug_queue : com_tx_queue, &tx_item, 500 / portTICK_PERIOD_MS, pdTRUE))
        {
			// Data received from the queue. Ensure all bytes are written.
            uint32_t bytesLeft = tx_item.length;
            uint8_t *dataPtr = tx_item.buf;
			
            uint32_t clean_addr = ((uint32_t)dataPtr) & ~(32-1);
            uint32_t clean_length = bytesLeft + (dataPtr - clean_addr);
            if ((clean_length % 32) != 0)
                clean_length = clean_length + (32 - (clean_length % 32));
            SCB_CleanInvalidateDCache_by_Addr ((uint32_t*)clean_addr, clean_length);

            do
            {
                status = CDCDSerialDriver_Write((char *)dataPtr, bytesLeft, _UsbDataSent, 0);
            } while (status != USBD_STATUS_SUCCESS);
                
			do {
				ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
                monitor_task_alive(monitor_task_handle);
			} while (ulNotificationValue == 0);
				
            xQueueReceive(usbDebug ? com_tx_debug_queue : com_tx_queue, &tx_item, 0); // Now remove from queue.
            tx_item.callback(tx_item.callback_arg);
        }
    }
}

static void _usb_task_rx(void *pvParameters)
{
    (void)pvParameters;
    uint32_t status = XT_SUCCESS;

    monitor_task_t * monitor_task_handle;
    status = monitor_task_register(&monitor_task_handle, 1000, TASK_USB_RX_STACK_SIZE);

    in_progress = 0;

	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
    while (true)
    {
        monitor_task_alive(monitor_task_handle);

        if (USBD_GetState() < USBD_STATE_CONFIGURED)
        {
            usbIdle = true;
            in_progress = 0;
            vTaskDelay(10UL / portTICK_PERIOD_MS);
            continue;
        } else if ((usbIdle) || (in_progress == 0))
        {
            usbIdle = false;
            in_progress = 1;
            
            /* Start receiving data on the USB */
            CDCDSerialDriver_Read(usbBuffer,
                            DATAPACKETSIZE,
                            (TransferCallback) _UsbDataReceived,
                            0);
        }

        uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        if (ulNotificationValue != 0)
        {
            parseData(usbDebug ? xt_protocol_debug_local : xt_protocol_local, usbBuffer, usbReceived); // Parse received data.
            
            /* Start receiving data on the USB */
            CDCDSerialDriver_Read(usbBuffer,
                            DATAPACKETSIZE,
                            (TransferCallback) _UsbDataReceived,
                            0);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// UART part
//

int _uart_task_init(xtProtocol * xt_protocol, bool primary)
{
    int status = XT_SUCCESS;

    // Check communication mode
    comm_mode_t mode = UART_MODE;
    if (xt_get_operation_mode() == XT_OPMODE_SYNC_SERIAL)
        mode = USRT_MODE;
    else if (xt_get_operation_mode() == XT_OPMODE_SPI)
        mode = SPI_MODE;

    uartEnabled = false;
    uartIdle = false;
    
    uartDebug = !primary;

    if (primary)
    {
        xtio_uart_init(mode);
        
        uartEnabled = true;
    } else
    {
        xtio_uart_init(UART_RX_MODE);
        xtio_uart_set_baudrate(600);
    }
    
    xTaskCreate(_uart_task_rx, (const char * const) "tskUartRx", TASK_UART_RX_STACK_SIZE, NULL, TASK_UART_RX_PRIORITY, &pTaskUartRx);
    xTaskCreate(_uart_task_tx, (const char * const) "tskUartTx", TASK_UART_TX_STACK_SIZE, NULL, TASK_UART_TX_PRIORITY, &pTaskUartTx);

    return status;
}

static void _uart_task_tx(void *pvParameters)
{
    (void)pvParameters;
    com_tx_item_t tx_item;
    int status = XT_SUCCESS;
	const uint32_t time_out_definition = (100UL / portTICK_PERIOD_MS);

    monitor_task_t * monitor_task_handle;
    status = monitor_task_register(&monitor_task_handle, 3000, TASK_UART_TX_STACK_SIZE);

    while (true)
    {
        monitor_task_alive(monitor_task_handle);
        if (!uartEnabled)
        {
            if (xQueueReceive(uartDebug ? com_tx_debug_queue : com_tx_queue, &tx_item, 100 / portTICK_PERIOD_MS))
                tx_item.callback(tx_item.callback_arg);
            continue;
        }
        uartIdle = false;

        if (xQueueGenericReceive(uartDebug ? com_tx_debug_queue : com_tx_queue, &tx_item, 500 / portTICK_PERIOD_MS, pdTRUE))
        {
            // Item fetched from queue, but not removed from the queue yet. Continue blocking next send.
            status = xtio_uart_send(tx_item.buf, tx_item.length, time_out_definition);
    		if (status == XTIO_SUCCESS)
    		{
                xQueueReceive(uartDebug ? com_tx_debug_queue : com_tx_queue, &tx_item, 0); // Successfully transmitted, now remove from queue.
                tx_item.callback(tx_item.callback_arg);
            } else
            {
                // Don't do anything. Will retry on next pass.
                vTaskDelay(10UL / portTICK_PERIOD_MS);
            }
        }
    }
}

static void _uart_task_rx(void *pvParameters)
{
    (void)pvParameters;
    int status = XTIO_SUCCESS;
    uint8_t rxbuff[UART_RX_READ_BUFFER_SIZE];
	uint32_t length = 0;

    monitor_task_t * monitor_task_handle;
    status = monitor_task_register(&monitor_task_handle, 1000, TASK_UART_RX_STACK_SIZE);

    int debug_received = 0;
    while (true)
    {
        monitor_task_alive(monitor_task_handle);
        if (!uartEnabled)
        {
            if (uartDebug)
            {
                length = UART_RX_READ_BUFFER_SIZE;
                status = xtio_uart_receive(&rxbuff[debug_received], &length);
                debug_received += length;
                
                while (debug_received >= 4)
                {
                    uint32_t debug_token = rxbuff[0] + (rxbuff[1]<<8) + (rxbuff[2]<<16) + (rxbuff[3]<<24);
                    switch (debug_token)
                    {
                        case DEBUG_TOKEN_UART:
                            debug_received = 0;
                            xtio_uart_configure(UART_MODE);
                            xtio_uart_set_baudrate(115200);
                            uartEnabled = true;
                            break;
                        case DEBUG_TOKEN_USRT:
                            debug_received = 0;
                            xtio_uart_configure(USRT_MODE);
                            uartEnabled = true;
                            break;
                        case DEBUG_TOKEN_SPI:
                            debug_received = 0;
                            xtio_uart_configure(SPI_MODE);
                            uartEnabled = true;
                            break;
                        default:
                            {
                                for (int i = 1; i < debug_received; i++)
                                    rxbuff[i-1] = rxbuff[i];
                                debug_received--;
                                break;
                            }
                    }
                }
            }
            
            uartIdle = true;
            vTaskDelay(100UL / portTICK_PERIOD_MS);
            continue;
        }
        uartIdle = false;

        length = UART_RX_READ_BUFFER_SIZE;
        status = xtio_uart_receive(rxbuff, &length);
        if (length > 0)
        {
            uint32_t bytesRead = length;
            
            // Note that partial reads are perfectly fine here as remaining
            // bytes will be read in the next iteration
            parseData(uartDebug ? xt_protocol_debug_local : xt_protocol_local, rxbuff, bytesRead); // Parse received data.
        }
        
        if (status != XTIO_REPEAT)
            ulTaskNotifyTake(pdTRUE, 1UL / portTICK_PERIOD_MS);
    }
}

////////////////////////////////////////////////////////////////////////////////
// I2C part
//

int _i2c_task_init(xtProtocol * xt_protocol, bool primary)
{
    int status = XT_SUCCESS;

    xtio_i2c_init();

    xTaskCreate(_i2c_task_rx, (const char * const) "tskI2cRx", TASK_I2C_RX_STACK_SIZE, NULL, TASK_I2C_RX_PRIORITY, &pTaskI2cRx);
    xTaskCreate(_i2c_task_tx, (const char * const) "tskI2cTx", TASK_I2C_TX_STACK_SIZE, NULL, TASK_I2C_TX_PRIORITY, &pTaskI2cTx);

    return status;
}

static void _i2c_task_tx(void *pvParameters)
{
    (void)pvParameters;
    com_tx_item_t tx_item;
    int status = XTIO_SUCCESS;
    
    monitor_task_t * monitor_task_handle;
    status = monitor_task_register(&monitor_task_handle, 1000, TASK_I2C_TX_STACK_SIZE);

	const uint32_t time_out_definition = (100UL / portTICK_PERIOD_MS);

    while (true)
    {
        monitor_task_alive(monitor_task_handle);

        if (xQueueGenericReceive(com_tx_queue, &tx_item, 500 / portTICK_PERIOD_MS, pdTRUE))
        {
            // Item fetched from queue, but not removed from the queue yet. Continue blocking next send.
            xtio_i2c_send(tx_item.buf, tx_item.length, time_out_definition);
            
            while (!xtio_i2c_send_completed())
            {
                monitor_task_alive(monitor_task_handle);
                vTaskDelay(10UL / portTICK_PERIOD_MS);
            }

            xQueueReceive(com_tx_queue, &tx_item, 0); // Now remove from queue.
            tx_item.callback(tx_item.callback_arg);
        }
    }
}

static void _i2c_task_rx(void *pvParameters)
{
    (void)pvParameters;
    int status = XTIO_SUCCESS;
    uint8_t rxbuff[UART_RX_READ_BUFFER_SIZE];
	uint32_t length = 0;
    
    monitor_task_t * monitor_task_handle;
    status = monitor_task_register(&monitor_task_handle, 1000, TASK_I2C_RX_STACK_SIZE);
    
    while (true)
    {
        monitor_task_alive(monitor_task_handle);
        
        length = UART_RX_READ_BUFFER_SIZE;
        status = xtio_i2c_receive(rxbuff, &length);
        if (length > 0)
        {
            uint32_t bytesRead = length;
            // Note that partial reads are perfectly fine here as remaining
            // bytes will be read in the next iteration
            parseData(xt_protocol_local, rxbuff, bytesRead); // Parse received data.
        }
        vTaskDelay(10UL / portTICK_PERIOD_MS);
    }
}
