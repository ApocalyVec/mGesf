/*
 * task_xethru.c
 *
 * Created: 09.05.2016 13.14.06
 *
 */ 

#include "task_xethru.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Atmel library includes. */
#include "freertos_usart_serial.h"

/* XeThru protocol implementation. */
#include "mcp_wrapper.h"
#include "xtid.h"

#include "arm_math.h"


#define TASK_XETHRU_STACK_SIZE						(3000)
#define TASK_XETHRU_PRIORITY					(tskIDLE_PRIORITY+0)
#define TASK_XETHRU_RX_STACK_SIZE						(2000)
#define TASK_XETHRU_RX_PRIORITY					(tskIDLE_PRIORITY+0)
#define TASK_XETHRU_TX_STACK_SIZE						(500)
#define TASK_XETHRU_TX_PRIORITY					(tskIDLE_PRIORITY+0)


/* The size of the buffer used to receive characters from the USART driver.
 * This equals the length of the longest string used in this file. */
#define XETHRU_RX_BUFFER_SIZE          (100)
#define XETHRU_TX_BUFFER_SIZE          (100)

/* The baud rate to use at startup. */
#define USART_BAUD_RATE         (115200)

/*
 * Tasks used to develop the USART drivers. 
 */
static void xethru_usart_tx_task(void *pvParameters);
static void xethru_usart_rx_task(void *pvParameters);
static void xethru_task(void *pvParameters);


/* The buffer provided to the USART driver to store incoming character in. */
static uint8_t receive_buffer[XETHRU_RX_BUFFER_SIZE+1]  __attribute__ ((aligned (4)));

mcp_wrapper_t* mcpw = NULL;
xSemaphoreHandle mcpw_sync_responce_semaphore;

static xQueueHandle xethru_usart_tx_queue;

void printf_float(float val);
void printf_float(float val)
{
	float fval = val;
	int pot = 0;
	float flim = 1000;
	while (fval>flim)
	{
		fval /= 10;
		pot++;
	}
	printf("%u.%02.2u", (int)fval, (int)((fval-(int)fval)*100.0f));
	if (pot>0)
	{
		printf("e%u", pot);	
	}
}


bool mcpw_send_bytes(void* mcpw_void); // Send bytes over serial port
bool mcpw_wait_for_response(uint32_t timeout); // Task synchronization, wait for response
void mcpw_response_ready(void); // Task synchronization, response ready
void mcpw_delay(uint32_t delay_ms); // System delay, given in ms.
void mcpw_on_host_parser_sleep(SleepData data, void * user_data); // Data parsed: Sleep message
void mcpw_on_host_parser_respiration(RespirationData data, void * user_data); // Data parsed: Respiration message
void mcpw_on_host_parser_respiration_moving_list(RespirationMovingListData data, void * user_data);  // Data parsed: Respiration MovingList message
void mcpw_on_host_parser_presence_single(PresenceSingleData data, void * user_data); // Data parsed: PresenceSingle message
void mcpw_on_host_parser_presence_moving_list(PresenceMovingListData data, void * user_data); // Data parsed: Presence MovingList message
void mcpw_on_host_parser_baseband_ap(BasebandApData data, void * user_data); // Data parsed: Baseband AP


bool mcpw_send_bytes(void *mcpw_void)
{
	mcp_wrapper_t* mcpw = (mcp_wrapper_t*)mcpw_void;
	for(int i=0;i<mcpw->tx_buf_index;i++)
	{
		if( xQueueSend( xethru_usart_tx_queue, mcpw->tx_buf+i, 10UL / portTICK_RATE_MS ) != pdPASS )
		{
			return false;
		}
	}
}

bool mcpw_wait_for_response(uint32_t timeout)
{
	if (xSemaphoreTake(mcpw_sync_responce_semaphore, timeout / portTICK_RATE_MS) == pdTRUE)
		return true;
	else
		return false;
}

void mcpw_response_ready(void)
{
	xSemaphoreGive(mcpw_sync_responce_semaphore);
}

void mcpw_delay(uint32_t delay_ms)
{
	vTaskDelay(delay_ms / portTICK_RATE_MS);
}

void mcpw_on_host_parser_sleep(SleepData sleep_data, void * user_data)
{
	printf("Sleep: Counter=%u", sleep_data.frame_counter);
	printf(", State=%u", sleep_data.sensor_state);
	printf(", RPM="); printf_float(sleep_data.respiration_rate);
	printf(", Distance="); printf_float(sleep_data.distance);
	printf(", SigQ=%u", sleep_data.signal_quality);
	printf(", MovementSlow="); printf_float(sleep_data.movement_slow);
	printf(", MovementFast="); printf_float(sleep_data.movement_fast);
	printf("\n");
}

void mcpw_on_host_parser_respiration(RespirationData respiration_data, void * user_data)
{
    printf("Respiration: Counter=%u", respiration_data.frame_counter);
    printf(", State=%u", respiration_data.sensor_state);
    printf(", RPM=%u", respiration_data.respiration_rate);
    printf(", Distance="); printf_float(respiration_data.distance);
    printf(", Movement (breathing pattern)="); printf_float(respiration_data.movement);
    printf(", SigQ=%u", respiration_data.signal_quality);
    printf("\n");
}

void mcpw_on_host_parser_respiration_moving_list(RespirationMovingListData data, void * user_data)
{
	float slow = 0.0f;
	float fast = 0.0f;
	float* movement_slow_items = (float*)pvPortMalloc(sizeof(float)*data.interval_count);
	float* movement_fast_items = (float*)pvPortMalloc(sizeof(float)*data.interval_count);
	memcpy(movement_slow_items, data.movement_slow_items, sizeof(float)*data.interval_count);
	memcpy(movement_fast_items, data.movement_fast_items, sizeof(float)*data.interval_count);
	if (data.interval_count > 0)
	{
		for (int i = 0; i < data.interval_count; i++)
		{
			slow += movement_slow_items[i];
			fast += movement_fast_items[i];
		}
		slow = slow / data.interval_count;
		fast = fast / data.interval_count;
	}
	vPortFree((void*)movement_slow_items);
	vPortFree((void*)movement_fast_items);

	printf("Respiration MovingList: Counter=%d", data.counter);
	printf(", IntervalCount=%d", data.interval_count);
	printf(", Slow="); printf_float(slow);
	printf(", Fast="); printf_float(fast);
	printf("\n");
}

void mcpw_on_host_parser_presence_single(PresenceSingleData presence_single_data, void * user_data)
{
	printf("Counter=%u", presence_single_data.frame_counter);
	printf(", State=");
	if (presence_single_data.presence_state == XTS_VAL_PRESENCE_PRESENCESTATE_PRESENCE)
		printf("Presence");
	else if (presence_single_data.presence_state == XTS_VAL_PRESENCE_PRESENCESTATE_NO_PRESENCE)
		printf("No presence");
	else
		printf("Initializing");
	if (presence_single_data.distance>0)
		printf(", Distance=%u.%02.2u", (int)presence_single_data.distance, (int)((presence_single_data.distance-(int)presence_single_data.distance)*100.0f));
	printf("\n");
}

void mcpw_on_host_parser_presence_moving_list(PresenceMovingListData data, void * user_data)
{
	float slow = 0.0f;
	float fast = 0.0f;
	float* movement_slow_items = (float*)pvPortMalloc(sizeof(float)*data.interval_count);
	float* movement_fast_items = (float*)pvPortMalloc(sizeof(float)*data.interval_count);
	memcpy(movement_slow_items, data.movement_slow_items, sizeof(float)*data.interval_count);
	memcpy(movement_fast_items, data.movement_fast_items, sizeof(float)*data.interval_count);
	if (data.interval_count > 0)
	{
		for (int i = 0; i < data.interval_count; i++)
		{
			slow += movement_slow_items[i];
			fast += movement_fast_items[i];
		}
		slow = slow / data.interval_count;
		fast = fast / data.interval_count;
	}
	vPortFree((void*)movement_slow_items);
	vPortFree((void*)movement_fast_items);

	printf("Presence MovingList: Counter=%d", data.frame_counter);
	printf(", IntervalCount=%d", data.interval_count);
	printf(", Slow="); printf_float(slow);
	printf(", Fast="); printf_float(fast);
	printf("\n");
}

void mcpw_on_host_parser_baseband_ap(BasebandApData data, void * user_data)
{
	printf("Baseband AP: Counter=%d", data.frame_counter);
	printf(", BinLength="); printf_float(data.bin_length);
	printf(", NumBins=%d", data.num_bins);
	printf(", Fc="); printf_float(data.carrier_frequency);
	printf(", Fs="); printf_float(data.sample_frequency);
	printf("\n");
}

void task_xethru_init(Usart *usart_base)
{
	freertos_usart_if freertos_usart;
	freertos_peripheral_options_t driver_options = {
		receive_buffer,								/* The buffer used internally by the USART driver to store incoming characters. */
		XETHRU_RX_BUFFER_SIZE,									/* The size of the buffer provided to the USART driver to store incoming characters. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,	/* The priority used by the USART interrupts. */
		USART_RS232,									/* Configure the USART for RS232 operation. */
		(USE_TX_ACCESS_MUTEX | USE_RX_ACCESS_MUTEX)
	};

	const sam_usart_opt_t usart_settings = {
		USART_BAUD_RATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		0 /* Only used in IrDA mode. */
	}; 

	/* Initialise the USART interface. */
	freertos_usart = freertos_usart_serial_init(usart_base,
			&usart_settings,
			&driver_options);
	configASSERT(freertos_usart);
	
	xethru_usart_tx_queue = xQueueCreate(100, sizeof(uint8_t));

	/* Create the tasks. */
	xTaskCreate(xethru_usart_tx_task, (const signed char *const) "XtTx",
			TASK_XETHRU_TX_STACK_SIZE, (void *) freertos_usart,
			TASK_XETHRU_TX_PRIORITY, NULL);
	xTaskCreate(xethru_usart_rx_task, (const signed char *const) "XtRx",
			TASK_XETHRU_RX_STACK_SIZE, (void *) freertos_usart,
			TASK_XETHRU_RX_PRIORITY + 1, NULL);
	xTaskCreate(xethru_task, (const signed char *const) "XtCtr",
			TASK_XETHRU_STACK_SIZE, (void *) NULL,
			TASK_XETHRU_PRIORITY + 1, NULL);
}

static void xethru_usart_tx_task(void *pvParameters)
{
	freertos_usart_if usart_port;
	static uint8_t local_buffer[XETHRU_TX_BUFFER_SIZE+1];
	const portTickType time_out_definition = (100UL / portTICK_RATE_MS),
			short_delay = (10UL / portTICK_RATE_MS);
	xSemaphoreHandle notification_semaphore;
	unsigned portBASE_TYPE string_index;
	status_code_t returned_status;

	/* The (already open) USART port is passed in as the task parameter. */
	usart_port = (freertos_usart_if)pvParameters;

	/* Create the semaphore to be used to get notified of end of
	transmissions. */
	vSemaphoreCreateBinary(notification_semaphore);
	configASSERT(notification_semaphore);

	/* Start with the semaphore in the expected state - no data has been sent
	yet.  A block time of zero is used as the semaphore is guaranteed to be
	there as it has only just been created. */
	xSemaphoreTake(notification_semaphore, 0);

	uint8_t data;
	for (;;) 
	{
		/* Wait for data. */
		int i=0;
		while ( xQueueReceive( xethru_usart_tx_queue, &data, 10UL / portTICK_RATE_MS) == pdTRUE)
        {
			local_buffer[i++] = data;
			if (i>=XETHRU_TX_BUFFER_SIZE) break;
        }		
		
		if (i>0)
		{
			// Start send. 
			returned_status = freertos_usart_write_packet_async(usart_port,
					local_buffer, 
					i,
					time_out_definition, notification_semaphore);
			// configASSERT(returned_status == STATUS_OK);

			/* The async version of the write function is being used, so wait for
			the end of the transmission.  No CPU time is used while waiting for the
			semaphore.*/
			xSemaphoreTake(notification_semaphore, time_out_definition * 2);
			
		}

		vTaskDelay(100UL / portTICK_RATE_MS);
	}
}

static void xethru_usart_rx_task(void *pvParameters)
{
	freertos_usart_if usart_port;
	static uint8_t rx_buffer[XETHRU_RX_BUFFER_SIZE+1];
	uint32_t received = 1;
	unsigned portBASE_TYPE string_index;

	/* The (already open) USART port is passed in as the task parameter. */
	usart_port = (freertos_usart_if)pvParameters;

	string_index = 0;

	for (;;) 
	{
		received = freertos_usart_serial_read_packet(usart_port, rx_buffer, XETHRU_RX_BUFFER_SIZE, received == 0 ? 100UL / portTICK_RATE_MS : 10UL / portTICK_RATE_MS);
		if (received == 0) continue;

		mcpw_process_rx_data(mcpw, rx_buffer, received);
	}
}



int mcpw_demo_x2m200();
int mcpw_demo_x2m200()
{
	printf("Starting mcpw_demo_x2m200.\n");
	
    mcpw->mcp_host_parser->sleep = mcpw_on_host_parser_sleep; // X2M200 sleep message
    mcpw->mcp_host_parser->respiration = mcpw_on_host_parser_respiration; // X2M200 respiration message

    int res = 0;
    mcpw_module_reset(mcpw);
    vTaskDelay(1000UL / portTICK_RATE_MS);
    res |= mcpw_load_profile(mcpw, XTS_ID_APP_SLEEP);
    res |= mcpw_set_detection_zone(mcpw, 0.4, 2.0);
    res |= mcpw_set_sensitivity(mcpw, 9);
    res |= mcpw_set_led_control(mcpw, 2, 100);
    res |= mcpw_set_sensor_mode(mcpw, XTS_SM_RUN, 0);

	return 0;	
}


int mcpw_demo_x4m300();
int mcpw_demo_x4m300()
{
	printf("Starting mcpw_demo_x4m300.\n");

	mcpw->mcp_host_parser->presence_single = mcpw_on_host_parser_presence_single; // X4M300 presence single message
	mcpw->mcp_host_parser->presence_movinglist = mcpw_on_host_parser_presence_moving_list; // X4M300 presence movinglist message
	mcpw->mcp_host_parser->baseband_ap = mcpw_on_host_parser_baseband_ap; // X4M300 baseband AP message

	// X4M300 Presence profile
	int res = 0;
	// First stop any running profile and change baudrate.
#if 1
	// Assume 115kbps, stop and change baudrate. If already 921kbps, these will fail, but the next stop will work.
	mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	mcpw_set_baudrate(mcpw, XTID_BAUDRATE_921600);
	usart_set_async_baudrate(USART3, XTID_BAUDRATE_921600, sysclk_get_cpu_hz());
#endif
	mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	
	char system_info[50];
	res = mcpw_get_systeminfo(mcpw, XTID_SSIC_ORDERCODE, system_info, sizeof(system_info));
	printf("Module ordercode: %s\n", system_info);
	res = mcpw_get_systeminfo(mcpw, XTID_SSIC_VERSION, system_info, sizeof(system_info));
	printf("Module version: %s\n", system_info);
	
	if (MCPW_OK != mcpw_load_profile(mcpw, XTS_ID_APP_PRESENCE_2)) printf("mcpw_load_profile failed.\n");
	if (MCPW_OK != mcpw_set_noisemap_control(mcpw, XTID_NOISEMAP_CONTROL_ENABLE | XTID_NOISEMAP_CONTROL_ADAPTIVE)) printf("mcpw_set_noisemap_control failed.\n");
	if (MCPW_OK != mcpw_set_detection_zone(mcpw, 1.0, 3.0)) printf("mcpw_set_detection_zone failed.\n");
	if (MCPW_OK != mcpw_set_sensitivity(mcpw, 9)) printf("mcpw_set_sensitivity failed.\n");
	if (MCPW_OK != mcpw_set_led_control(mcpw, XTID_LED_MODE_FULL, 100)) printf("mcpw_set_led_control failed.\n");
	// Methods to turn on or off module data messages. Select _ENABLE or _DISABLE.
	// Remember: Data throughput limitations may prevent you from outputting multiple messages at the same time, in particular with long DetectionZone.
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_PRESENCE_SINGLE, XTID_OUTPUT_CONTROL_ENABLE)) printf("mcpw_set_output_control(XTS_ID_PRESENCE_SINGLE) failed.\n");
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_PRESENCE_MOVINGLIST, XTID_OUTPUT_CONTROL_DISABLE)) printf("mcpw_set_output_control(XTS_ID_PRESENCE_MOVINGLIST) failed.\n");
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_BASEBAND_AMPLITUDE_PHASE, XTID_OUTPUT_CONTROL_DISABLE)) printf("mcpw_set_output_control(XTS_ID_BASEBAND_AMPLITUDE_PHASE) failed.\n");
	// Start module execution.
	if (MCPW_OK != mcpw_set_sensor_mode(mcpw, XTS_SM_RUN, 0)) printf("mcpw_set_sensor_mode failed.\n");

	// Wait indefinately for readThread to finish.
	for (;;)
	{
		// Every 3 minutes, store noisemap.
		vTaskDelay(3*60*1000UL / portTICK_RATE_MS);
		res = mcpw_store_noisemap(mcpw);
		printf("Store noisemap %s.\n", (res == MCPW_OK ? "succeeded" : "failed"));
	}

	return 0;
}

int mcpw_demo_x4m200();
int mcpw_demo_x4m200()
{
	printf("Starting mcpw_demo_x4m200.\n");

	mcpw->mcp_host_parser->sleep = mcpw_on_host_parser_sleep; // X4M200 sleep message
	mcpw->mcp_host_parser->respiration = mcpw_on_host_parser_respiration; // X4M200 legacy respiration message (original X2M200 resp message)
	mcpw->mcp_host_parser->respiration_movinglist = mcpw_on_host_parser_respiration_moving_list; // X4M200 movinglist message
	mcpw->mcp_host_parser->baseband_ap = mcpw_on_host_parser_baseband_ap; // X4M200 baseband AP message

	// X4M200 Respiration profile
	int res = 0;
	// First stop any running profile and change baudrate.
#if 1
	// Assume 115kbps, stop and change baudrate. If already 921kbps, these will fail, but the next stop will work.
	mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	mcpw_set_baudrate(mcpw, XTID_BAUDRATE_921600);
	usart_set_async_baudrate(USART3, XTID_BAUDRATE_921600, sysclk_get_cpu_hz());
#endif
	mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	
	char system_info[50];
	res = mcpw_get_systeminfo(mcpw, XTID_SSIC_ORDERCODE, system_info, sizeof(system_info));
	printf("Module ordercode: %s\n", system_info);
	res = mcpw_get_systeminfo(mcpw, XTID_SSIC_VERSION, system_info, sizeof(system_info));
	printf("Module version: %s\n", system_info);
	
	if (MCPW_OK != mcpw_load_profile(mcpw, XTS_ID_APP_RESPIRATION_2)) printf("mcpw_load_profile failed.\n"); // XTS_ID_APP_RESPIRATION_2 = Adult, XTS_ID_APP_RESPIRATION_3 = Baby.
	if (MCPW_OK != mcpw_set_noisemap_control(mcpw, XTID_NOISEMAP_CONTROL_ENABLE | XTID_NOISEMAP_CONTROL_ADAPTIVE)) printf("mcpw_set_noisemap_control failed.\n");
	if (MCPW_OK != mcpw_set_detection_zone(mcpw, 1.0, 3.0)) printf("mcpw_set_detection_zone failed.\n");
	if (MCPW_OK != mcpw_set_sensitivity(mcpw, 9)) printf("mcpw_set_sensitivity failed.\n");
	if (MCPW_OK != mcpw_set_led_control(mcpw, XTID_LED_MODE_FULL, 100)) printf("mcpw_set_led_control failed.\n");
	// Methods to turn on or off module data messages. Select _ENABLE or _DISABLE.
	// Remember: Data throughput limitations may prevent you from outputting multiple messages at the same time, in particular with long DetectionZone.
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_SLEEP_STATUS, XTID_OUTPUT_CONTROL_ENABLE)) printf("mcpw_set_output_control(XTS_ID_SLEEP_STATUS) failed.\n");
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_RESP_STATUS, XTID_OUTPUT_CONTROL_DISABLE)) printf("mcpw_set_output_control(XTS_ID_RESP_STATUS) failed.\n");
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_RESPIRATION_MOVINGLIST, XTID_OUTPUT_CONTROL_DISABLE)) printf("mcpw_set_output_control(XTS_ID_RESPIRATION_MOVINGLIST) failed.\n");
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_BASEBAND_AMPLITUDE_PHASE, XTID_OUTPUT_CONTROL_DISABLE)) printf("mcpw_set_output_control(XTS_ID_BASEBAND_AMPLITUDE_PHASE) failed.\n");
	// Start module execution.
	if (MCPW_OK != mcpw_set_sensor_mode(mcpw, XTS_SM_RUN, 0)) printf("mcpw_set_sensor_mode failed.\n");

	// Wait indefinately for readThread to finish.
	for (;;)
	{
		// Every 3 minutes, store noisemap.
		vTaskDelay(3*60*1000UL / portTICK_RATE_MS);
		res = mcpw_store_noisemap(mcpw);
		printf("Store noisemap %s.\n", (res == MCPW_OK ? "succeeded" : "failed"));
	}

	return 0;
}


static void xethru_task(void *pvParameters)
{

    printf("Connecting to XeThru module.\n");
    printf("Starting serial port read thread.\n");
	

	vSemaphoreCreateBinary(mcpw_sync_responce_semaphore);
	xSemaphoreTake(mcpw_sync_responce_semaphore, 0);

	unsigned char mcpw_instance_memory[mcpw_get_instance_size()];
	mcpw = mcpw_init(mcpw_instance_memory);
	mcpw->send_bytes = mcpw_send_bytes;
	mcpw->wait_for_response = mcpw_wait_for_response;
	mcpw->response_ready = mcpw_response_ready;
	
	// Choose demo method
	//mcpw_demo_x2m200();
	//mcpw_demo_x4m300();
	mcpw_demo_x4m200();

	for (;;)
	{
		vTaskDelay(1000UL / portTICK_RATE_MS);
	}

}

