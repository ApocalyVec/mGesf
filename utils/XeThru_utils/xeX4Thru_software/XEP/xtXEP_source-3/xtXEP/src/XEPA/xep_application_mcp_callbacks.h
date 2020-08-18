/**
 * @file
 *
 * @brief XEP Module application interface
 *
 * Primary setup of module implementation. Creates bridge to application layer.
 */

#ifndef XEP_APPLICATION_MCP_CALLBACKS_H
#define XEP_APPLICATION_MCP_CALLBACKS_H

#include <stdint.h>
#include "xep_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void onMcpPing(uint32_t pingval, void * user_data);
void onMcpUnknown(const uint8_t* data, uint32_t length, void * user_data);
void onMcpModuleReset(void * user_data);
void onMcpSystemRunTest(uint8_t testcode, void * user_data);
void onMcpStartBootloader(uint32_t key, void * user_data);
void onMcpGetSystemInfo(uint32_t contentid, uint8_t infocode, void * user_data);
void onMcpGetCrashDumpLength(void * user_data);
void onMcpGetCrashDump(uint32_t offset, uint32_t length, void * user_data);
void onMcpClearCrashDump(void * user_data);
void onMcpSetBaudrate(uint32_t baudrate, void * user_data);

void onMcpX4DriverSetFps(float32_t fps, void * user_data);
void onMcpX4DriverSetIterations(uint32_t iterations, void * user_data);
void onMcpX4DriverGetIterations( void * user_data);
void onMcpX4DriverSetPulsesPerStep(uint32_t pulsesperstep, void * user_data);
void onMcpX4DriverGetPulsesPerStep( void * user_data);
void onMcpX4DriverSetDownconversion(uint8_t downconversion, void * user_data);
void onMcpX4DriverGetDownconversion(void * user_data);
void onMcpX4DriverSetFrameArea(float start, float end, void * user_data);
void onMcpX4DriverInit(void *user_data);
void onMcpX4DriverSetDacStep(uint8_t dac_step, void * user_data);
void onMcpX4DriverGetDacStep( void * user_data);
void onMcpX4DriverSetDacMin(uint32_t dac_min, void * user_data);
void onMcpX4DriverGetDacMin(void * user_data);
void onMcpX4DriverSetDacMax(uint32_t dac_max, void * user_data);
void onMcpX4DriverGetDacMax(void * user_data);
void onMcpX4DriverGetFrameBinCount(void * user_data);
void onMcpX4DriverSetFrameAreaOffset(float offset, void * user_data);
void onMcpX4DriverSetEnable(uint8_t enable, void * user_data);
void onMcpX4DriverSetTxCenterFrequency(uint8_t tx_center_frequency, void * user_data);
void onMcpX4DriverGetTxCenterFrequency( void * user_data);
void onMcpX4DriverSetTxPower(uint8_t tx_power, void * user_data);
void onMcpX4DriverGetTxPower(void * user_data);
void onMcpX4DriverGetFps(void * user_data);
void onMcpX4DriverSetSpiRegister(uint8_t address, uint8_t value, void * user_data);
void onMcpX4DriverGetSpiRegister(uint8_t address, void * user_data);
void onMcpX4DriverWriteToSpiRegister(uint8_t address, const uint8_t* values, uint32_t length, void * user_data);
void onMcpX4DriverReadFromSpiRegister(uint8_t address, uint32_t length, void * user_data);
void onMcpX4DriverSetPifRegister(uint8_t address, uint8_t value, void * user_data);
void onMcpX4DriverGetPifRegister(uint8_t address, void * user_data);
void onMcpX4DriverSetXifRegister(uint8_t address, uint8_t value, void * user_data);
void onMcpX4DriverGetXifRegister(uint8_t address, void * user_data);
void onMcpX4DriverSetPrfDiv(uint8_t prf_div, void * user_data);
void onMcpX4DriverGetPrfDiv(void * user_data);
void onMcpX4DriverGetFrameArea(void * user_data);
void onMcpX4DriverGetFrameAreaOffset(void * user_data);
void onMcpSetIOPinControl(uint32_t pin_id, uint32_t pin_setup, uint32_t pin_feature, void * user_data);
void onMcpSetIOPinValue(uint32_t pin_id, uint32_t pin_value, void * user_data);
void onMcpGetIOPinValue(uint32_t pin_id, void * user_data);

void onMcpFindAllFiles(void * user_data);
void onMcpSearchFileType(uint32_t type, void * user_data);
void onMcpCreateFile(uint32_t file_type, uint32_t identifier, uint32_t length, void * user_data);
void onMcpOpenFile(uint32_t file_type, uint32_t identifier, void * user_data);
void onMcpDeleteFile(uint32_t file_type, uint32_t identifier, void * user_data);
void onMcpFormatFilesystem(uint32_t key, void * user_data);
void onMcpCommitFile(uint32_t file_type, uint32_t identifier, uint32_t commit, void * user_data);
void onMcpGetFileLength(uint32_t type, uint32_t identifier, void * user_data);
void onMcpSetFileData(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    const uint8_t * data,
    void * user_data);

void onMcpGetFileData(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    void * user_data);


#ifdef __cplusplus
}
#endif

#endif // XEP_APPLICATION_MCP_CALLBACKS_H
