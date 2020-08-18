#ifndef PROTOCOL_PARSER_H
#define PROTOCOL_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Module messages
typedef void (*McpPingCallback)(uint32_t pongval, void * user_data);
typedef void (*McpStartBootloaderCallback)(uint32_t key, void * user_data);
typedef void (*McpSetLedControlCallback)(uint8_t mode, uint8_t intensity, void * user_data);
typedef void (*McpGetLedControlCallback)(void * user_data);
typedef void (*McpModuleResetCallback)(void * user_data);
typedef void (*McpResetToFactoryPresetCallback)(void * user_data);
typedef void (*McpSystemRunTestCallback)(uint8_t testcode, void * user_data);
typedef void (*McpGetSystemInfoCallback)(uint32_t contentid, uint8_t infocode, void * user_data);
typedef void (*McpSetDebugLevelCallback)(uint8_t debug_level, void * user_data);
typedef void (*McpGetSystemDebugDataCallback)(uint32_t contentid, uint8_t infocode, void * user_data);

typedef void (*McpInjectFrameCallback)(uint32_t frameCounter, float *frame, uint32_t frameLength, void * user_data);
typedef void (*McpPrepareInjectFrameCallback)(uint32_t num_frames, uint32_t num_bins, uint32_t mode,  void * user_data);


// Profile messages
typedef void (*McpSetSensorModeCallback)(uint8_t mode, uint8_t param, void * user_data);
typedef void (*McpGetSensorModeCallback)(void * user_data);
typedef void (*McpLoadProfileCallback)(uint32_t profileid, void * user_data);
typedef void (*McpProfileSetDetectionZoneCallback)(float start, float end, void * user_data);
typedef void (*McpProfileGetDetectionZoneCallback)(void * user_data);
typedef void (*McpProfileGetDetectionZoneLimitsCallback)(void * user_data);
typedef void (*McpProfileSetSensitivityCallback)(uint32_t sensitivity, void * user_data);
typedef void (*McpProfileGetSensitivityCallback)(void * user_data);
typedef void (*McpProfileSetTXCenterFrequencyCallback)(uint32_t frequencyBand, void * user_data);
typedef void (*McpProfileGetTXCenterFrequencyCallback)(void * user_data);
typedef void (*McpProfileSetParameterFileCallback)(
    uint32_t filename_length,
    uint32_t data_length,
    const char * filename,
    const char * data,
    void * user_data);
typedef void (*McpProfileGetParameterFileCallback)(
    uint32_t filename_length,
    const char * filename,
    void * user_data);

// Output control messages
typedef void (*McpSetOutputControlCallback)(uint32_t output_feature, uint32_t output_control, void * user_data);
typedef void (*McpGetOutputControlCallback)(uint32_t output_feature, void * user_data);
typedef void (*McpSetDebugOutputControlCallback)(uint32_t output_feature, uint32_t output_control, void * user_data);
typedef void (*McpGetDebugOutputControlCallback)(uint32_t output_feature, void * user_data);

// Noisemap control messages
typedef void (*McpSetNoisemapControlCallback)(uint32_t noisemap_control, void * user_data);
typedef void (*McpGetNoisemapControlCallback)(void * user_data);

// IOPin messages
typedef void (*McpSetIOPinControlCallback)(uint32_t pin_id, uint32_t pin_setup, uint32_t pin_feature, void * user_data);
typedef void (*McpGetIOPinControlCallback)(uint32_t pin_id, void * user_data);

typedef void (*McpSetIOPinValueCallback)(uint32_t pin_id, uint32_t pin_value, void * user_data);
typedef void (*McpGetIOPinValueCallback)(uint32_t pin_id, void * user_data);

// X4Driver messages
typedef void (*McpX4DriverSetFpsCallback)(float fps, void * user_data);
typedef void (*McpX4DriverSetIterationsCallback)(uint32_t iterations, void * user_data);
typedef void (*McpX4DriverGetIterationsCallback)(void * user_data);
typedef void (*McpX4DriverSetPulsesPerStepCallback)(uint32_t pps, void * user_data);
typedef void (*McpX4DriverGetPulsesPerStepCallback)(void * user_data);
typedef void (*McpX4DriverSetDownconversionCallback)(uint8_t downconversion, void * user_data);
typedef void (*McpX4DriverGetDownconversionCallback)(void * user_data);
typedef void (*McpX4DriverSetFrameAreaCallback)(float start, float end, void * user_data);
typedef void (*McpX4DriverInitCallback)(void * user_data);
typedef void (*McpX4DriverSetDacStepCallback)(uint8_t dac_step, void * user_data);
typedef void (*McpX4DriverGetDacStepCallback)(void * user_data);
typedef void (*McpX4DriverSetDacMinCallback)(uint32_t dac_min, void * user_data);
typedef void (*McpX4DriverGetDacMinCallback)(void * user_data);
typedef void (*McpX4DriverSetDacMaxCallback)(uint32_t dac_max, void * user_data);
typedef void (*McpX4DriverGetDacMaxCallback)(void * user_data);
typedef void (*McpX4DriverGetFrameBinCountCallback)(void * user_data);
typedef void (*McpX4DriverSetFrameAreaOffsetCallback)(float offset, void * user_data);
typedef void (*McpX4DriverSetEnableCallback)(uint8_t enable, void * user_data);
typedef void (*McpX4DriverSetTxCenterFrequencyCallback)(uint8_t tx_center_frequency, void * user_data);
typedef void (*McpX4DriverGetTxCenterFrequencyCallback)(void * user_data);
typedef void (*McpX4DriverSetTxPowerCallback)(uint8_t tx_power, void * user_data);
typedef void (*McpX4DriverGetFpsCallback)(void * user_data);
typedef void (*McpX4DriverSetSpiRegisterCallback)(uint8_t address, uint8_t value, void * user_data);
typedef void (*McpX4DriverGetSpiRegisterCallback)(uint8_t address, void * user_data);
typedef void (*McpX4DriverWriteToSpiRegisterCallback)(uint8_t address, const uint8_t * values, uint32_t length, void * user_data);
typedef void (*McpX4DriverReadFromSpiRegisterCallback)(uint8_t address, uint32_t length, void * user_data);
typedef void (*McpX4DriverSetPifRegisterCallback)(uint8_t address, uint8_t value, void * user_data);
typedef void (*McpX4DriverGetPifRegisterCallback)(uint8_t address, void * user_data);
typedef void (*McpX4DriverSetXifRegisterCallback)(uint8_t address, uint8_t value, void * user_data);
typedef void (*McpX4DriverGetXifRegisterCallback)(uint8_t address, void * user_data);
typedef void (*McpX4DriverSetPrfDivCallback)(uint8_t prf_div, void * user_data);
typedef void (*McpX4DriverGetPrfDivCallback)(void * user_data);
typedef void (*McpX4DriverGetFrameAreaCallback)(void * user_data);
typedef void (*McpX4DriverGetFrameAreaOffsetCallback)(void * user_data);
typedef void (*McpX4DriverGetTxPowerCallback)(void * user_data);
typedef void (*McpSetBaudRateCallback)(uint32_t, void * user_data);
typedef void (*McpStoreNoiseMap)(void * user_data);
typedef void (*McpLoadNoiseMap)(void * user_data);
typedef void (*McpDeleteNoiseMap)(void * user_data);
typedef void (*McpGetParameterFile)(uint32_t content_id, void * user_data);
typedef void (*McpGetCrashDumpLength)(void * user_data);
typedef void (*McpClearCrashDump)(void *user_data);
typedef void (*McpGetCrashDump)(uint32_t offset, uint32_t length, void * user_data);
typedef void (*McpSearchFileType)(uint32_t type, void * user_data);
typedef void (*McpFindAllFiles)(void * user_data);
typedef void (*McpCreateNewFile)(uint32_t type, uint32_t identifier, uint32_t length, void * user_data);
typedef void (*McpOpenFile)(uint32_t type, uint32_t identifier, void * user_data);
typedef void (*McpSetFileData)(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    const uint8_t * data,
    void * user_data);
typedef void (*McpCommitFile)(uint32_t type, uint32_t identifier, uint32_t commit, void * user_data);
typedef void (*McpDeleteFile)(uint32_t type, uint32_t identifier, void * user_data);
typedef void (*McpGetFileLength)(uint32_t type, uint32_t identifier, void * user_data);
typedef void (*McpGetFileData)(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    void * user_data);
typedef void (*McpFormatFilesystemCallback)(uint32_t key, void * user_data);
typedef void (*McpGetProfileIdCallback)(void * user_data);


// Unknown message
typedef void (*McpUnknownCallback)(const uint8_t* data, uint32_t length, void * user_data);


typedef struct
{
    McpPingCallback onMcpPing;
    McpStartBootloaderCallback onMcpStartBootloader;
    McpGetCrashDumpLength onMcpGetCrashDumpLength;
    McpClearCrashDump onMcpClearCrashDump;
    McpGetCrashDump onMcpGetCrashDump;
    McpSearchFileType onMcpSearchFileType;
    McpFindAllFiles onMcpFindAllFiles;
    McpCreateNewFile onMcpCreateNewFile;
    McpOpenFile onMcpOpenFile;
    McpCommitFile onMcpCommitFile;
    McpDeleteFile onMcpDeleteFile;
    McpSetFileData onMcpSetFileData;
    McpGetFileLength onMcpGetFileLength;
    McpGetFileData onMcpGetFileData;
    McpSetLedControlCallback onMcpSetLedControl;
    McpGetLedControlCallback onMcpGetLedControl;
    McpModuleResetCallback onMcpModuleReset;
    McpResetToFactoryPresetCallback onMcpResetToFactoryPreset;
    McpSystemRunTestCallback onMcpSystemRunTest;
    McpSetBaudRateCallback onMcpSetBaudrate;
    McpGetSystemInfoCallback onMcpGetSystemInfo;
    McpSetDebugLevelCallback onMcpSetDebugLevel;
    McpGetSystemDebugDataCallback onMcpGetSystemDebugData;
    McpFormatFilesystemCallback onMcpFormatFilesystem;

    McpInjectFrameCallback onMcpInjectFrame;
    McpPrepareInjectFrameCallback onMcpPrepareInjectFrame;    

    // Profile messages
    McpSetSensorModeCallback onMcpSetSensorMode;
    McpGetSensorModeCallback onMcpGetSensorMode;    
    McpLoadProfileCallback onMcpLoadProfile;
    McpGetProfileIdCallback onMcpGetProfileId;
    McpProfileSetDetectionZoneCallback onMcpProfileSetDetectionZone;
    McpProfileGetDetectionZoneCallback onMcpProfileGetDetectionZone;
    McpProfileGetDetectionZoneLimitsCallback onMcpProfileGetDetectionZoneLimits;
    McpProfileSetSensitivityCallback onMcpProfileSetSensitivity;
    McpProfileGetSensitivityCallback onMcpProfileGetSensitivity;
    McpProfileSetTXCenterFrequencyCallback onMcpProfileSetTXCenterFrequency;
    McpProfileGetTXCenterFrequencyCallback onMcpProfileGetTXCenterFrequency;
    McpProfileSetParameterFileCallback onMcpSetParameterFile;
    McpProfileGetParameterFileCallback onMcpGetParameterFile;

    // Output control messages
    McpSetOutputControlCallback onMcpSetOutputControl;
    McpGetOutputControlCallback onMcpGetOutputControl;
    McpSetDebugOutputControlCallback onMcpSetDebugOutputControl;
    McpGetDebugOutputControlCallback onMcpGetDebugOutputControl;

    // Noisemap control messages
    McpSetNoisemapControlCallback onMcpSetNoisemapControl;
    McpGetNoisemapControlCallback onMcpGetNoisemapControl;

    // IOPin messages
    McpSetIOPinControlCallback onMcpSetIOPinControl;
    McpGetIOPinControlCallback onMcpGetIOPinControl;
    McpSetIOPinValueCallback onMcpSetIOPinValue;
    McpGetIOPinValueCallback onMcpGetIOPinValue;

    // X4Driver messages
    McpX4DriverSetFpsCallback onMcpX4DriverSetFps;
    McpX4DriverSetIterationsCallback onMcpX4DriverSetIterations;
    McpX4DriverGetIterationsCallback onMcpX4DriverGetIterations;
    McpX4DriverSetPulsesPerStepCallback onMcpX4DriverSetPulsesPerStep;
    McpX4DriverGetPulsesPerStepCallback onMcpX4DriverGetPulsesPerStep;
    McpX4DriverSetDownconversionCallback onMcpX4DriverSetDownconversion;
    McpX4DriverGetDownconversionCallback onMcpX4DriverGetDownconversion;
    McpX4DriverSetFrameAreaCallback onMcpX4DriverSetFrameArea;
    McpX4DriverInitCallback onMcpX4DriverInit;
    McpX4DriverSetDacStepCallback onMcpX4DriverSetDacStep;
    McpX4DriverGetDacStepCallback onMcpX4DriverGetDacStep;
    McpX4DriverSetDacMinCallback onMcpX4DriverSetDacMin;
    McpX4DriverGetDacMinCallback onMcpX4DriverGetDacMin;
    McpX4DriverSetDacMaxCallback onMcpX4DriverSetDacMax;
    McpX4DriverGetDacMaxCallback onMcpX4DriverGetDacMax;
    McpX4DriverGetFrameBinCountCallback onMcpX4DriverGetFrameBinCount;
    McpX4DriverSetFrameAreaOffsetCallback onMcpX4DriverSetFrameAreaOffset;
    McpX4DriverSetEnableCallback onMcpX4DriverSetEnable;
    McpX4DriverSetTxCenterFrequencyCallback onMcpX4DriverSetTxCenterFrequency;
    McpX4DriverGetTxCenterFrequencyCallback onMcpX4DriverGetTxCenterFrequency;
    McpX4DriverSetTxPowerCallback onMcpX4DriverSetTxPower;
    McpX4DriverGetTxPowerCallback onMcpX4DriverGetTxPower;
    McpX4DriverGetFpsCallback onMcpX4DriverGetFps;
    McpX4DriverSetSpiRegisterCallback onMcpX4DriverSetSpiRegister;
    McpX4DriverGetSpiRegisterCallback onMcpX4DriverGetSpiRegister;
    McpX4DriverWriteToSpiRegisterCallback onMcpX4DriverWriteToSpiRegister;
    McpX4DriverReadFromSpiRegisterCallback onMcpX4DriverReadFromSpiRegister;
    McpX4DriverSetPifRegisterCallback onMcpX4DriverSetPifRegister;
    McpX4DriverGetPifRegisterCallback onMcpX4DriverGetPifRegister;
    McpX4DriverSetXifRegisterCallback onMcpX4DriverSetXifRegister;
    McpX4DriverGetXifRegisterCallback onMcpX4DriverGetXifRegister;
    McpX4DriverSetPrfDivCallback onMcpX4DriverSetPrfDiv;
    McpX4DriverGetPrfDivCallback onMcpX4DriverGetPrfDiv;
    McpX4DriverGetFrameAreaCallback onMcpX4DriverGetFrameArea;
    McpX4DriverGetFrameAreaOffsetCallback onMcpX4DriverGetFrameAreaOffset;
    McpStoreNoiseMap onMcpStoreNoiseMap;
    McpLoadNoiseMap onMcpLoadNoiseMap;
    McpDeleteNoiseMap onMcpDeleteNoiseMap;

    // Unknown message
    McpUnknownCallback onMcpUnknown;


    void * user_data;
}McpParser_t;


unsigned int getMcpParserInstanceSize(void);
McpParser_t *createMcpParser(void * user_data, void * instance_memory);
int mcpParseMessage(McpParser_t *mcp_parser, const uint8_t * data, uint32_t length);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROTOCOL_PARSER_H */
