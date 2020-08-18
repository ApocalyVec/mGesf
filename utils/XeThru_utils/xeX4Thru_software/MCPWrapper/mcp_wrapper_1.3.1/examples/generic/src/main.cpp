#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string.h>
#include "ModuleIoFactory.hpp"

#include "mcp_wrapper.h"
#include "../examples/res/X4M300_1_2_1_bin.h"

using namespace std;

std::mutex moduleIoMutex;

std::mutex mcpw_sync_response_mutex;
std::condition_variable mcpw_sync_response_condition;

//! [MCPW Platform Methods]
bool mcpw_send_bytes(void *mcpw_void);																					  // Send bytes over serial port
bool mcpw_wait_for_response(uint32_t timeout);																			  // Task synchronization, wait for response
void mcpw_response_ready(void);																							  // Task synchronization, response ready
void mcpw_delay(uint32_t delay_ms);																						  // System delay, given in ms.
void mcpw_on_host_parser_sleep(SleepData data, void *user_data);														  // Data parsed: Sleep message
void mcpw_on_host_parser_respiration(RespirationData data, void *user_data);											  // Data parsed: Respiration message
void mcpw_on_host_parser_respiration_moving_list(RespirationMovingListData data, void *user_data);						  // Data parsed: Respiration MovingList message
void mcpw_on_host_parser_respiration_normalized_moving_list(RespirationNormalizedMovementListData data, void *user_data); // Data parsed: Respiration Normalized MovingList message
void mcpw_on_host_parser_presence_single(PresenceSingleData data, void *user_data);										  // Data parsed: PresenceSingle message
void mcpw_on_host_parser_presence_moving_list(PresenceMovingListData data, void *user_data);							  // Data parsed: Presence MovingList message
void mcpw_on_host_parser_baseband_ap(BasebandApData data, void *user_data);												  // Data parsed: Baseband AP

void mcpw_on_host_parser_data_float(FloatData data, void *user_data); // Data parsed: Float array
//! [MCPW Platform Methods]

uint32_t global_test_results = 0;

bool mcpw_send_bytes(void *mcpw_void)
{
	mcp_wrapper_t *mcpw = (mcp_wrapper_t *)mcpw_void;
	ModuleIo *moduleIo = (ModuleIo *)mcpw->user_reference;
	moduleIoMutex.lock();
	moduleIo->write(mcpw->tx_buf, mcpw->tx_buf_index);
	moduleIoMutex.unlock();
	return true;
}

bool mcpw_wait_for_response(uint32_t timeout)
{
	std::unique_lock<std::mutex> lock(mcpw_sync_response_mutex);
	bool res = (mcpw_sync_response_condition.wait_for(lock, std::chrono::milliseconds(timeout)) != std::cv_status::timeout);
	return res;
}

void mcpw_response_ready(void)
{
	mcpw_sync_response_condition.notify_one();
}

void mcpw_delay(uint32_t delay_ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
}

void mcpw_on_host_parser_respiration(RespirationData data, void *user_data)
{
	cout << "Respiration: Counter=" << data.frame_counter;
	cout << ", State=" << data.sensor_state;
	cout << ", RPM=" << data.respiration_rate;
	cout << ", Distance=" << data.distance;
	cout << ", Movement (breathing pattern)=" << data.movement;
	cout << ", SigQ=" << data.signal_quality;
	cout << endl;
}

void mcpw_on_host_parser_respiration_moving_list(RespirationMovingListData data, void *user_data)
{
	float slow = 0.0f;
	float fast = 0.0f;
	if (data.interval_count > 0)
	{
		for (int i = 0; i < data.interval_count; i++)
		{
			slow += data.movement_slow_items[i];
			fast += data.movement_fast_items[i];
		}
		slow = slow / data.interval_count;
		fast = fast / data.interval_count;
	}

	cout << "Respiration MovingList: Counter=" << data.counter;
	cout << ", IntervalCount=" << data.interval_count;
	cout << ", Slow=" << slow;
	cout << ", Fast=" << fast;
	cout << endl;
}

void mcpw_on_host_parser_respiration_normalized_moving_list(RespirationNormalizedMovementListData data, void *user_data)
{
	float slow = 0.0f;
	float fast = 0.0f;
	if (data.count > 0)
	{
		for (int i = 0; i < data.count; i++)
		{
			slow += data.normalized_movement_slow_items[i];
			fast += data.normalized_movement_fast_items[i];
		}
		slow = slow / data.count;
		fast = fast / data.count;
	}

	cout << "Respiration Normalized MovingList: Counter=" << data.frame_counter;
	cout << ", binCount=" << data.count;
	cout << ", Normalized_Slow=" << slow;
	cout << ", Normalized_Fast=" << fast;
	cout << endl;
}

void mcpw_on_host_parser_sleep(SleepData data, void *user_data)
{
	cout << "Sleep: Counter=" << data.frame_counter;
	cout << ", State=" << data.sensor_state;
	cout << ", RPM=" << data.respiration_rate;
	cout << ", Distance=" << data.distance;
	cout << ", SigQ=" << data.signal_quality;
	cout << ", MovementSlow=" << data.movement_slow;
	cout << ", MovementFast=" << data.movement_fast;
	cout << endl;
}

void mcpw_on_host_parser_presence_single(PresenceSingleData data, void *user_data)
{
	cout << "PresenceSingle: Counter=" << data.frame_counter;
	cout << ", State=";
	if (data.presence_state == XTS_VAL_PRESENCE_PRESENCESTATE_PRESENCE)
		cout << "Presence";
	else if (data.presence_state == XTS_VAL_PRESENCE_PRESENCESTATE_NO_PRESENCE)
		cout << "No presence";
	else
		cout << "Initializing";
	if (data.distance > 0)
		cout << ", Distance=" << data.distance;

	cout << endl;
}

void mcpw_on_host_parser_presence_moving_list(PresenceMovingListData data, void *user_data)
{
	float slow = 0.0f;
	float fast = 0.0f;
	if (data.interval_count > 0)
	{
		for (int i = 0; i < data.interval_count; i++)
		{
			slow += data.movement_slow_items[i];
			fast += data.movement_fast_items[i];
		}
		slow = slow / data.interval_count;
		fast = fast / data.interval_count;
	}

	cout << "Presence MovingList: Counter=" << data.frame_counter;
	cout << ", IntervalCount=" << data.interval_count;
	cout << ", Slow=" << slow;
	cout << ", Fast=" << fast;
	cout << endl;
}

void mcpw_on_host_parser_baseband_ap(BasebandApData data, void *user_data)
{
	cout << "Baseband AP: Counter=" << data.frame_counter;
	cout << ", BinLength=" << data.bin_length;
	cout << ", NumBins=" << data.num_bins;
	cout << ", Fc=" << data.carrier_frequency;
	cout << ", Fs=" << data.sample_frequency;
	cout << endl;
}

void mcpw_on_host_parser_data_float(FloatData data, void *user_data)
{
	cout << "Float: ContentID=" << data.content_id;
	cout << ", Info=" << data.info;
	cout << ", Length=" << data.length;
	cout << endl;

	float radar_frame[80];
	uint32_t radar_frame_length_max = sizeof(radar_frame) / sizeof(float);
	uint32_t radar_frame_length = data.length;
	if (radar_frame_length > radar_frame_length_max)
		radar_frame_length = radar_frame_length_max;

	memcpy(radar_frame, data.data, sizeof(float) * radar_frame_length);

	// TODO: Add frame test here. Print to screen for now.
	cout << "# radar_frame_length: " << radar_frame_length << endl;
	for (int i = 0; i < radar_frame_length - 1; i++)
		cout << radar_frame[i] << ", ";
	cout << radar_frame[radar_frame_length - 1] << endl;

	global_test_results = 42;
}

void readThreadMethod(mcp_wrapper_t *mcpw)
{
	ModuleIo *moduleIo = (ModuleIo *)mcpw->user_reference;
	const int rx_buffer_length = 100;
	unsigned char rx_buffer[rx_buffer_length];

	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		moduleIoMutex.lock();
		int received = moduleIo->read(rx_buffer, rx_buffer_length, 100);
		moduleIoMutex.unlock();
		if (received <= 0)
			continue;

		mcpw_process_rx_data(mcpw, rx_buffer, received);
		//cout << "Read " << read << " bytes" << endl;
	}
}

int mcpw_demo_x4m300(char *com_port)
{
	cout << "Starting mcpw_demo_x4m300." << endl;

	ModuleIo *moduleIo = createModuleIo();
	if (0 != moduleIo->open(com_port))
	{
		cout << "Error opening " << com_port << ". Aborting." << endl;
		return -1;
	}

	cout << "Connecting to XeThru module on " << com_port << "." << endl;

	// Configure MCPW.
	uint32_t mcpw_instance_memory_size = mcpw_get_instance_size();
	void *mcpw_instance_memory = malloc(mcpw_instance_memory_size);
	mcp_wrapper_t *mcpw = mcpw_init(mcpw_instance_memory);
	mcpw->send_bytes = mcpw_send_bytes;
	mcpw->wait_for_response = mcpw_wait_for_response;
	mcpw->response_ready = mcpw_response_ready;
	mcpw->delay = mcpw_delay;
	mcpw->mcp_host_parser->presence_single = mcpw_on_host_parser_presence_single;		   // X4M300 presence single message
	mcpw->mcp_host_parser->presence_movinglist = mcpw_on_host_parser_presence_moving_list; // X4M300 presence movinglist message
	mcpw->mcp_host_parser->baseband_ap = mcpw_on_host_parser_baseband_ap;				   // X4M300 baseband AP message
	mcpw->user_reference = (void *)moduleIo;

	cout << "Starting serial port read thread." << endl;
	std::thread readThread(readThreadMethod, mcpw);

	// X4M300 Presence profile
	int res = 0;
	// First stop any running profile and change baudrate.
#if 1
	// Assume 115kbps, stop and change baudrate. If already 921kbps, these will fail, but the next stop will work.
	mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	mcpw_set_baudrate(mcpw, XTID_BAUDRATE_921600);
	moduleIo->setBaudrate(XTID_BAUDRATE_921600);
#endif
	mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	if (MCPW_OK != mcpw_load_profile(mcpw, XTS_ID_APP_PRESENCE_2))
		cout << "mcpw_load_profile failed." << endl;
	if (MCPW_OK != mcpw_set_noisemap_control(mcpw, XTID_NOISEMAP_CONTROL_ENABLE | XTID_NOISEMAP_CONTROL_ADAPTIVE))
		cout << "mcpw_set_noisemap_control failed." << endl;
	if (MCPW_OK != mcpw_set_detection_zone(mcpw, 1.0, 3.0))
		cout << "mcpw_set_detection_zone failed." << endl;
	if (MCPW_OK != mcpw_set_sensitivity(mcpw, 9))
		cout << "mcpw_set_sensitivity failed." << endl;
	if (MCPW_OK != mcpw_set_led_control(mcpw, XTID_LED_MODE_FULL, 100))
		cout << "mcpw_set_led_control failed." << endl;
	// Methods to turn on or off module data messages. Select _ENABLE or _DISABLE.
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_PRESENCE_SINGLE, XTID_OUTPUT_CONTROL_ENABLE))
		cout << "mcpw_set_output_control(XTS_ID_PRESENCE_SINGLE) failed." << endl;
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_PRESENCE_MOVINGLIST, XTID_OUTPUT_CONTROL_ENABLE))
		cout << "mcpw_set_output_control(XTS_ID_PRESENCE_MOVINGLIST) failed." << endl;
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_BASEBAND_AMPLITUDE_PHASE, XTID_OUTPUT_CONTROL_ENABLE))
		cout << "mcpw_set_output_control(XTS_ID_BASEBAND_AMPLITUDE_PHASE) failed." << endl;
	// Start module execution.
	if (MCPW_OK != mcpw_set_sensor_mode(mcpw, XTS_SM_RUN, 0))
		cout << "mcpw_set_sensor_mode failed." << endl;

	// Wait indefinately for readThread to finish.
	for (;;)
	{
		// Every 30 minutes, store noisemap.
		std::this_thread::sleep_for(std::chrono::minutes(30));
		res = mcpw_store_noisemap(mcpw);
		cout << "Store noisemap " << (res == MCPW_OK ? "succeeded" : "failed") << "." << endl;
	}
	readThread.join();

	delete moduleIo;
	moduleIo = NULL;

	free(mcpw_instance_memory);

	return 0;
}

int mcpw_demo_x4m200(char *com_port)
{
	cout << "Starting mcpw_demo_x4m200." << endl;

	ModuleIo *moduleIo = createModuleIo();
	if (0 != moduleIo->open(com_port))
	{
		cout << "Error opening " << com_port << ". Aborting." << endl;
		return -1;
	}

	cout << "Connecting to XeThru module on " << com_port << "." << endl;

	// Configure MCPW.
	uint32_t mcpw_instance_memory_size = mcpw_get_instance_size();
	void *mcpw_instance_memory = malloc(mcpw_instance_memory_size);
	mcp_wrapper_t *mcpw = mcpw_init(mcpw_instance_memory);
	mcpw->send_bytes = mcpw_send_bytes;
	mcpw->wait_for_response = mcpw_wait_for_response;
	mcpw->response_ready = mcpw_response_ready;
	mcpw->delay = mcpw_delay;
	mcpw->mcp_host_parser->sleep = mcpw_on_host_parser_sleep;														   // X4M200 sleep message
	mcpw->mcp_host_parser->respiration = mcpw_on_host_parser_respiration;											   // X4M200 legacy respiration message (original X2M200 resp message)
	mcpw->mcp_host_parser->respiration_movinglist = mcpw_on_host_parser_respiration_moving_list;					   // X4M200 movinglist message
	mcpw->mcp_host_parser->respiration_normalized_movinglist = mcpw_on_host_parser_respiration_normalized_moving_list; // X4M200 normalized movinglist message
	mcpw->mcp_host_parser->baseband_ap = mcpw_on_host_parser_baseband_ap;											   // X4M200 baseband AP message
	mcpw->user_reference = (void *)moduleIo;

	cout << "Starting serial port read thread." << endl;
	std::thread readThread(readThreadMethod, mcpw);

	// X4M200 Respiration profile
	int res = 0;
	// First stop any running profile and change baudrate.
#if 1
	// Assume 115kbps, stop and change baudrate. If already 921kbps, these will fail, but the next stop will work.
	mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	mcpw_set_baudrate(mcpw, XTID_BAUDRATE_921600);
	moduleIo->setBaudrate(XTID_BAUDRATE_921600);
#endif
	mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	if (MCPW_OK != mcpw_load_profile(mcpw, XTS_ID_APP_RESPIRATION_2))
		cout << "mcpw_load_profile failed." << endl;
	if (MCPW_OK != mcpw_set_noisemap_control(mcpw, XTID_NOISEMAP_CONTROL_ENABLE | XTID_NOISEMAP_CONTROL_ADAPTIVE))
		cout << "mcpw_set_noisemap_control failed." << endl;
	if (MCPW_OK != mcpw_set_detection_zone(mcpw, 0.5, 3.5))
		cout << "mcpw_set_detection_zone failed." << endl;
	if (MCPW_OK != mcpw_set_sensitivity(mcpw, 9))
		cout << "mcpw_set_sensitivity failed." << endl;
	if (MCPW_OK != mcpw_set_led_control(mcpw, XTID_LED_MODE_FULL, 100))
		cout << "mcpw_set_led_control failed." << endl;
	// Methods to turn on or off module data messages. Select _ENABLE or _DISABLE.
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_SLEEP_STATUS, XTID_OUTPUT_CONTROL_ENABLE))
		cout << "mcpw_set_output_control(XTS_ID_SLEEP_STATUS) failed." << endl;
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_RESP_STATUS, XTID_OUTPUT_CONTROL_DISABLE))
		cout << "mcpw_set_output_control(XTS_ID_RESP_STATUS) failed." << endl;
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_RESPIRATION_MOVINGLIST, XTID_OUTPUT_CONTROL_DISABLE))
		cout << "mcpw_set_output_control(XTS_ID_RESPIRATION_MOVINGLIST) failed." << endl;
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_RESPIRATION_NORMALIZEDMOVEMENTLIST, XTID_OUTPUT_CONTROL_ENABLE))
		cout << "mcpw_set_output_control(XTS_ID_RESPIRATION_MOVINGLIST) failed." << endl;
	if (MCPW_OK != mcpw_set_output_control(mcpw, XTS_ID_BASEBAND_AMPLITUDE_PHASE, XTID_OUTPUT_CONTROL_DISABLE))
		cout << "mcpw_set_output_control(XTS_ID_BASEBAND_AMPLITUDE_PHASE) failed." << endl;
	// Start module execution.
	if (MCPW_OK != mcpw_set_sensor_mode(mcpw, XTS_SM_RUN, 0))
		cout << "mcpw_set_sensor_mode failed." << endl;

	// Wait indefinately for readThread to finish.
	for (;;)
	{
		// Every 30 minutes, store noisemap.
		std::this_thread::sleep_for(std::chrono::minutes(30));
		res = mcpw_store_noisemap(mcpw);
		cout << "Store noisemap " << (res == MCPW_OK ? "succeeded" : "failed") << "." << endl;
	}
	readThread.join();

	delete moduleIo;
	moduleIo = NULL;

	free(mcpw_instance_memory);

	return 0;
}

int mcpw_demo_x4m200_test(char *com_port, uint8_t testcode)
{
	cout << "Starting mcpw_demo_x4m200." << endl;

	ModuleIo *moduleIo = createModuleIo();
	if (0 != moduleIo->open(com_port))
	{
		cout << "Error opening " << com_port << ". Aborting." << endl;
		return -1;
	}

	cout << "Connecting to XeThru module on " << com_port << "." << endl;

	// Configure MCPW.
	uint32_t mcpw_instance_memory_size = mcpw_get_instance_size();
	void *mcpw_instance_memory = malloc(mcpw_instance_memory_size);
	mcp_wrapper_t *mcpw = mcpw_init(mcpw_instance_memory);
	mcpw->send_bytes = mcpw_send_bytes;
	mcpw->wait_for_response = mcpw_wait_for_response;
	mcpw->response_ready = mcpw_response_ready;
	mcpw->delay = mcpw_delay;
	mcpw->mcp_host_parser->sleep = mcpw_on_host_parser_sleep;									 // X4M200 sleep message
	mcpw->mcp_host_parser->respiration = mcpw_on_host_parser_respiration;						 // X4M200 legacy respiration message (original X2M200 resp message)
	mcpw->mcp_host_parser->respiration_movinglist = mcpw_on_host_parser_respiration_moving_list; // X4M200 movinglist message
	mcpw->mcp_host_parser->baseband_ap = mcpw_on_host_parser_baseband_ap;						 // X4M200 baseband AP message
	mcpw->user_reference = (void *)moduleIo;

	cout << "Starting serial port read thread." << endl;
	std::thread readThread(readThreadMethod, mcpw);

	mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	mcpw_system_run_test(mcpw, testcode);
	cout << "test mode start!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	while (1)
		;
}

int mcpw_demo_x2m200(char *com_port)
{
	cout << "Starting mcpw_demo_x2m200." << endl;

	ModuleIo *moduleIo = createModuleIo();
	if (0 != moduleIo->open(com_port))
	{
		cout << "Error opening " << com_port << ". Aborting." << endl;
		return -1;
	}

	cout << "Connecting to XeThru module on " << com_port << "." << endl;

	// Configure MCPW.
	void *mcpw_instance_memory = malloc(mcpw_get_instance_size());
	mcp_wrapper_t *mcpw = mcpw_init(mcpw_instance_memory);
	mcpw->send_bytes = mcpw_send_bytes;
	mcpw->wait_for_response = mcpw_wait_for_response;
	mcpw->response_ready = mcpw_response_ready;
	mcpw->delay = mcpw_delay;
	mcpw->mcp_host_parser->sleep = mcpw_on_host_parser_sleep;			  // X2M200 sleep message
	mcpw->mcp_host_parser->respiration = mcpw_on_host_parser_respiration; // X2M200 respiration message
	mcpw->user_reference = (void *)moduleIo;

	// X2M200 Sleep profile
	int res = 0;
	// Send commands to XeThru module

	// First perform module reset. Without read thread, this will timeout. Intended.
	mcpw_module_reset(mcpw);
	// In case of USB connection, because of the module reset, we need to close the serial port, then wait for the USB serial port to be available again. Then reconnect.
	// This is not needed when connected to module UART, but it will still work for both connection.
	// Could be removed if only connected using UART.
	delete moduleIo;
	moduleIo = NULL;
	cout << "Reopening " << com_port << " after reset..." << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	moduleIo = createModuleIo(); // Reopen port.
	if (0 != moduleIo->open(com_port))
	{
		cout << "Error opening " << com_port << ". Aborting." << endl;
		return -1;
	}
	mcpw->user_reference = (void *)moduleIo;

	cout << "Starting serial port read thread." << endl;
	std::thread readThread(readThreadMethod, mcpw);

	res |= mcpw_load_profile(mcpw, XTS_ID_APP_SLEEP);
	res |= mcpw_set_detection_zone(mcpw, 0.4, 2.0);
	res |= mcpw_set_sensitivity(mcpw, 5);
	res |= mcpw_set_led_control(mcpw, XTID_LED_MODE_FULL, 100);
	res |= mcpw_set_sensor_mode(mcpw, XTS_SM_RUN, 0);

	readThread.join();

	delete moduleIo;
	moduleIo = NULL;

	return 0;
}

int mcpw_demo_upgrade_x4m300(char *com_port)
{
	cout << "Starting mcpw_demo_upgrade." << endl;

	ModuleIo *moduleIo = createModuleIo();
	if (0 != moduleIo->open(com_port))
	{
		cout << "Error opening " << com_port << ". Aborting." << endl;
		return -1;
	}

	cout << "Connecting to XeThru module on " << com_port << "." << endl;

	// Configure MCPW.
	uint32_t mcpw_instance_memory_size = mcpw_get_instance_size();
	void *mcpw_instance_memory = malloc(mcpw_instance_memory_size);
	mcp_wrapper_t *mcpw = mcpw_init(mcpw_instance_memory);
	mcpw->send_bytes = mcpw_send_bytes;
	mcpw->wait_for_response = mcpw_wait_for_response;
	mcpw->response_ready = mcpw_response_ready;
	mcpw->delay = mcpw_delay;
	mcpw->user_reference = (void *)moduleIo;

	cout << "Starting serial port read thread." << endl;
	std::thread readThread(readThreadMethod, mcpw);

	// Generic profile
	int res = 0;
	// Send commands to XeThru module
	res = mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	mcpw->delay(500);

	// Read module info
	char system_info[50];
	res = mcpw_get_systeminfo(mcpw, XTID_SSIC_FIRMWAREID, system_info, sizeof(system_info));
	res = mcpw_get_systeminfo(mcpw, XTID_SSIC_VERSION, system_info, sizeof(system_info));

	uint32_t fw_length = sizeof(X4M300_1_2_1_bin);
	uint8_t *fw_data = (uint8_t *)X4M300_1_2_1_bin;
	res = mcpw_firmware_upgrade(mcpw, fw_data, fw_length);

	// Wait indefinately for readThread to finish.
	readThread.join();

	delete moduleIo;
	moduleIo = NULL;

	free(mcpw_instance_memory);

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 1)
	{
		cout << "Add XeThru module serial port as parameter, e.g. COM1.";
		return -1;
	}
	char *com_port = argv[1];

	if ((argc == 4) && (strcmp(argv[2], "upgrade") == 0) && (strcmp(argv[3], "x4m300") == 0))
	{
		return mcpw_demo_upgrade_x4m300(com_port);
	}

	if ((argc == 3) && (strcmp(argv[2], "test") == 0))
	{
		return mcpw_demo_x4m200_test(com_port, 0x1F);
	}

	if (argc == 3)
	{
		std::string demo_to_run(argv[2]);
		if ((demo_to_run == "x2m200") || (demo_to_run == "X2M200"))
			return mcpw_demo_x2m200(com_port);
		else if ((demo_to_run == "x4m200") || (demo_to_run == "X4M200"))
			return mcpw_demo_x4m200(com_port);
	}

	return mcpw_demo_x4m300(com_port);
}
