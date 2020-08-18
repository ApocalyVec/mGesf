#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "xtserial_definitions.h"

typedef struct xtProtocol xtProtocol;

typedef void (*PacketHandlerFunc)(
    void * user_data,
    const unsigned char * bytes,
    unsigned int length);

typedef void (*ErrorHandlerFunc)(
    void * user_data,
    unsigned int length);


unsigned int getInstanceSize(void);

xtProtocol * createApplicationProtocol(
    PacketHandlerFunc packetHandler,
    ErrorHandlerFunc errorHandler,
    void * user_data,
    void * instance_memory,
    unsigned char * buffer,
    unsigned int max_length);

void destroyProtocol(xtProtocol * instance);


/* int createCommand( */
/*     const unsigned char * src, */
/*     unsigned int length, */
/*     unsigned char * destination, */
/*     unsigned int max_length); */

typedef void (*AppendCallback)(unsigned char byte, void * user_data);


int createCommand(
    const unsigned char * src,
    unsigned int length,
    AppendCallback append_callback,
    void * user_data);

int parseData(
    xtProtocol * instance,
    const unsigned char * buffer,
    unsigned int length);


int createSetSensorModeCommand(
    sensor_mode_t mode,
    unsigned char data,
    AppendCallback callback,
    void * user_data);

int createGetSensorModeCommand(
    AppendCallback callback,
    void * user_data);        

int createPingCommand(
    AppendCallback callback,
    void * user_data);


int createStartBootloaderCommand(
    uint32_t key,
    AppendCallback callback,
    void * user_data);

int createBootloaderDeviceInfoCommand(
    AppendCallback callback,
    void * user_data);

int createLoadAppCommand(
    uint32_t app_id,
    AppendCallback callback,
    void * user_data);

int createSetDetectionZoneCommand(
    float start,
    float end,
    AppendCallback callback,
    void * user_data);

int createSetSensitivityCommand(
    const uint32_t sensitivity,
    AppendCallback callback,
    void * user_data);

int createGetSensitivityCommand(
    AppendCallback callback,
    void * user_data);

int createSetTxCenterFrequencyCommand(
    const uint32_t frequencyBand,
    AppendCallback callback,
    void * user_data);

int createGetTxCenterFrequencyCommand(
    AppendCallback callback,
    void * user_data);

int createSetParameterFileCommand(
    uint32_t filename_length,
    uint32_t data_length,
    const char * filename,
    const char * data,
    AppendCallback callback,
    void * user_data);

int createGetParameterFileCommand(
    uint32_t filename_length,
    const char * filename,
    AppendCallback callback,
    void * user_data);

int createModuleResetCommand(
    AppendCallback callback,
    void * user_data);

int createResetToFactoryPreset(
    AppendCallback callback,
    void * user_data);

int createSetLedControlCommand(
    unsigned char mode,
    unsigned char intensity,
    AppendCallback callback,
    void * user_data);

int createGetLedControlCommand(
    AppendCallback callback,
    void * user_data);

int createSetBaudRateCommand(
    int baudrate,
    AppendCallback callback,
    void * user_data);

int createSystemGetVersionCommand(
    AppendCallback callback,
    void * user_data);

int createSystemGetInfoCommand(
    unsigned char info_code,
    AppendCallback callback,
    void * user_data);

int createSystemRunTest(
    const unsigned char test_code,
    AppendCallback callback,
    void * user_data);
    
int createInjectFrameCommand(
    uint32_t frame_counter,
    uint32_t frame_length,
    const float * frame,
    AppendCallback callback,
    void * user_data);
    
int createPrepareInjectFrameCommand(
    uint32_t num_frames,
    uint32_t num_bins,
    uint32_t mode,
    AppendCallback callback,
    void * user_data);       

int createAppCommand(
    unsigned char app_command,
    const unsigned char * data,
    unsigned int length,
    AppendCallback callback,
    void * user_data);

int createAppSetIntCommand(
    int reference,
    const int * data,
    unsigned int data_length,
    AppendCallback callback,
    void * user_data);



int createConfigSetIntCommand(
    int reference,
    int data,
    AppendCallback callback,
    void * user_data);


int createConfigSetFloatCommand(
    int reference,
    float data,
    AppendCallback callback,
    void * user_data);



int createDebugLevelCommand(
    unsigned char debug_level,
    AppendCallback callback,
    void * user_data);

int createPageWriteCommand(
    uint16_t page_address,
    const unsigned char * page_data,
    unsigned int page_length,
    AppendCallback callback,
    void * user_data);


int createApplicationStartCommand(
    AppendCallback callback,
    void * user_data);

int createSetRegisterCommand(
    const uint32_t id,
    const uint32_t value,
    AppendCallback callback,
    void * user_data);

int createGetRegisterCommand(
    const uint32_t id,
    AppendCallback callback,
    void * user_data);

int createGetIntCommand(
    uint32_t id,
    AppendCallback callback,
    void * user_data);

int createSetIntCommand(
    uint32_t id,
    uint32_t value,
    AppendCallback callback,
    void * user_data);

int createRunTimingMeasurementCommand(
    AppendCallback callback,
    void * user_data);

int createSetFloatCommand(
    const uint32_t id,
    const float value,
    AppendCallback callback,
    void * user_data);

int createGetFloatCommand(
    const uint32_t id,
    AppendCallback callback,
    void * user_data);

int createGetFrame(
    AppendCallback callback,
    void * user_data);

int createGetSystemInfoCommand(
    const unsigned char info_code,
    AppendCallback callback,
    void * user_data);

int createX4DriverSetFpsCommand(const float fps,
    AppendCallback callback,
    void * user_data);


int createX4DriverSetIterationsCommand(
    const uint32_t iterations,
    AppendCallback callback,
    void * user_data);
    
int createX4DriverGetIterationsCommand(
    AppendCallback callback,
    void * user_data);

int createX4DriverSetPulsesPerStepCommand(
    const uint32_t pulsesperstep,
    AppendCallback callback,
    void * user_data);

int createX4DriverGetPulsesPerStepCommand(
    AppendCallback callback,
    void * user_data);


int createX4DriverSetDownconversionCommand(
    const uint8_t downconversion,
    AppendCallback callback,
    void * user_data);

int createX4DriverGetDownconversionCommand(
    AppendCallback callback,
    void * user_data);
int createX4DriverGetFrameBinCountCommand(
    AppendCallback callback,
    void * user_data);
int createX4DriverSetFrameAreaCommand(
    const float start,
    const float end,
    AppendCallback callback,
    void * user_data);

int createX4DriverInitCommand(
    AppendCallback callback,
    void * user_data);

int createX4DriverSetDacStepCommand(
    const uint8_t dac_step,
    AppendCallback callback,
    void * user_data);
    
int createX4DriverGetDacStepCommand(
    AppendCallback callback,
    void * user_data);

int createX4DriverSetDacMinCommand(
    const uint32_t dac_min,
    AppendCallback callback,
    void * user_data);

int createX4DriverGetDacMinCommand(
    AppendCallback callback,
    void * user_data);

int createX4DriverSetDacMaxCommand(
    const uint32_t dac_max,
    AppendCallback callback,
    void * user_data);

int createX4DriverGetDacMaxCommand(
    AppendCallback callback,
    void * user_data);

int createX4DriverSetFrameAreaOffsetCommand(
    const float offset,
    AppendCallback callback,
    void * user_data);

int createX4DriverSetEnableCommand(
    const uint8_t enable,
    AppendCallback callback,
    void * user_data);

int createX4DriverSetTxCenterFrequencyCommand(
    const uint8_t tx_center_frequency,
    AppendCallback callback,
    void * user_data);
int createX4DriverGetTxCenterFrequencyCommand(
    AppendCallback callback,
    void * user_data);

int createX4DriverSetTxPowerCommand(
    const uint8_t tx_power,
    AppendCallback callback,
    void * user_data);
    
int createX4DriverGetTxPowerCommand(
    AppendCallback callback,
    void * user_data);

int createX4DriverGetFpsCommand(
    AppendCallback callback,
    void * user_data);

int createX4DriverSetSpiRegisterCommand(
    const uint8_t address,
    const uint8_t value,
    AppendCallback callback,
    void * user_data);

int createX4DriverGetSpiRegisterCommand(const uint8_t address,
    AppendCallback callback,
    void * user_data);

int createX4DriverWriteToSpiRegisterCommand(
    const uint8_t address,
    const uint8_t *values,
    const uint32_t length,
    AppendCallback callback,
    void * user_data);

int createX4DriverReadFromSpiRegisterCommand(const uint8_t address,
    const uint32_t length,
    AppendCallback callback,
    void * user_data);

int createX4DriverSetPifRegisterCommand(
    const uint8_t address,
    const uint8_t value,
    AppendCallback callback,
    void * user_data);

int createX4DriverGetPifRegisterCommand(const uint8_t address,
    AppendCallback callback,
    void * user_data);

int createX4DriverSetXifRegisterCommand(
    const uint8_t address,
    const uint8_t value,
    AppendCallback callback,
    void * user_data);

int createX4DriverGetXifRegisterCommand(const uint8_t address,
    AppendCallback callback,
    void * user_data);

int createX4DriverSetPrfDivCommand(
    const uint8_t prf_div,
    AppendCallback callback,
    void * user_data);

int createX4DriverGetPrfDivCommand(AppendCallback callback,
    void * user_data);

int createX4DriverGetFrameAreaCommand(
    AppendCallback callback,
    void * user_data);

int createX4DriverGetFrameAreaOffsetCommand(
    AppendCallback callback,
    void * user_data);

int createSetIOPinControlCommand(
    const uint32_t pin_id,
    const uint32_t pin_setup,
    const uint32_t pin_feature,
    AppendCallback callback,
    void * user_data);

int createGetIOPinControlCommand(
    const uint32_t pin_id,
    AppendCallback callback,
    void * user_data);    

int createSetIOPinValueCommand(
    const uint32_t pin_id,
    const uint32_t pin_value,
    AppendCallback callback,
    void * user_data);

int createGetIOPinValueCommand(
    const uint32_t pin_id,
    AppendCallback callback,
    void * user_data);

int createLoadProfileCommand(
    uint32_t profile_id,
    AppendCallback callback,
    void * user_data);

int createGetDetectionZoneCommand(
    AppendCallback callback,
    void * user_data);

int createGetDetectionZoneLimitsCommand(
    AppendCallback callback,
    void * user_data);

int createSetOutputControlCommand(
    const uint32_t output_feature,
    const uint32_t output_control,
    AppendCallback callback,
    void * user_data);

int createGetOutputControlCommand(
    const uint32_t output_feature,    
    AppendCallback callback,
    void * user_data); 

int createSetDebugOutputControlCommand(
    const uint32_t output_feature,
    const uint32_t output_control,
    AppendCallback callback,
    void * user_data);

int createGetDebugOutputControlCommand(
    const uint32_t output_feature,    
    AppendCallback callback,
    void * user_data);        

int createSetBaudRate(
    const uint32_t baudrate,
    AppendCallback callback,
    void * user_data);

int createStoreNoiseMapCommand(
    AppendCallback callback,
    void * user_data);

int createLoadNoiseMapCommand(
    AppendCallback callback,
    void * user_data);
    
int createDeleteNoiseMapCommand(
    AppendCallback callback,
    void * user_data);

int createSetNoiseMapControlCommand(
    const uint32_t control,
    AppendCallback callback,
    void * user_data);

int createGetNoiseMapControlCommand(
    AppendCallback callback,
    void * user_data);

int createSearchForFileTypeCommand(
    uint32_t type,
    AppendCallback callback,
    void * user_data);

int createFindAllFilesCommand(
    AppendCallback callback,
    void * user_data);

int createNewFileCommand(
    uint32_t file_type,
    uint32_t identifier,
    uint32_t length,
    AppendCallback callback,
    void * user_data);

int createOpenFileCommand(
    uint32_t file_type,
    uint32_t identifier,
    AppendCallback callback,
    void * user_data);

int createSetFileDataCommand(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    const uint8_t * data,
    AppendCallback callback,
    void * user_data);

int createCloseFileCommand(
    uint32_t type,
    uint32_t identifier,
    uint32_t commit,
    AppendCallback callback,
    void * user_data);

int createGetFileLengthCommand(
    uint32_t type,
    uint32_t identifier,
    AppendCallback callback,
    void * user_data);

int createDeleteFileCommand(
    uint32_t type,
    uint32_t identifier,
    AppendCallback callback,
    void * user_data);

int createGetFileDataCommand(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    AppendCallback callback,
    void * user_data);


int createFormatFilesystemCommand(
    uint32_t key,
    AppendCallback callback,
    void * user_data);


int createGetProfileIdCommand(
    AppendCallback callback,
    void * user_data);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROTOCOL_H */
