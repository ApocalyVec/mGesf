#include "mcp_wrapper.h"
#include <stdio.h>
#include <string.h>
#include "mcp/protocol.h"

#define MCPW_BOOTLOADER_PROTO_RSP_SUCCESS  (0xe0)


bool mcpw_start_send(mcp_wrapper_t *mcpw);
void mcpw_on_mcp_messagebuild(unsigned char byte, void * user_data);
void mcpw_mcp_handle_protocol_packet(void * userData, const unsigned char * data, unsigned int length);
void mcpw_mcp_handle_protocol_error(void * userData, unsigned int error);

// MCP host parser callbacks implemented here
void mcpw_on_host_parser_ack(Ack ack, void* user_data);
void mcpw_on_host_parser_error(uint32_t error, void * user_data);
void mcpw_on_host_parser_reply(Reply reply, void * user_data);

// MCP host parser callbacks that should be implemented in user code
//void mcpw_on_host_parser_pong(uint32_t pong, void * user_data);
//void mcpw_on_host_parser_datafloat(FloatData data, void * user_data);
//void mcpw_on_host_parser_respiration(RespirationData data, void * user_data);
//void mcpw_on_host_parser_sleep(SleepData data, void * user_data);
//void mcpw_on_host_parser_baseband_ap(BasebandApData data, void * user_data);
//void mcpw_on_host_parser_baseband_iq(BasebandIqData data, void * user_data);
//void mcpw_on_host_parser_presence_single(PresenceSingleData data, void * user_data);
//void mcpw_on_host_parser_presence_movinglist(PresenceMovingListData data, void * user_data);
//void mcpw_on_host_parser_system_status(SystemStatus ss, void * user_data);
//void mcpw_on_host_parser_respiration_movinglist(RespirationMovingListData data, void * user_data);
//void mcpw_on_host_parser_respiration_detectionlist(RespirationDetectionListData data, void * user_data);



void mcpw_on_host_parser_ack(Ack ack, void* user_data)
{
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*)user_data;
	mcpw->sync_response[0] = XTS_SPR_ACK;
	mcpw->sync_response_length = 1;
	mcpw->response_ready();
}

void mcpw_on_host_parser_error(uint32_t error, void * user_data)
{
	if (error == 2) return; // Unknown error in parse, too trigger happy to include here.

	mcp_wrapper_t *mcpw = (mcp_wrapper_t*)user_data;
	mcpw->sync_response[0] = XTS_SPR_ERROR;
	mcpw->sync_response_length = 1;
	mcpw->response_ready();
}

void mcpw_on_host_parser_reply(Reply reply, void * user_data)
{
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*)user_data;
	mcpw->sync_response[0] = XTS_SPR_REPLY;
	mcpw->sync_response_length = 1;
	memcpy(&mcpw->reply, &reply, sizeof(Reply));
	mcpw->response_ready();
}

void mcpw_on_mcp_messagebuild(unsigned char byte, void * user_data)
{
	// Callback used by protocol to create message.
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*)user_data;
	if (mcpw->tx_buf_index >= MCPW_TX_BUFFER_LENGTH) return; // Unhandled error. TX buffer too small.
	mcpw->tx_buf[mcpw->tx_buf_index++] = byte;
}

uint32_t mcpw_get_instance_size(void)
{
	return sizeof(mcp_wrapper_t) + getInstanceSize() + sizeof(HostParser);
}

bool mcpw_start_send(mcp_wrapper_t *mcpw)
{
	if (mcpw->busy) return false;
	mcpw->busy = true;
	mcpw->tx_buf_index = 0;
	return true;
}

mcp_wrapper_t * mcpw_init(void *instance_memory)
{
	mcp_wrapper_t * mcpw = (mcp_wrapper_t*)instance_memory;
	void* protocol_instance_memory = (int)(void*)mcpw + sizeof(mcp_wrapper_t); // Place protocol instance right after mcp_wrapper struct in already alocated memory provided.
	void* mcp_host_parser_instance_memory = (int)(void*)protocol_instance_memory + getInstanceSize();
	mcpw->mcp = createApplicationProtocol(
		&mcpw_mcp_handle_protocol_packet,
		&mcpw_mcp_handle_protocol_error,
		(void*)mcpw,
		protocol_instance_memory,
		mcpw->rx_buf,
		sizeof(mcpw->rx_buf));

	mcpw->on_mcp_messagebuild = mcpw_on_mcp_messagebuild;
	mcpw->send_bytes = NULL; // Must be implemented by user.
	mcpw->wait_for_response = NULL;
	mcpw->response_ready = NULL;
	mcpw->delay = NULL;

	mcpw->sync_response_length = 0;
	mcpw->default_timeout = 2000;

	mcpw->tx_buf_index = 0;
	mcpw->busy = false;
	mcpw->bootloader_mode = false;
	mcpw->mcp_host_parser = (HostParser*)mcp_host_parser_instance_memory;

	init_host_parser(mcpw->mcp_host_parser);
	mcpw->mcp_host_parser->ack = mcpw_on_host_parser_ack;
	mcpw->mcp_host_parser->error = mcpw_on_host_parser_error;
	mcpw->mcp_host_parser->reply = mcpw_on_host_parser_reply;


	return mcpw;
}

int parse_bootloader_packet(mcp_wrapper_t *mcpw, const unsigned char * data, unsigned int length)
{
	if (!mcpw) return MCPW_ERROR;
	if (!mcpw->mcp) return MCPW_ERROR;

	if (length >= MCPW_SYNC_RESPONSE_LENGTH)
		return MCPW_ERROR;

	memcpy(mcpw->sync_response, data, length);
	mcpw->sync_response_length = length;
	mcpw->response_ready();
}

void mcpw_process_rx_data(mcp_wrapper_t *mcpw, uint8_t *data, uint32_t length)
{
	if (!mcpw) return;
	parseData(mcpw->mcp, data, length);
}

void mcpw_mcp_handle_protocol_packet(void * userData, const unsigned char * data, unsigned int length)
{
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*) userData;

	if (!mcpw->bootloader_mode)
		parse(mcpw->mcp_host_parser, data, length, (void*) mcpw);
	else
		parse_bootloader_packet(mcpw, data, length);
}


void mcpw_mcp_handle_protocol_error(void * userData, unsigned int error)
{
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*) userData;

}




/* Bootloader methods */

int mcpw_start_bootloader(mcp_wrapper_t *mcpw)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	int ret = MCPW_OK;
	createStartBootloaderCommand(XTS_DEF_ENTER_BOOTLOADER_KEY_2, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		// New key failed, try old.
		createStartBootloaderCommand(XTS_DEF_ENTER_BOOTLOADER_KEY, mcpw->on_mcp_messagebuild, (void*)mcpw);
		mcpw->send_bytes(mcpw);
		if (!mcpw->wait_for_response(mcpw->default_timeout))
		{
			ret = MCPW_ERROR_TIMEOUT;
		}
	}
	mcpw->bootloader_mode = true; // Assume OK or already in bootloader mode.
	mcpw->busy = false;

    return ret;
}

int mcpw_bl_write_page(mcp_wrapper_t *mcpw, uint32_t page_address, uint8_t *page_data, uint32_t length)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createPageWriteCommand(page_address, page_data, length, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(10000))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length > 0) && (mcpw->sync_response[0] == MCPW_BOOTLOADER_PROTO_RSP_SUCCESS))
	{
		ret = MCPW_OK;
	}
	mcpw->busy = false;

    return ret;
}

int mcpw_bl_start_application(mcp_wrapper_t *mcpw)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createApplicationStartCommand(mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(10000))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == MCPW_BOOTLOADER_PROTO_RSP_SUCCESS))
	{
		ret = MCPW_OK;
		mcpw->sync_response_length = 0; // Reset rx buffer.
	}
	mcpw->bootloader_mode = false;
	mcpw->busy = false;

    return ret;
}


// Bootloader firmware upgrade convenience method
int mcpw_firmware_upgrade(mcp_wrapper_t *mcpw, uint8_t* fw_data, uint32_t fw_length)
{
	int ret = MCPW_OK;
	mcpw->delay(1000);
	ret = mcpw_start_bootloader(mcpw);
	mcpw->delay(1000);
	ret = MCPW_OK; // Assume either OK, or already in bootloader mode.

	const uint32_t page_size = 512;
	uint8_t tmp_buf[512];
	uint32_t page = 0;

	while ((ret == MCPW_OK) && (page*page_size < fw_length))
	{
		if ((page == 0) || (page > 127))
		{
			uint32_t data_length = fw_length - page*page_size;
			if (data_length >= page_size)
			{
				ret |= mcpw_bl_write_page(mcpw, page, &fw_data[page_size*page], page_size);
			}
			else
			{
				// Last page, must pad with 0xff.
				memset(tmp_buf, 0xff, page_size);
				memcpy(tmp_buf, &fw_data[page_size*page], data_length);
				ret |= mcpw_bl_write_page(mcpw, page, tmp_buf, page_size);
			}
		}
		page++;
	}

	ret |= mcpw_bl_start_application(mcpw);
	return ret;
}

/* Module profile methods */

int mcpw_ping(mcp_wrapper_t *mcpw)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createPingCommand(mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	mcpw->busy = false;
	return MCPW_OK;
}

int mcpw_module_reset(mcp_wrapper_t *mcpw)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createModuleResetCommand(mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;

    return ret;
}

int mcpw_load_profile(mcp_wrapper_t *mcpw, uint32_t profileid)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createLoadProfileCommand(profileid, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_set_sensor_mode(mcp_wrapper_t *mcpw, uint8_t mode, uint8_t param)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetSensorModeCommand(mode, param, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_set_sensitivity(mcp_wrapper_t *mcpw, uint32_t sensitivity)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetSensitivityCommand(sensitivity, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_set_detection_zone(mcp_wrapper_t *mcpw, float start, float end)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetDetectionZoneCommand(start, end, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_set_led_control(mcp_wrapper_t *mcpw, uint8_t mode, uint8_t intensity)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetLedControlCommand(mode, intensity, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_set_output_control(mcp_wrapper_t *mcpw, uint32_t output_feature, uint32_t output_control)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetOutputControlCommand(output_feature, output_control, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}


int mcpw_set_parameter_file(mcp_wrapper_t *mcpw, uint32_t filename_length, uint32_t data_length, const char * filename, const char * data)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetParameterFileCommand(filename_length, data_length, filename, data, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_get_systeminfo(mcp_wrapper_t *mcpw, uint8_t info_code,	char * result, uint32_t max_length)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createGetSystemInfoCommand(info_code, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response[0] == XTS_SPR_REPLY) && (mcpw->reply.data_size*mcpw->reply.length <= max_length))
	{
		memcpy(result, mcpw->reply.data, mcpw->reply.data_size*mcpw->reply.length);
		if (mcpw->reply.data_size*mcpw->reply.length < max_length)
			result[mcpw->reply.data_size*mcpw->reply.length] = 0; // NULL-terminate if string.
		ret = MCPW_OK;
	}
	mcpw->busy = false;
	return ret;
}

int mcpw_store_noisemap(mcp_wrapper_t *mcpw)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createStoreNoiseMapCommand(mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}

int mcpw_set_noisemap_control(mcp_wrapper_t *mcpw, uint32_t noisemap_control)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetNoiseMapControlCommand(noisemap_control, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}

int mcpw_set_baudrate(mcp_wrapper_t *mcpw, uint32_t baudrate)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetBaudRateCommand(baudrate, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}

int mcpw_system_run_test(mcp_wrapper_t *mcpw, uint8_t testcode )
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSystemRunTest(testcode, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}

int mcpw_reset_to_factory_preset(mcp_wrapper_t *mcpw)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createResetToFactoryPreset(mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}


int mcpw_x4driver_set_pif_register(mcp_wrapper_t *mcpw, uint8_t address, uint8_t value)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createX4DriverSetPifRegisterCommand(address, value, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}

int mcpw_x4driver_set_dac_min(mcp_wrapper_t *mcpw, uint32_t dac_min)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createX4DriverSetDacMinCommand(dac_min, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}


int mcpw_x4driver_set_dac_max(mcp_wrapper_t *mcpw, uint32_t dac_max) {
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createX4DriverSetDacMaxCommand(dac_max, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}


int mcpw_x4driver_set_pulses_per_step(mcp_wrapper_t *mcpw, uint32_t pulsesperstep)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createX4DriverSetPulsesPerStepCommand(pulsesperstep, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}


int mcpw_x4driver_set_iterations(mcp_wrapper_t *mcpw, uint32_t iterations)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createX4DriverSetIterationsCommand(iterations, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}


int mcpw_x4driver_set_downconversion(mcp_wrapper_t *mcpw, uint8_t downconversion)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createX4DriverSetDownconversionCommand(downconversion, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}


int mcpw_x4driver_set_fps(mcp_wrapper_t *mcpw, float fps)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createX4DriverSetFpsCommand(fps, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}


int mcpw_x4driver_set_frame_area(mcp_wrapper_t *mcpw, float start, float end)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createX4DriverSetFrameAreaCommand(start, end, mcpw->on_mcp_messagebuild, (void*)mcpw);
	mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;
	return ret;
}