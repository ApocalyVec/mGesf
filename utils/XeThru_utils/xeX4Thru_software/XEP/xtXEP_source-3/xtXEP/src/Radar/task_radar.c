/**
 * @file
 *
 *
 */

#include "task_radar.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include "x4driver.h"
#include "xtcompiler.h"
#include "radar_hal.h"
#include "xep_dispatch_messages.h"
#include "arm_math.h"
#include "xtmemory.h"
#include "task_monitor.h"
#include "xep_hal.h"
#include "xtserial_definitions.h"
#include "xt_system.h"

#define TASK_RADAR_STACK_SIZE            (300)
#define TASK_RADAR_PRIORITY        (tskIDLE_PRIORITY + 6)

#define XEP_NOTIFY_RADAR_DATAREADY		0x0001
#define XEP_NOTIFY_RADAR_TRIGGER_SWEEP	0x0002
#define XEP_NOTIFY_X4DRIVER_ACTION		0x0004

volatile xtx4driver_errors_t x4_initialize_status = XEP_ERROR_X4DRIVER_UNINITIALIZED;

typedef struct
{
	TaskHandle_t radar_task_handle;
	void* radar_handle;				// Some info separating different radar chips on the same module.
} XepRadarX4DriverUserReference_t;

static void task_radar(void *pvParameters);
static int radar_certification_handler(X4Driver_t* x4driver);
static uint32_t read_and_send_radar_frame(X4Driver_t* x4driver, XepDispatch_t* dispatch);
uint32_t x4driver_timer_set_timer_timeout_frequency(void* timer , uint32_t fps);
void x4driver_timer_sweep_timeout(TimerHandle_t pxTimer);
void x4driver_timer_action_timeout(TimerHandle_t pxTimer);
void x4driver_notify_data_ready(void* user_reference);
void x4driver_X4_interrupt_notify_data_ready(void);
void x4driver_enable_ISR(void* user_reference,uint32_t enable);
uint32_t x4driver_trigger_sweep_pin(void* user_reference);
int _x4driver_test_io_connectivity(X4Driver_t* x4driver);

static TaskHandle_t h_task_radar = NULL;

static uint32_t x4driver_callback_take_sem(void * sem,uint32_t timeout)
{
    return xSemaphoreTakeRecursive((SemaphoreHandle_t)sem, timeout);
}
static void x4driver_callback_give_sem(void * sem)
{
    xSemaphoreGiveRecursive((SemaphoreHandle_t)sem);
}

static uint32_t isr_enabled = 0;
static uint32_t x4driver_callback_pin_set_enable(void* user_reference, uint8_t value)
{
	XepRadarX4DriverUserReference_t* x4driver_user_reference = user_reference;
    
    uint32_t isr_was_enabled = isr_enabled;
    if (isr_was_enabled)
	    x4driver_enable_ISR(NULL,0);
	int status = radar_hal_pin_set_enable(x4driver_user_reference->radar_handle, value);
	if (isr_was_enabled)
        x4driver_enable_ISR(NULL,1);
	return status;
}
static uint32_t x4driver_callback_spi_write(void* user_reference, uint8_t* data, uint32_t length)
{
	XepRadarX4DriverUserReference_t* x4driver_user_reference = user_reference;
	return radar_hal_spi_write(x4driver_user_reference->radar_handle, data, length);
}
static uint32_t x4driver_callback_spi_read(void* user_reference, uint8_t* data, uint32_t length)
{
	XepRadarX4DriverUserReference_t* x4driver_user_reference = user_reference;
	return radar_hal_spi_read(x4driver_user_reference->radar_handle, data, length);
}

static uint32_t x4driver_callback_spi_write_read(void* user_reference, uint8_t* wdata, uint32_t wlength, uint8_t* rdata, uint32_t rlength)
{
	XepRadarX4DriverUserReference_t* x4driver_user_reference = user_reference;
	return radar_hal_spi_write_read(x4driver_user_reference->radar_handle, wdata, wlength, rdata, rlength);
}


/************************************************************************/
/*   Simple dummy                                                       */
/************************************************************************/
static void x4driver_callback_wait_us(uint32_t us)
{
	xt_delay_us(us);
}
typedef struct
{
	XepDispatch_t* dispatch;
	X4Driver_t* x4driver;
} RadarTaskParameters_t;


void x4driver_timer_sweep_timeout(TimerHandle_t pxTimer)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xTaskNotifyFromISR(h_task_radar, XEP_NOTIFY_RADAR_TRIGGER_SWEEP, eSetBits, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


uint32_t x4driver_timer_set_timer_timeout_frequency(void* timer , uint32_t frequency)
{
	uint32_t status = XEP_ERROR_X4DRIVER_OK;
	uint32_t timer_ticks =  1000 / frequency / portTICK_PERIOD_MS;
	X4DriverTimer_t * x4drivertimer = (X4DriverTimer_t*)timer;
	x4drivertimer->configured_frequency = 1000.0 /timer_ticks;
	if(frequency == 0)
	{
		xTimerStop(x4drivertimer->object, 100 / portTICK_PERIOD_MS);
	}
	else
	{
		if (pdFAIL == xTimerChangePeriod(x4drivertimer->object, timer_ticks, 100 / portTICK_PERIOD_MS))
		{
			status = XEP_ERROR_X4DRIVER_TIMER_PERIOD_CHANGE_FAIL;
		}
	}
	return status;
}


void x4driver_notify_data_ready(void* user_reference)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xTaskNotifyFromISR(h_task_radar, XEP_NOTIFY_RADAR_DATAREADY, eSetBits, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void x4driver_timer_action_timeout(TimerHandle_t pxTimer)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xTaskNotifyFromISR(h_task_radar, XEP_NOTIFY_X4DRIVER_ACTION, eSetBits, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void x4driver_X4_interrupt_notify_data_ready(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xTaskNotifyFromISR(h_task_radar, XEP_NOTIFY_RADAR_DATAREADY, eSetBits, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


uint32_t x4driver_trigger_sweep_pin(void* user_reference)
{
	xtio_set_level(XTIO_X4_IO2,XTIO_PIN_LEVEL_HIGH);
	xtio_set_level(XTIO_X4_IO2,XTIO_PIN_LEVEL_LOW);
	return XEP_ERROR_X4DRIVER_OK;
}

void x4driver_enable_ISR(void* user_reference,uint32_t enable)
{
	if(enable == 1)
		xtio_irq_register_callback(XTIO_X4_IO1, x4driver_X4_interrupt_notify_data_ready, XTIO_INTERRUPT_RISING_EDGE);
	else
		xtio_irq_unregister_callback(XTIO_X4_IO1);

    isr_enabled = enable;

	xtio_set_pin_mode(XTIO_X4_IO2,XTIO_GPIO_MODE);//setup gpio mode
    xtio_set_direction(XTIO_X4_IO2,XTIO_OUTPUT,XTIO_PIN_LEVEL_LOW);//set output mode. level high
    //return 0;
}

uint32_t task_radar_init(X4Driver_t** x4driver, XepDispatch_t* dispatch)
{
    XepRadarX4DriverUserReference_t* x4driver_user_reference = (XepRadarX4DriverUserReference_t*)xtmemory_malloc_default(sizeof(XepRadarX4DriverUserReference_t));
    memset(x4driver_user_reference, 0, sizeof(XepRadarX4DriverUserReference_t));

    void * radar_hal_memory = xtmemory_malloc_default(radar_hal_get_instance_size());
    int status = radar_hal_init((void*)&(x4driver_user_reference->radar_handle), radar_hal_memory);
    (void)status;


//! [X4Driver Platform Dependencies]

    // X4Driver lock mechanism, including methods for locking and unlocking.
    X4DriverLock_t lock;
    lock.object = (void*)xSemaphoreCreateRecursiveMutex();
    lock.lock = x4driver_callback_take_sem;
    lock.unlock = x4driver_callback_give_sem;

    // X4Driver timer for generating sweep FPS on MCU. Not used when sweep FPS is generated on X4.
    uint32_t timer_id_sweep = 2;
    X4DriverTimer_t timer_sweep;
    timer_sweep.object = xTimerCreate("X4Driver_sweep_timer", 1000 / portTICK_PERIOD_MS, pdTRUE, (void*)timer_id_sweep, x4driver_timer_sweep_timeout);
    timer_sweep.configure = x4driver_timer_set_timer_timeout_frequency;

    // X4Driver timer used for driver action timeout.
    uint32_t timer_id_action = 3;
	X4DriverTimer_t timer_action;
    timer_action.object = xTimerCreate("X4Driver_action_timer", 1000 / portTICK_PERIOD_MS, pdTRUE, (void*)timer_id_action, x4driver_timer_action_timeout);
	timer_action.configure = x4driver_timer_set_timer_timeout_frequency;

    // X4Driver callback methods.
	X4DriverCallbacks_t x4driver_callbacks;
    x4driver_callbacks.pin_set_enable = x4driver_callback_pin_set_enable;   // X4 ENABLE pin
    x4driver_callbacks.spi_read = x4driver_callback_spi_read;               // SPI read method
    x4driver_callbacks.spi_write = x4driver_callback_spi_write;             // SPI write method
    x4driver_callbacks.spi_write_read = x4driver_callback_spi_write_read;   // SPI write and read method
    x4driver_callbacks.wait_us = x4driver_callback_wait_us;                 // Delay method
    x4driver_callbacks.notify_data_ready = x4driver_notify_data_ready;      // Notification when radar data is ready to read
    x4driver_callbacks.trigger_sweep = x4driver_trigger_sweep_pin;          // Method to set X4 sweep trigger pin
    x4driver_callbacks.enable_data_ready_isr = x4driver_enable_ISR;         // Control data ready notification ISR

//! [X4Driver Platform Dependencies]

    void* x4driver_instance_memory = xtmemory_malloc_default(x4driver_get_instance_size());//pvPortMalloc(x4driver_get_instance_size());
    x4driver_create(x4driver, x4driver_instance_memory, &x4driver_callbacks,&lock,&timer_sweep,&timer_action, (void*)x4driver_user_reference);

    RadarTaskParameters_t* task_parameters = (RadarTaskParameters_t*)xtmemory_malloc_default(sizeof(RadarTaskParameters_t));
	task_parameters->dispatch = dispatch;
	task_parameters->x4driver = *x4driver;
	
	task_parameters->x4driver->spi_buffer_size = 1536*4 + 32;
	task_parameters->x4driver->spi_buffer = xtmemory_malloc_default(task_parameters->x4driver->spi_buffer_size);
	if ((((uint32_t)task_parameters->x4driver->spi_buffer) % 32) != 0)
	{
		int alignment_diff = 32 - (((uint32_t)task_parameters->x4driver->spi_buffer) % 32);
		task_parameters->x4driver->spi_buffer += alignment_diff;
		task_parameters->x4driver->spi_buffer_size -= alignment_diff;
	}
	task_parameters->x4driver->spi_buffer_size -= task_parameters->x4driver->spi_buffer_size % 32;

	xTaskCreate(task_radar, (const char * const) "Radar", TASK_RADAR_STACK_SIZE, (void*)task_parameters, TASK_RADAR_PRIORITY, &h_task_radar);
	x4driver_user_reference->radar_task_handle = h_task_radar;

	return 0;
}


int _x4driver_test_io_connectivity(X4Driver_t* x4driver)
{
	uint32_t status = XEP_ERROR_X4DRIVER_OK;

	int x4_io_pin_count;
	const int* x4_io_pins;
	const int* mcu_io_pins;
	xt_selftest_x4_connectivity_get_pin_table(&x4_io_pin_count, &x4_io_pins, &mcu_io_pins);

	uint8_t x4_pin_mask = 0;
	for (int i = 0; i < x4_io_pin_count; i++)
	x4_pin_mask |= 1<<x4_io_pins[i];

	for (int i = 0; i < x4_io_pin_count; i++)
	xtio_set_direction(mcu_io_pins[i], XTIO_INPUT, XTIO_PIN_LEVEL_LOW);

	uint8_t ctrl_val, oe_val, out_val;
	x4driver_get_pif_register(x4driver, ADDR_PIF_IO_CTRL_3_RW, &ctrl_val);
	x4driver_get_pif_register(x4driver, ADDR_PIF_GPIO_OE_RW, &oe_val);
	x4driver_get_pif_register(x4driver, ADDR_PIF_GPIO_OUT_RW, &out_val);
	
	x4driver_set_pif_register(x4driver, ADDR_PIF_IO_CTRL_3_RW, ctrl_val | x4_pin_mask);
	x4driver_set_pif_register(x4driver, ADDR_PIF_GPIO_OE_RW, oe_val | x4_pin_mask);
	
	// Test travelling 1 pattern
	for (int i = 0; i < x4_io_pin_count; i++)
	{
		x4driver_set_pif_register(x4driver, ADDR_PIF_GPIO_OUT_RW, (out_val&(~x4_pin_mask)) | (1<<x4_io_pins[i]));
		
		// Assure that value has time to propagate
		for (volatile int k = 0; k < 100; k++);

		for (int j = 0; j < x4_io_pin_count; j++)
		{
			xtio_pin_level_t level;
			xtio_get_level(mcu_io_pins[j], &level);
			if (((j == i) && (!level)) || // Pin that should have been high was low
			((j != i) && (level))) // Pin that should have been low was high
			{
				xt_selftest_x4_connectivity_mark_faulty_pin(j);
				status = XEP_ERROR_X4DRIVER_NOK;
			}
		}
	}
	
	// Set X4 IO pins as they were
	x4driver_set_pif_register(x4driver, ADDR_PIF_GPIO_OUT_RW, out_val);
	x4driver_set_pif_register(x4driver, ADDR_PIF_GPIO_OE_RW, oe_val);
	x4driver_set_pif_register(x4driver, ADDR_PIF_IO_CTRL_3_RW, ctrl_val);

	return status;
}


int cert_mode = XTS_SM_REG_IDLE;
static void task_radar(void *pvParameters)
{
	RadarTaskParameters_t* task_parameters = (RadarTaskParameters_t*)pvParameters;
	XepDispatch_t* dispatch = (XepDispatch_t*)task_parameters->dispatch;
    X4Driver_t* x4driver = (X4Driver_t*)task_parameters->x4driver;
	xtmemory_free_default((void*)task_parameters);
	task_parameters = NULL;
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
	#pragma GCC diagnostic push
	int status = 0;
	#pragma GCC diagnostic pop

    monitor_task_t * monitor_task_handle;
    status = monitor_task_register(&monitor_task_handle, 1000, TASK_RADAR_STACK_SIZE);

	status = x4driver_set_enable(x4driver, 1);
    xt_delay_us(100);
    
    // Get default configuration
    xt_radar_t default_config;
    xt_get_default_radar_config(&default_config);
    x4driver->external_load_caps = default_config.external_load_caps;

	xtx4driver_errors_t tmp_status =  x4driver_init(x4driver);
	xtx4driver_errors_t io_tmp_status = XEP_ERROR_X4DRIVER_UNINITIALIZED;
	if (xt_get_operation_mode() == XT_OPMODE_FACTORY)
	{
		// Perform pin test pattern
		io_tmp_status = _x4driver_test_io_connectivity(x4driver);

		if (io_tmp_status != XEP_ERROR_X4DRIVER_OK)
			tmp_status = io_tmp_status;// will mask other errors but, previous solution would do this as well. 
		
		x4driver_init(x4driver);
	}
	x4_initialize_status = tmp_status;
	x4driver_check_configuration(x4driver);
	x4driver_set_sweep_trigger_control(x4driver,SWEEP_TRIGGER_X4); // By default let sweep trigger control done by X4

    // Set defaults
    x4driver_set_frame_area_offset(x4driver, default_config.frame_area_offset);
    x4driver_set_tx_power(x4driver, default_config.tx_power);
    x4driver_set_prf_div(x4driver, default_config.prf_div);
    x4driver_set_tx_center_frequency(x4driver, default_config.center_frequency);

	uint32_t notify_value;
	for (;;)
	{
        // TODO: Check if radar communication is OK
        status = monitor_task_alive(monitor_task_handle);

		if (xt_get_operation_mode() == XT_OPMODE_CERTIFICATION)
		{
			cert_mode = radar_certification_handler(x4driver);
		}

		xTaskNotifyWait( 0x00,      /* Don't clear any notification bits on entry. */
                         0xffffffff, /* Reset the notification value to 0 on exit. */
                         &notify_value, /* Notified value pass out. */
                         500 / portTICK_PERIOD_MS );  /* Block indefinitely. */

		if (notify_value & XEP_NOTIFY_RADAR_DATAREADY)
		{
			// Radar data ready
			if(x4driver->trigger_mode != SWEEP_TRIGGER_MANUAL)
				read_and_send_radar_frame(x4driver, dispatch);
		}
		else if(notify_value & XEP_NOTIFY_RADAR_TRIGGER_SWEEP)
		{
			x4driver_start_sweep(x4driver);
		}

		else if(notify_value & XEP_NOTIFY_X4DRIVER_ACTION)
		{
			x4driver_on_action_event(x4driver);
		}
        else if (notify_value == 0) // Timeout
		{

		}
	}
}

void radar_certification_enter_tx_only(X4Driver_t* x4driver)
{
    xt_radar_t default_config;
    xt_get_default_radar_config(&default_config);

    x4driver_setup_default(x4driver);

    x4driver_set_tx_power(x4driver, default_config.tx_power);

    // * trx_send_every_pulse = 1 (Enable transmitter always on), trx_ctrl_mode, 3
    // * rx_strobe_enable = 0 (Disable receiver), trx_ctrl_mode, 6
    uint8_t value;
    x4driver_get_pif_register(x4driver, ADDR_PIF_TRX_CTRL_MODE_RW, &value);
        value |= (1<<3);
        value &= ~(1<<6);
    x4driver_set_pif_register(x4driver, ADDR_PIF_TRX_CTRL_MODE_RW, value);

    // Enter sleep mode to shut off clocks
    xt_deep_sleep();

    while(1); // Should never get here
}

void radar_certification_enter_tx_only_tx_off(X4Driver_t* x4driver)
{
    x4driver_setup_default(x4driver);

    x4driver_set_tx_power(x4driver, TX_POWER_OFF);

    // * trx_send_every_pulse = 1 (Enable transmitter always on), trx_ctrl_mode, 3
    // * rx_strobe_enable = 0 (Disable receiver), trx_ctrl_mode, 6
    uint8_t value;
    x4driver_get_pif_register(x4driver, ADDR_PIF_TRX_CTRL_MODE_RW, &value);
        value |= (1<<3);
        value &= ~(1<<6);
    x4driver_set_pif_register(x4driver, ADDR_PIF_TRX_CTRL_MODE_RW, value);
                    
    // Enter sleep mode to shut off clocks
    xt_deep_sleep();

    while(1); // Should never get here
}

static int radar_certification_handler(X4Driver_t* x4driver)
{
	static sensor_mode_reg_submode_t prev_submode = XTS_SM_REG_IDLE;

    sensor_mode_reg_submode_t submode = (sensor_mode_reg_submode_t) xt_get_certification_submode();

	if (submode != prev_submode)
	{
		prev_submode = submode;
		xt_radar_t default_config;
		switch(submode)
		{
			case XTS_SM_REG_CERT_TX_ONLY:
				radar_certification_enter_tx_only(x4driver);
				
				break;
			case XTS_SM_REG_CERT_TX_ONLY_TX_OFF:
				radar_certification_enter_tx_only_tx_off(x4driver);

				break;
			case XTS_SM_REG_CERT_STREAM_TX_OFF:
				// Start with radar in default setup
				x4driver_setup_default(x4driver);
				// * tx_power = 0
				x4driver_set_tx_power(x4driver, TX_POWER_OFF);
				// * Stream frames
				x4driver_set_fps(x4driver, 20);
				break;
			case XTS_SM_REG_CERT_STREAM:
				// Start with radar in default setup
				x4driver_setup_default(x4driver);
                
                xt_get_default_radar_config(&default_config);
                x4driver_set_tx_power(x4driver, default_config.tx_power);
				// * Stream frames
				x4driver_set_fps(x4driver, 20);
				break;
			case XTS_SM_REG_CERT_RX_FAILURE_EMU:
				//Start with radar in default setup
				x4driver_setup_default(x4driver);
                
                xt_get_default_radar_config(&default_config);
                x4driver_set_tx_power(x4driver, default_config.tx_power);
                x4driver_set_frame_area_offset(x4driver, 0);

				x4driver_set_fps(x4driver, 20);
				
				// * sampler_preset_en = 1
				// * sampler_preset_val_lsb = 0
				// * sampler_preset_val_msb = 0
				x4driver_set_xif_register(x4driver, ADDR_XIF_SAMPLER_PRESET_LSB_RW, 0);
				x4driver_set_xif_register(x4driver, ADDR_XIF_SAMPLER_PRESET_MSB_RW, (1<<7));
				
				break;
			case XTS_SM_REG_CERT_TX_ONLY_EXTCLK:
			case XTS_SM_REG_CERT_TX_ONLY_CLKOUT:
			case XTS_SM_REG_IDLE:
			default:
				break;
		}
	}

	return submode;
}

static uint32_t read_and_send_radar_frame(X4Driver_t* x4driver, XepDispatch_t* dispatch)
{
	uint32_t status;

    XepDispatchMessageContentRadardataFramePacket_t* frame_packet;
	MemoryBlock_t* memoryblock;

	uint32_t bin_count = 0;
	x4driver_get_frame_bin_count(x4driver,&bin_count);
	uint8_t down_conversion_enabled = 0;
	x4driver_get_downconversion(x4driver,&down_conversion_enabled);

	uint32_t fdata_count = bin_count;
	if(down_conversion_enabled == 1)
	{
		fdata_count = bin_count * 2;
	}

    status = dispatch_message_radardata_prepare_frame(&frame_packet, &memoryblock, dispatch, fdata_count);
	if (status != XEP_ERROR_OK)
	{
        // Read frame and ignore
        uint32_t frame_counter;
	    x4driver_read_frame_normalized(x4driver, &frame_counter, NULL, bin_count);
		
		// Check for failure and shut down transmitter
		if (x4driver->zero_frame_counter >= X4DRIVER_MAX_ALLOWED_ZERO_FRAMES)
		{
			x4driver->zero_frame_counter = 0;
			x4driver_set_tx_power(x4driver, TX_POWER_OFF);
			x4driver_set_fps(x4driver, 0);
		}
		
		return status;
	}

    // Read radar data into dispatch memory.
	status = x4driver_read_frame_normalized(x4driver, &frame_packet->framecounter, frame_packet->framedata, frame_packet->bin_count);

	if (status != XEP_ERROR_OK)
	{
		memorypool_free(memoryblock);
		return status;
	}

	// Check for failure and shut down transmitter
	if (x4driver->zero_frame_counter >= X4DRIVER_MAX_ALLOWED_ZERO_FRAMES)
	{
		x4driver->zero_frame_counter = 0;
		x4driver_set_tx_power(x4driver, TX_POWER_OFF);
		x4driver_set_fps(x4driver, 0);
	}

	if ((cert_mode == XTS_SM_REG_CERT_STREAM) || (cert_mode == XTS_SM_REG_CERT_STREAM_TX_OFF) || (cert_mode == XTS_SM_REG_CERT_RX_FAILURE_EMU))
	{
		memorypool_free(memoryblock);
		return status;
	}
    status = dispatch_send_message(NULL, dispatch, XEP_DISPATCH_MESSAGETAG_RADAR_DATA, memoryblock, frame_packet->common.message_size);

	return status;
}
