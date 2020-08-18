#ifndef PROTOCOL_TARGET_H
#define PROTOCOL_TARGET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "protocol.h"

int createAckCommand(
    AppendCallback callback,
    void * user_data);

int createErrorCommand(
	int errorcode,
	AppendCallback callback,
	void * user_data);

int createPongCommand(
	int pongval,
	AppendCallback callback,
	void * user_data);

int createSystemCommand(
	uint32_t contentid,
	uint8_t* data,
	uint32_t length,
	AppendCallback callback,
	void * user_data);

int createAppdataSleepCommand(
    uint32_t counter,
    uint32_t stateCode,
    float stateData,
    float distance,
    uint32_t signalQuality,
    float movementSlow,
    float movementFast,
    AppendCallback callback,
    void * user_data);
    
int createAppdataVitalSignsCommand(
    uint32_t counter,
    uint32_t stateCode,
    float respiration_rate,
    float respiration_distance,
    float respiration_confidence,
    float heart_rate,
    float heart_distance,
    float heart_confidence,
    float normalized_movement_slow,
    float normalized_movement_fast,
    float normalized_movement_start,
    float normalized_movement_end,
    AppendCallback callback,
    void * user_data);

int createAppdataSleepStageCommand(
    uint32_t counter,
    uint32_t sleepstage,
    AppendCallback callback,
    void * user_data);
    
int createAppdataRespirationCommand(
    uint32_t counter,
    uint32_t stateCode,
    uint32_t stateData,
    float distance,
    float movement,
    uint32_t signalQuality,
    AppendCallback callback,
    void * user_data);

int createAppdataRespirationMovinglistCommand(
    uint32_t counter,
    uint32_t intervalCount,
    float * movementSlowItems,
    float * movementFastItems,
    AppendCallback callback,
    void * user_data);

int createAppdataRespirationDetectionlistCommand(
    uint32_t counter,
    uint32_t detectionCount,
    float * detectionDistanceItems,
    float * detectionRadarCrossSectionItems,
    float * detectionVelocityItems,
    AppendCallback callback,
    void * user_data);

int createAppdataRespirationNormalizedmovementlistCommand(
    uint32_t counter,
    float start,
    float binLength,
    uint32_t count,
    float *normalizedMovementSlowItems,
    float *normalizedMovementFastItems,
    AppendCallback callback,
    void * user_data);

int createAppdataPresenceSingleCommand(
    uint32_t counter,
    uint32_t presenceState,
    float distance,
    uint8_t direction,
    uint32_t signalQuality,
    AppendCallback callback,
    void * user_data);

int createAppdataPresenceMovinglistCommand(
    uint32_t counter,
    uint32_t presenceState,
    uint32_t movementIntervalCount,
    uint32_t detectionCount,
    float* movementSlowItem,
    float* movementFastItem,
    float* detectionDistance,
    float* detectionRadarCrossSection,
    float* detectionVelocity,
    AppendCallback callback,
    void * user_data);

int createAppdataBasebandAmplitudePhaseCommand(
    uint32_t counter,
    uint32_t numOfBins,
    float binLength,
    float samplingFrequency,
    float carrierFrequency,
    float rangeOffset,
    float* dataAmplitude,
    float* dataPhase,
    AppendCallback callback,
    void * user_data);

int createAppdataBasebandIQCommand(
    uint32_t counter,
    uint32_t numOfBins,
    float binLength,
    float samplingFrequency,
    float carrierFrequency,
    float rangeOffset,
    float* signalI,
    float* signalQ,
    AppendCallback callback,
    void * user_data);

int createAppdataProfileParameterFileCommand(
    uint32_t filename_length,
    uint32_t data_length,
    unsigned char * filename,
    unsigned char * data,
    AppendCallback callback,
    void * user_data);

int createAppdataPulseDopplerFloatCommand(
    uint32_t counter,
    uint32_t matrix_counter,
    uint32_t range_idx,
    uint32_t range_bins,
    uint32_t freq_count,
    uint32_t pd_instance,
    float fps,
    float fps_decimated,
    float freq_start,
    float freq_step,
    float range,
    const float * pwr,
    AppendCallback callback,
    void * user_data);

int createAppdataPulseDopplerFloatCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t * total_packet_length,
    uint32_t counter,
    uint32_t matrix_counter,
    uint32_t range_idx,
    uint32_t range_bins,
    uint32_t freq_count,
    uint32_t pd_instance,
    float fps,
    float fps_decimated,
    float freq_start,
    float freq_step,
    float range);

int createAppdataNoisemapFloatCommand(
    uint32_t counter,
    uint32_t matrix_counter,
    uint32_t range_idx,
    uint32_t range_bins,
    uint32_t freq_count,
    uint32_t pd_instance,
    float fps,
    float fps_decimated,
    float freq_start,
    float freq_step,
    float range,
    const float * pwr,
    AppendCallback callback,
    void * user_data);

int createAppdataNoisemapFloatCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t * total_packet_length,
    uint32_t counter,
    uint32_t matrix_counter,
    uint32_t range_idx,
    uint32_t range_bins,
    uint32_t freq_count,
    uint32_t pd_instance,
    float fps,
    float fps_decimated,
    float freq_start,
    float freq_step,
    float range);

int createAppdataPulseDopplerByteCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t * total_packet_length,
    uint32_t counter,
    uint32_t matrix_counter,
    uint32_t range_idx,
    uint32_t range_bins,
    uint32_t freq_count,
    uint32_t pd_instance,
    float step_start,
    float step_size,
    float fps,
    float fps_decimated,
    float freq_start,
    float freq_step,
    float range);

int createAppdataNoisemapByteCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t * total_packet_length,
    uint32_t counter,
    uint32_t matrix_counter,
    uint32_t range_idx,
    uint32_t range_bins,
    uint32_t freq_count,
    uint32_t pd_instance,
    float step_start,
    float step_size,
    float fps,
    float fps_decimated,
    float freq_start,
    float freq_step,
    float range);

int createDataFloatCommand(
	uint32_t contentid,
	uint32_t info,
	float* data,
	uint32_t length,
	AppendCallback callback,
	void * user_data);

int createDataByteCommand(
    uint32_t contentid,
    uint32_t info,
    uint8_t* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data);

int createDataByteCommandNoEscape(
    uint8_t **reverse_injector,
    uint8_t reverse_max_length,
    uint32_t *total_packet_length,
    uint32_t contentid,
    uint32_t info,
    uint32_t data_length);

int createDataStringnCommand(
	uint32_t contentid,
	uint32_t info,
	unsigned char* data,
	uint32_t length,
	AppendCallback callback,
	void * user_data);

int createDataUserCommand(
	uint32_t contentid,
	uint32_t info,
	void* data,
	uint32_t length,
	AppendCallback callback,
	void * user_data);

int createReplyByteCommand(
    uint32_t contentid,
    uint32_t info,
    const uint8_t * data,
    uint32_t length,
    AppendCallback callback,
	void * user_data);

int createReplyIntCommand(
    uint32_t contentid,
    uint32_t info,
    const int32_t * data,
    uint32_t length,
    AppendCallback callback,
    void * user_data);

int createReplyStringnCommand(
    uint32_t contentid,
    uint32_t info,
    const unsigned char* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data);

int createReplyFloatCommand(
    uint32_t contentid,
    uint32_t info,
    float* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data);

int createHilUpCommand(
    serial_protocol_response_datatype_t datatype, 
    serial_protocol_hil_t hil_command, 
    uint32_t info, 
    uint32_t length, 
    uint8_t* data, 
    uint8_t datasize,
    AppendCallback callback,
    void * user_data);


int createDataFloatCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t content_id,
    uint32_t frame_counter,
    uint32_t bin_count,
    uint32_t * packet_length);

int createAppdataBasebandIQCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t * total_packet_length,
    uint32_t counter,
    uint32_t num_of_bins,
    float bin_length,
    float sampling_frequency,
    float carrier_frequency,
    float range_offset);


int createAppdataBasebandAPCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t * total_packet_length,
    uint32_t counter,
    uint32_t num_of_bins,
    float bin_length,
    float sampling_frequency,
    float carrier_frequency,
    float range_offset);

int createParameterFileCommand(
    const char* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROTOCOL_TARGET_H */
