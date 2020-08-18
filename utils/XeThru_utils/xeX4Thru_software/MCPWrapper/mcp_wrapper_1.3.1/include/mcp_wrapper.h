#ifndef MCP_WRAPPER_H
#define MCP_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>

#include "mcp/protocol.h"
#include "mcp/protocol_host_parser.h"

#define MCPW_OK					0
#define MCPW_ERROR				1
#define MCPW_ERROR_TIMEOUT		2

#define MCPW_VERSION_STRING "1.3.0"


#define MCPW_RX_BUFFER_LENGTH 1024*7 // Short, processed messages: 200. Baseband data: 7000.
#define MCPW_TX_BUFFER_LENGTH 640 // No FW upgrade: 100, FW Upgrade: 640
#define MCPW_SYNC_RESPONSE_LENGTH 200

typedef bool (*mcpw_send_bytes_method)(void* mcpw_ref);
typedef bool (*mcpw_wait_for_response_method)(uint32_t timeout);
typedef void (*mcpw_response_ready_method)(void);
typedef void(*mcpw_delay_method)(uint32_t delay_ms);

typedef struct
{
	uint32_t tx_buf_index;
	uint8_t rx_buf[MCPW_RX_BUFFER_LENGTH]; //  __attribute__((aligned(4)));
	uint8_t tx_buf[MCPW_TX_BUFFER_LENGTH]; //  __attribute__((aligned(4)));
	uint8_t sync_response[MCPW_SYNC_RESPONSE_LENGTH];
	uint32_t sync_response_length;

	xtProtocol * mcp;
	HostParser * mcp_host_parser;
	AppendCallback on_mcp_messagebuild;
	mcpw_send_bytes_method send_bytes;
	mcpw_wait_for_response_method wait_for_response;
	mcpw_response_ready_method response_ready;
	mcpw_delay_method delay;

	uint32_t default_timeout;

	bool busy;
	bool bootloader_mode;

	Reply reply;
	void* user_reference;
} mcp_wrapper_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Driver methods */
uint32_t mcpw_get_instance_size(void);
mcp_wrapper_t * mcpw_init(void *instance_memory);
void mcpw_process_rx_data(mcp_wrapper_t *mcpw, uint8_t *data, uint32_t length);

/* Bootloader methods */
int mcpw_start_bootloader(mcp_wrapper_t *mcpw);
int mcpw_bl_write_page(mcp_wrapper_t *mcpw, uint32_t page_address, uint8_t *page_data, uint32_t length);
int mcpw_bl_start_application(mcp_wrapper_t *mcpw);
// Bootloader convenience methods.
int mcpw_firmware_upgrade(mcp_wrapper_t *mcpw, uint8_t* fw_data, uint32_t fw_length);

/* Module profile methods */
int mcpw_ping(mcp_wrapper_t *mcpw);
int mcpw_module_reset(mcp_wrapper_t *mcpw);
int mcpw_load_profile(mcp_wrapper_t *mcpw, uint32_t profileid);
int mcpw_set_sensor_mode(mcp_wrapper_t *mcpw, uint8_t mode, uint8_t param);
int mcpw_set_sensitivity(mcp_wrapper_t *mcpw, uint32_t sensitivity);
int mcpw_set_detection_zone(mcp_wrapper_t *mcpw, float start, float end);
int mcpw_set_led_control(mcp_wrapper_t *mcpw, uint8_t mode, uint8_t intensity);
int mcpw_set_output_control(mcp_wrapper_t *mcpw, uint32_t output_feature, uint32_t output_control);
int mcpw_set_parameter_file(mcp_wrapper_t *mcpw, uint32_t filename_length, uint32_t data_length, const char * filename, const char * data);
int mcpw_get_systeminfo(mcp_wrapper_t *mcpw, uint8_t info_code, char * result, uint32_t max_length);

int mcpw_store_noisemap(mcp_wrapper_t *mcpw); 
int mcpw_set_noisemap_control(mcp_wrapper_t *mcpw, uint32_t noisemap_control);
int mcpw_set_baudrate(mcp_wrapper_t *mcpw, uint32_t baudrate);
int mcpw_system_run_test(mcp_wrapper_t *mcpw, uint8_t testcode);
int mcpw_reset_to_factory_preset(mcp_wrapper_t *mcpw);


/* Special non-documented methods */
int mcpw_x4driver_set_pif_register(mcp_wrapper_t *mcpw, uint8_t address, uint8_t value);
int mcpw_x4driver_set_dac_min(mcp_wrapper_t *mcpw, uint32_t dac_min);
int mcpw_x4driver_set_dac_max(mcp_wrapper_t *mcpw, uint32_t dac_max);
int mcpw_x4driver_set_pulses_per_step(mcp_wrapper_t *mcpw, uint32_t pulsesperstep);
int mcpw_x4driver_set_iterations(mcp_wrapper_t *mcpw, uint32_t iterations);
int mcpw_x4driver_set_downconversion(mcp_wrapper_t *mcpw, uint8_t downconversion);
int mcpw_x4driver_set_fps(mcp_wrapper_t *mcpw, float fps);
int mcpw_x4driver_set_frame_area(mcp_wrapper_t *mcpw, float start, float end);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MCP_WRAPPER_H */
