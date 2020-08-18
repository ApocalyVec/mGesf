/**
 * @file
 *
 *
 */

#include "xep_application_mcp_callbacks.h"
#include "xep.h"
#include "xep_hal.h"
#include "xtserial_definitions.h"
#include "xep_dispatch_messages.h"
#include "xtid.h"
#include "xep_hal.h"
#include "build.h"
#include "version.h"
#include "xtfile.h"
#include "xthousekeeping.h"
#include "xfs.h"
#include "xtmemory.h"
#include "task_radar.h"
#include "module_ui.h"
#include "FreeRTOS.h"
#include <string.h>
//## #include "conf_usb.h"

void onMcpPing(uint32_t pingval, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    uint32_t pongval=0;
    if (!xthousekeeping_in_safe_mode())
    {
        pongval = XTS_DEF_PONGVAL_READY;
    } else
    {
        pongval = XTS_DEF_PONGVAL_SAFEMODE;
    }

    dispatch_message_hostcom_send_pong(xep->dispatch, xep->route, pongval);
}

void onMcpUnknown(const uint8_t* data, uint32_t length, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_NOT_RECOGNIZED);
}

void onMcpSetBaudrate(uint32_t baudrate, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    
    // Wait a while for ack to be sent
    xt_wait(500);

    xtio_uart_set_baudrate(baudrate);
}

void onMcpModuleReset(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);

    // Wait a while for ack to be sent
    xt_wait(500);

    xt_software_reset(XT_SWRST_HOSTCOMMAND);
}

void onMcpSystemRunTest(uint8_t testcode, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    xt_test_result_t test_result;

    switch (testcode)
    {
        case XTS_SSTC_TEST_CERT_TX_ONLY:
            dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);

            xt_wait(500);
            portENTER_CRITICAL();
            module_ui_led_override_color(0, 0, 0, true);
            radar_certification_enter_tx_only(xep->x4driver);

            break;
        case XTS_SSTC_TEST_CERT_TX_ONLY_TX_OFF:
            dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
            
            xt_wait(500);
            portENTER_CRITICAL();
            module_ui_led_override_color(0, 0, 0, true);
            radar_certification_enter_tx_only_tx_off(xep->x4driver);

            break;
        case XTS_SSTC_TEST_CERT_STREAM_TX_OFF:
        {
            uint32_t res = x4driver_set_tx_power(xep->x4driver, TX_POWER_OFF);
            module_ui_led_override_color(1, 1, 0, true);
            if (res == 0)
                dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
            else
                dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
            
            break;
        }
        default:
            if (xt_run_selftest(testcode, &test_result) != XT_SUCCESS)
            {
                dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_NOT_RECOGNIZED);
                return;
            }
            
            dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route, 
                (XTS_SPC_DIR_COMMAND)|(XTS_SDC_SYSTEM_TEST<<8), 
                test_result.id|((test_result.passed ? 1 : 0) << 8), 
                test_result.data, 
                XT_SELFTEST_DATA_LENGTH);

            break;
    }
}

void onMcpStartBootloader(uint32_t key, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    uint32_t key_ref = 0xa2b96ef1;
    if (key != key_ref)
    {
        dispatch_message_hostcom_send_data_string(xep->dispatch, xep->route, 0, 0, (char*)"Unknown start_bootloader key.");
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_NOT_RECOGNIZED);
        return;
    }

    dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);

    // Wait a while for ack to be sent
    xt_wait(500);

    xt_software_reset(XT_SWRST_BOOTLOADER);
}

void onMcpGetSystemInfo(uint32_t contentid, uint8_t infocode, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    if (infocode == XTID_SSIC_FIRMWAREID)
        dispatch_message_hostcom_send_reply_string(xep->dispatch, xep->route, contentid, infocode, (char*)PRODUCT_NAME);
    else if (infocode == XTID_SSIC_VERSION)
        dispatch_message_hostcom_send_reply_string(xep->dispatch, xep->route, contentid, infocode, (char*)VERSION_STRING);
    else if (infocode == XTID_SSIC_BUILD)
        dispatch_message_hostcom_send_reply_string(xep->dispatch, xep->route, contentid, infocode, (char*)BUILD_STRING);
    else if (infocode == XTID_SSIC_VERSIONLIST)
    {
        char version_list[40];
        int len = xep_get_systeminfo_versionlist(version_list, 40);
        if (len > 0)
            dispatch_message_hostcom_send_reply_string(xep->dispatch, xep->route, contentid, infocode, version_list);
        else
            dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_RESERVED);
    }
    else if (infocode == XTID_SSIC_BOOTLOADER)
    {
        dispatch_message_hostcom_send_reply_string(xep->dispatch, xep->route,
                                                   contentid, infocode,
                                                   xt_bootloader_string());
    }
    else if (infocode == XTID_SSIC_SYSTEMCOREID)
    {
        char system_core_id[40];
        uint32_t max_size = sizeof(system_core_id)-1; // Room for '\0'
        uint32_t size = 0;
        if (XT_SUCCESS == xtio_flash_get_unique_id(system_core_id, max_size, &size))
        {
            system_core_id[size] = '\0';
            dispatch_message_hostcom_send_reply_string(xep->dispatch, xep->route, contentid, infocode, system_core_id);
        }
        else
            dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_COMMAND_FAILED);
    }
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_NOT_RECOGNIZED);
}

void onMcpGetCrashDumpLength(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    uint32_t size = 0;
    if (xtfile_get_size(XTFILE_TYPE_CRASHDUMP, 0, &size) != XTFILE_OK)
        size = 0;
    
    dispatch_message_hostcom_send_reply_int(xep->dispatch, xep->route, XTS_SDC_GET_CRASH_DUMP_LENGTH, 0, (int32_t*)&size, 1);
}

void onMcpGetCrashDump(uint32_t offset, uint32_t length, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    void* handle = 0;
    if (xtfile_open(&handle, XTFILE_TYPE_CRASHDUMP, 0) != XTFILE_OK)
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_NOT_RECOGNIZED);
    
    void* data = 0;
    uint32_t size = 0;

    if (xtfile_get_buffer(handle, &data, &size) != XTFILE_OK)
    {
        xtfile_close(handle);
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_NOT_RECOGNIZED);
    }
    
    if ((offset + length) > size)
    {
        xtfile_close(handle);
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_USER);
    }

    dispatch_message_hostcom_send_reply_byte(
        xep->dispatch, xep->route,
        XTS_SDC_GET_CRASH_DUMP,
        0,
        &(((uint8_t*)data)[offset]),
        length);

    xtfile_close(handle);
}

void onMcpClearCrashDump(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    switch (xtfile_delete(XTFILE_TYPE_CRASHDUMP, 0))
    {
        case XTFILE_OK:
        case XTFILE_FILE_NOT_FOUND:
            dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
            break;
        default:
            dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_BUSY);
    }
}



/* --- X4Driver commands --- */

void onMcpX4DriverSetFps(float32_t fps, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_fps(xep->x4driver, fps);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetIterations(uint32_t iterations, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_iterations(xep->x4driver, iterations);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}
void onMcpX4DriverGetIterations(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    uint8_t iterations=0;
    int32_t res = x4driver_get_iterations(xep->x4driver, &iterations);
    int32_t iterations_i32=(int32_t)iterations;
    if (res == 0)
        dispatch_message_hostcom_send_reply_int(xep->dispatch, xep->route,XTS_SPCXI_ITERATIONS,0,&iterations_i32,1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}
void onMcpX4DriverSetPulsesPerStep(uint32_t pulsesperstep, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_pulses_per_step(xep->x4driver, pulsesperstep);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}
void onMcpX4DriverGetPulsesPerStep(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    uint16_t pulseperstep=0;
    int32_t res = x4driver_get_pulses_per_step(xep->x4driver, &pulseperstep);
    int32_t pulseperstep_i32=(int32_t)pulseperstep;
    if (res == 0)
        dispatch_message_hostcom_send_reply_int(xep->dispatch, xep->route,XTS_SPCXI_PULSESPERSTEP,0,&pulseperstep_i32,1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetDownconversion(uint8_t downconversion, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_downconversion(xep->x4driver,downconversion);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverGetDownconversion(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    uint8_t downconversion;
    int32_t res = x4driver_get_downconversion(xep->x4driver, &downconversion);
    if (res == 0)
        dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route,XTS_SPCXI_DOWNCONVERSION,0,&downconversion,1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}
void onMcpX4DriverGetFrameBinCount(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    uint32_t bincount;
    int32_t res = x4driver_get_frame_bin_count(xep->x4driver, &bincount);
    if (res == 0)
        dispatch_message_hostcom_send_reply_int(xep->dispatch, xep->route,XTS_SPCXI_FRAMEBINCOUNT,0,&bincount,1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetFrameArea(float start, float end, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_frame_area(xep->x4driver, start, end);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverInit(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_init(xep->x4driver);

    // Set defaults

	x4driver_check_configuration(xep->x4driver);
	x4driver_set_sweep_trigger_control(xep->x4driver,SWEEP_TRIGGER_X4); // By default let sweep trigger control done by X4

    xt_radar_t default_config;
    xt_get_default_radar_config(&default_config);
    x4driver_set_frame_area_offset(xep->x4driver, default_config.frame_area_offset);
    x4driver_set_tx_power(xep->x4driver, default_config.tx_power);
    x4driver_set_prf_div(xep->x4driver, default_config.prf_div);
    x4driver_set_tx_center_frequency(xep->x4driver, default_config.center_frequency);

    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetDacStep(uint8_t dac_step, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_dac_step(xep->x4driver, dac_step);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}
void onMcpX4DriverGetDacStep(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    
    xtx4_dac_step_t dac_step=0;
    int32_t res = x4driver_get_dac_step(xep->x4driver, &dac_step);
	uint8_t short_dac_step=(uint8_t)dac_step;
    if (res == 0)
        dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route,XTS_SPCXI_DACSTEP,0,&short_dac_step,1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetDacMin(uint32_t dac_min, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_dac_min(xep->x4driver, dac_min);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}
void onMcpX4DriverGetDacMin(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    uint16_t dac_min=0;
    int32_t res = x4driver_get_dac_min(xep->x4driver, &dac_min);
    int32_t dac_min_i32=(int32_t)dac_min;
    if (res == 0)
        dispatch_message_hostcom_send_reply_int(xep->dispatch, xep->route,XTS_SPCXI_DACMIN,0,&dac_min_i32,1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetDacMax(uint32_t dac_max, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_dac_max(xep->x4driver, dac_max);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}
void onMcpX4DriverGetDacMax(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    uint16_t dac_max=0;
    int32_t res = x4driver_get_dac_max(xep->x4driver, &dac_max);
    int32_t dac_max_i32=(int32_t)dac_max;
    if (res == 0)
        dispatch_message_hostcom_send_reply_int(xep->dispatch, xep->route,XTS_SPCXI_DACMAX,0,&dac_max_i32,1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetFrameAreaOffset(float offset, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_frame_area_offset(xep->x4driver, offset);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetEnable(uint8_t enable, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_enable(xep->x4driver, enable);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetTxCenterFrequency(uint8_t tx_center_frequency, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_tx_center_frequency(xep->x4driver, tx_center_frequency);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverGetTxCenterFrequency(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    xtx4_tx_center_frequency_t tx_center_frequency;
    int32_t res = x4driver_get_tx_center_frequency(xep->x4driver, &tx_center_frequency);
    uint8_t tx_center_frequency_short=(uint8_t)tx_center_frequency;
    if (res == 0)
        dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route,XTS_SPCXI_TXCENTERFREQUENCY,0,&tx_center_frequency_short,1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetTxPower(uint8_t tx_power, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_tx_power(xep->x4driver, tx_power);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}
void onMcpX4DriverGetTxPower(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    xtx4_tx_power_t tx_power;
    int32_t res = x4driver_get_tx_power(xep->x4driver, &tx_power);
	uint8_t short_tx_power=(uint8_t)tx_power;
    if (res == 0)
        dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route,XTS_SPCXI_TXPOWER,0,&short_tx_power,1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverGetFps(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    float32_t fps;
    uint32_t res = x4driver_get_fps(xep->x4driver, &fps);
    if (res == 0)
        dispatch_message_hostcom_send_reply_float(xep->dispatch, xep->route, XTS_SPCXI_FPS, 0, &fps, 1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverWriteToSpiRegister(uint8_t address, const uint8_t* values, uint32_t length, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_write_to_spi_register(xep->x4driver, address, values, length);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverReadFromSpiRegister(uint8_t address, uint32_t length, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    uint8_t* values = (uint8_t*)xtmemory_malloc_slow(length);
    if (values == NULL)
    {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_COMMAND_FAILED);
        return;
    }

    int32_t res = x4driver_read_from_spi_register(xep->x4driver, address, values, length);
    if (res == 0)
        dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route, XTS_SPCXI_SPIREGISTER, 0, values, length);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
    
    xtmemory_free_slow(values);
}

void onMcpX4DriverSetSpiRegister(uint8_t address, uint8_t value, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_spi_register(xep->x4driver, address, value);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverGetSpiRegister(uint8_t address, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    uint8_t value;
    int32_t res = x4driver_get_spi_register(xep->x4driver, address, &value);
    if (res == 0)
        dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route, XTS_SPCXI_SPIREGISTER, 0, &value, 1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetPifRegister(uint8_t address, uint8_t value, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_pif_register(xep->x4driver, address, value);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverGetPifRegister(uint8_t address, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    uint8_t value;
    int32_t res = x4driver_get_pif_register(xep->x4driver, address, &value);
    if (res == 0)
        dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route, XTS_SPCXI_PIFREGISTER, 0, &value, 1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetXifRegister(uint8_t address, uint8_t value, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_xif_register(xep->x4driver, address, value);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverGetXifRegister(uint8_t address, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    uint8_t value;
    int32_t res = x4driver_get_xif_register(xep->x4driver, address, &value);
    if (res == 0)
        dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route, XTS_SPCXI_XIFREGISTER, 0, &value, 1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverSetPrfDiv(uint8_t prf_div, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = x4driver_set_prf_div(xep->x4driver, prf_div);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverGetPrfDiv(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    uint8_t prf_div;
    int32_t res = x4driver_get_prf_div(xep->x4driver, &prf_div);
    if (res == 0)
        dispatch_message_hostcom_send_reply_byte(xep->dispatch, xep->route, XTS_SPCXI_PRFDIV, 0, &prf_div, 1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverGetFrameArea(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    float32_t frame_area[2];
    int32_t res = x4driver_get_frame_area(xep->x4driver, &frame_area[0], &frame_area[1]);
    if (res == 0)
        dispatch_message_hostcom_send_reply_float(xep->dispatch, xep->route, XTS_SPCXI_FRAMEAREA, 0, frame_area, 2);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpX4DriverGetFrameAreaOffset(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    float32_t offset;
    int32_t res = x4driver_get_frame_area_offset(xep->x4driver, &offset);
    if (res == 0)
        dispatch_message_hostcom_send_reply_float(xep->dispatch, xep->route, XTS_SPCXI_FRAMEAREAOFFSET, 0, &offset, 1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpSetIOPinControl(uint32_t pin_id, uint32_t pin_setup, uint32_t pin_feature, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    if (pin_feature != XTID_IOPIN_FEATURE_PASSIVE) // For now, only support passive mode.
    {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_NOT_RECOGNIZED);
        return;
    }

    int32_t res = xtio_set_direction(pin_id, pin_setup, 0);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpSetIOPinValue(uint32_t pin_id, uint32_t pin_value, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    int32_t res = xtio_set_level(pin_id, pin_value);
    if (res == 0)
        dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}

void onMcpGetIOPinValue(uint32_t pin_id, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    xtio_pin_level_t pin_value=0;
    int32_t res = xtio_get_level(pin_id, &pin_value);
    int32_t value = (pin_value == XTIO_PIN_LEVEL_HIGH) ? 1 : 0;
    if (res == 0)
        dispatch_message_hostcom_send_reply_int(xep->dispatch, xep->route, XTS_SPCIOP_GETVALUE, 0, &value, 1);
    else
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, res);
}


void onMcpFindAllFiles(void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;
    uint32_t types[XFS_MAX_FILES];
    uint32_t identifiers[XFS_MAX_FILES];
    uint32_t number_of_entries = 0;

    const int result = xtfile_find_all_files(types, identifiers, XFS_MAX_FILES, &number_of_entries);
    if (result != XTFILE_OK) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    int32_t tmp[ 2 * XFS_MAX_FILES ];

    for(unsigned int i = 0; i < number_of_entries; ++i) {
        tmp[i] = types[i];
        tmp[i + number_of_entries] = identifiers[i];
    }

    dispatch_message_hostcom_send_reply_int(
        xep->dispatch, xep->route,
        0,
        0,
        tmp,
        2 * number_of_entries);
}



void onMcpOpenFile(uint32_t type, uint32_t identifier, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    void * handle = 0;
    int result = xtfile_open(
        &handle,
        type,
        identifier);

    if (result != XTFILE_OK) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
}


void onMcpCreateFile(uint32_t type, uint32_t identifier, uint32_t length, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    void * handle = 0;
    int result = xtfile_create_and_open_empty(
        &handle,
        type,
        identifier,
        length);

    /*dispatch_message_hostcom_send_data_string(xep->dispatch, 0, 0, "Hello there from %d\n", result);*/

    if (result != XTFILE_OK) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
}


static bool wait_and_confirm_file_delete(uint32_t type, uint32_t identifier)
{
    for (unsigned int i = 0; i < 10; ++i) {
        xt_wait(100);
        if(xtfile_delete(type, identifier) == XTFILE_FILE_NOT_FOUND)
            return true;
    }
    return false;
}


void onMcpFormatFilesystem(
    uint32_t key,
    void * user_data)
{
    const XepHandle_t * const  xep = (XepHandle_t*)user_data;
    const int result = xtfile_format(key);

    if (result != XTFILE_OK) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
}


void onMcpDeleteFile(
    uint32_t type,
    uint32_t identifier,
    void * user_data)
{
    const XepHandle_t * const  xep = (XepHandle_t*)user_data;
    const int result = xtfile_delete(type, identifier);

    if (result != XTFILE_OK) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    if (!wait_and_confirm_file_delete(type, identifier)) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, COMMAND_TIMEOUT);
        return;
    }

    dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
}


void onMcpCommitFile(uint32_t type, uint32_t identifier, uint32_t commit, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    void * handle = 0;
    int result = xtfile_get_handle(&handle, type, identifier);

    if ( result != XTFILE_OK ) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    result = xtfile_close(handle);

    if ( result != XTFILE_OK ) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
}


void onMcpGetFileLength(uint32_t type, uint32_t identifier, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    uint32_t size = 0;
    const int result = xtfile_get_size(type, identifier, &size);

    if ( result != XTFILE_OK ) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    dispatch_message_hostcom_send_reply_int(xep->dispatch, xep->route, 0, 0, (int32_t*)&size, 1);
}


void onMcpSearchFileType(uint32_t type, void * user_data)
{
    XepHandle_t* xep = (XepHandle_t*)user_data;

    uint32_t identifiers[ XFS_MAX_FILES ];
    uint32_t count = 0;
    const int result = xtfile_search_for_files_of_type(type, identifiers, XFS_MAX_FILES, &count);

    if ( result != XTFILE_OK ) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    dispatch_message_hostcom_send_reply_int(xep->dispatch, xep->route, 0, 0, (int32_t *)identifiers, count);
}


void onMcpSetFileData(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    const uint8_t * data,
    void * user_data)
{
    XepHandle_t * xep = (XepHandle_t*)user_data;
    void * handle = 0;
    uint32_t result = xtfile_get_handle(&handle, type, identifier);

    if ( result != XTFILE_OK ) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }


    uint8_t * buffer = 0;
    uint32_t size;

    result = xtfile_get_buffer(handle, (void**)&buffer, &size);
    if ( result != XTFILE_OK ) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    if (size < offset + length) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_COMMAND_FAILED);
        return;
    }

    result = xtfile_write(handle, data, offset, length);
	if (result != XTFILE_OK) {
		dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
		return;
	}

    dispatch_message_hostcom_send_ack(xep->dispatch, xep->route);
}

void onMcpGetFileData(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    void * user_data)
{
    XepHandle_t * xep = (XepHandle_t*)user_data;
    void * handle = 0;
    uint32_t result = xtfile_get_handle(&handle, type, identifier);

    if ( result != XTFILE_OK ) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    uint8_t * buffer = 0;
    uint32_t size;

    result = xtfile_get_buffer(handle, (void**)&buffer, &size);
    if ( result != XTFILE_OK ) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, result);
        return;
    }

    if (size < offset + length) {
        dispatch_message_hostcom_send_error(xep->dispatch, xep->route, XTS_SPRE_COMMAND_FAILED);
        return;
    }

    dispatch_message_hostcom_send_reply_byte(
        xep->dispatch, xep->route,
        0,
        0,
        &(buffer[offset]),
        length);
}


