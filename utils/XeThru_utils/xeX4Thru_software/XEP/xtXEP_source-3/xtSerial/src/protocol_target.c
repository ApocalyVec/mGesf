#include "protocol_target.h"
#include "protocol_helpers.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


static void assign_uint32(uint8_t * destination, const uint32_t value)
{
    *destination = (value >> 0 & 0xff);
    ++destination;
    *destination = (value >> 8 & 0xff);
    ++destination;
    *destination = (value >> 16 & 0xff);
    ++destination;
    *destination = (value >> 24 & 0xff);
}

int createAckCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_ACK, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createErrorCommand(
    int errorcode,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_ERROR, callback, user_data);
    process_int(errorcode, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createPongCommand(
    int pongval,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_PONG, callback, user_data);
    process_int(pongval, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createSystemCommand(
    uint32_t contentid,
    uint8_t* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_SYSTEM, callback, user_data);
    process_int(contentid, callback, user_data);
    process_bytes(data, length, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createAppdataSleepCommand(
    uint32_t counter,
    uint32_t stateCode,
    float stateData,
    float distance,
    uint32_t signalQuality,
    float movementSlow,
    float movementFast,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_SLEEP_STATUS, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(stateCode, callback, user_data);
    process_float(stateData, callback, user_data);
    process_float(distance, callback, user_data);
    process_int(signalQuality, callback, user_data);
    process_float(movementSlow, callback, user_data);
    process_float(movementFast, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
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
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_VITAL_SIGNS, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(stateCode, callback, user_data);
    process_float(respiration_rate, callback, user_data);
    process_float(respiration_distance, callback, user_data);
    process_float(respiration_confidence, callback, user_data);
    process_float(heart_rate, callback, user_data);
    process_float(heart_distance, callback, user_data);
    process_float(heart_confidence, callback, user_data);
    process_float(normalized_movement_slow, callback, user_data);
    process_float(normalized_movement_fast, callback, user_data);
    process_float(normalized_movement_start, callback, user_data);
    process_float(normalized_movement_end, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createAppdataSleepStageCommand(
    uint32_t counter,
    uint32_t sleepstage,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_SLEEPSTAGE, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(sleepstage, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createAppdataRespirationCommand(
    uint32_t counter,
    uint32_t stateCode,
    uint32_t stateData,
    float distance,
    float movement,
    uint32_t signalQuality,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_RESP_STATUS, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(stateCode, callback, user_data);
    process_int(stateData, callback, user_data);
    process_float(distance, callback, user_data);
    process_float(movement, callback, user_data);
    process_int(signalQuality, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createAppdataRespirationMovinglistCommand(
    uint32_t counter,
    uint32_t intervalCount,
    float *movementSlowItems,
    float *movementFastItems,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_RESPIRATION_MOVINGLIST, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(intervalCount, callback, user_data);
    process_floats(movementSlowItems, intervalCount, callback, user_data);
    process_floats(movementFastItems, intervalCount, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createAppdataRespirationDetectionlistCommand(
    uint32_t counter,
    uint32_t detectionCount,
    float *detectionDistanceItems,
    float *detectionRadarCrossSectionItems,
    float *detectionVelocityItems,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_RESPIRATION_DETECTIONLIST, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(detectionCount, callback, user_data);
    process_floats(detectionDistanceItems, detectionCount, callback, user_data);
    process_floats(detectionRadarCrossSectionItems, detectionCount, callback, user_data);
    process_floats(detectionVelocityItems, detectionCount, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createAppdataRespirationNormalizedmovementlistCommand(
    uint32_t counter,
    float start,
    float binLength,
    uint32_t count,
    float *normalizedMovementSlowItems,
    float *normalizedMovementFastItems,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_RESPIRATION_NORMALIZEDMOVEMENTLIST, callback, user_data);
    process_int(counter, callback, user_data);
    process_float(start, callback, user_data);
    process_float(binLength, callback, user_data);
    process_int(count, callback, user_data);
    process_floats(normalizedMovementSlowItems, count, callback, user_data);
    process_floats(normalizedMovementFastItems, count, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createAppdataPresenceSingleCommand(
    uint32_t counter,
    uint32_t presenceState,
    float distance,
    uint8_t direction,
    uint32_t signalQuality,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_PRESENCE_SINGLE, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(presenceState, callback, user_data);
    process_float(distance, callback, user_data);
    process_byte(direction, callback, user_data);
    process_int(signalQuality, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

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
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_PRESENCE_MOVINGLIST, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(presenceState, callback, user_data);
    process_int(movementIntervalCount, callback, user_data);
    process_int(detectionCount, callback, user_data);
    process_floats(movementSlowItem, movementIntervalCount, callback, user_data);
    process_floats(movementFastItem, movementIntervalCount, callback, user_data);
    process_floats(detectionDistance, detectionCount, callback, user_data);
    process_floats(detectionRadarCrossSection, detectionCount, callback, user_data);
    process_floats(detectionVelocity, detectionCount, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

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
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_BASEBAND_AMPLITUDE_PHASE, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(numOfBins, callback, user_data);
    process_float(binLength, callback, user_data);
    process_float(samplingFrequency, callback, user_data);
    process_float(carrierFrequency, callback, user_data);
    process_float(rangeOffset, callback, user_data);
    process_floats(dataAmplitude, numOfBins, callback, user_data);
    process_floats(dataPhase, numOfBins, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}



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
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_BASEBAND_IQ, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(numOfBins, callback, user_data);
    process_float(binLength, callback, user_data);
    process_float(samplingFrequency, callback, user_data);
    process_float(carrierFrequency, callback, user_data);
    process_float(rangeOffset, callback, user_data);
    process_floats(signalI, numOfBins, callback, user_data);
    process_floats(signalQ, numOfBins, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createAppdataProfileParameterFileCommand(
    uint32_t filename_length,
    uint32_t data_length,
    unsigned char * filename,
    unsigned char * data,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_PROFILE_PARAMETERFILE, callback, user_data);
    process_int(filename_length, callback, user_data);
    process_int(data_length, callback, user_data);
    process_bytes(filename, filename_length, callback, user_data);
    process_bytes(data, data_length, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

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
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_PULSEDOPPLER_FLOAT, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(matrix_counter, callback, user_data);
    process_int(range_idx, callback, user_data);
    process_int(range_bins, callback, user_data);
    process_int(freq_count, callback, user_data);
    process_int(pd_instance, callback, user_data);
    process_float(fps, callback, user_data);
    process_float(fps_decimated, callback, user_data);
    process_float(freq_start, callback, user_data);
    process_float(freq_step, callback, user_data);
    process_float(range, callback, user_data);
    process_floats(pwr, freq_count, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

static int _createAppdataPDMatCommandNoEscape(
    uint8_t **reverse_injector,
    uint8_t reverse_max_length,
    uint32_t *total_packet_length,
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
    uint32_t datatype)
{
    const uint32_t no_escape_header_length =
        sizeof(uint32_t) + // XTS_FLAGSEQUENCE_START_NOESCAPE
        sizeof(uint32_t) + // packet length
        1; // crc

    const uint32_t packet_header_length =
        1 + // XTS_SPR_APPDATA
        sizeof(datatype) +
        sizeof(counter) +
        sizeof(matrix_counter) +
        sizeof(range_idx) +
        sizeof(range_bins) +
        sizeof(freq_count) +
        sizeof(pd_instance) +
        sizeof(fps) +
        sizeof(fps_decimated) +
        sizeof(freq_start) +
        sizeof(freq_step) +
        sizeof(range);

    const uint32_t header_length = no_escape_header_length + packet_header_length;
    const uint32_t length = packet_header_length + freq_count * sizeof(float);

    if (reverse_max_length < header_length)
        return 1;

    uint8_t *start = *reverse_injector - header_length;
    uint8_t *injector = start;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#define PUT(injector, type, x) \
        do { *((type*)injector) = x; injector += sizeof(type); } while(0)

    PUT(injector, uint32_t, XTS_FLAGSEQUENCE_START_NOESCAPE);
    PUT(injector, uint32_t, length);
    PUT(injector, uint8_t, 0); // unused xor
    PUT(injector, uint8_t, XTS_SPR_APPDATA);
    PUT(injector, uint32_t, datatype);
    PUT(injector, uint32_t, counter);
    PUT(injector, uint32_t, matrix_counter);
    PUT(injector, uint32_t, range_idx);
    PUT(injector, uint32_t, range_bins);
    PUT(injector, uint32_t, freq_count);
    PUT(injector, uint32_t, pd_instance);
    PUT(injector, float, fps);
    PUT(injector, float, fps_decimated);
    PUT(injector, float, freq_start);
    PUT(injector, float, freq_step);
    PUT(injector, float, range);

#undef PUT
#pragma GCC diagnostic pop

    assert(injector == *reverse_injector);
    *reverse_injector = start;
    *total_packet_length = no_escape_header_length + length;
    return 0;
}

static int _createAppdataPDMatByteCommandNoEscape(
    uint8_t **reverse_injector,
    uint8_t reverse_max_length,
    uint32_t *total_packet_length,
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
    float range,
    uint32_t datatype)
{
    const uint32_t no_escape_header_length =
        sizeof(uint32_t) + // XTS_FLAGSEQUENCE_START_NOESCAPE
        sizeof(uint32_t) + // packet length
        1; // crc

    const uint32_t packet_header_length =
        1 + // XTS_SPR_APPDATA
        sizeof(datatype) +
        sizeof(counter) +
        sizeof(matrix_counter) +
        sizeof(range_idx) +
        sizeof(range_bins) +
        sizeof(freq_count) +
        sizeof(pd_instance) +
        sizeof(step_start) +
        sizeof(step_size) +
        sizeof(fps) +
        sizeof(fps_decimated) +
        sizeof(freq_start) +
        sizeof(freq_step) +
        sizeof(range);

    const uint32_t header_length = no_escape_header_length + packet_header_length;
    const uint32_t length = packet_header_length + freq_count * sizeof(uint8_t);

    if (reverse_max_length < header_length)
        return 1;

    uint8_t *start = *reverse_injector - header_length;
    uint8_t *injector = start;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#define PUT(injector, type, x) \
        do { *((type*)injector) = x; injector += sizeof(type); } while(0)

    PUT(injector, uint32_t, XTS_FLAGSEQUENCE_START_NOESCAPE);
    PUT(injector, uint32_t, length);
    PUT(injector, uint8_t, 0); // unused xor
    PUT(injector, uint8_t, XTS_SPR_APPDATA);
    PUT(injector, uint32_t, datatype);
    PUT(injector, uint32_t, counter);
    PUT(injector, uint32_t, matrix_counter);
    PUT(injector, uint32_t, range_idx);
    PUT(injector, uint32_t, range_bins);
    PUT(injector, uint32_t, freq_count);
    PUT(injector, uint32_t, pd_instance);
    PUT(injector, float, step_start);
    PUT(injector, float, step_size);
    PUT(injector, float, fps);
    PUT(injector, float, fps_decimated);
    PUT(injector, float, freq_start);
    PUT(injector, float, freq_step);
    PUT(injector, float, range);

#undef PUT
#pragma GCC diagnostic pop

    assert(injector == *reverse_injector);
    *reverse_injector = start;
    *total_packet_length = no_escape_header_length + length;
    return 0;
}

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
    float range)
{
    return _createAppdataPDMatCommandNoEscape(reverse_injector,
            reverse_max_length, total_packet_length, counter, matrix_counter,
            range_idx, range_bins, freq_count, pd_instance, fps, fps_decimated,
            freq_start, freq_step, range, XTS_ID_PULSEDOPPLER_FLOAT);
}

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
    float range)
{
    return _createAppdataPDMatByteCommandNoEscape(reverse_injector,
            reverse_max_length, total_packet_length, counter, matrix_counter,
            range_idx, range_bins, freq_count, pd_instance, step_start,
            step_size, fps, fps_decimated, freq_start, freq_step, range,
            XTS_ID_PULSEDOPPLER_BYTE);
}

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
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_APPDATA, callback, user_data);
    process_int(XTS_ID_NOISEMAP_FLOAT, callback, user_data);
    process_int(counter, callback, user_data);
    process_int(matrix_counter, callback, user_data);
    process_int(range_idx, callback, user_data);
    process_int(range_bins, callback, user_data);
    process_int(freq_count, callback, user_data);
    process_int(pd_instance, callback, user_data);
    process_float(fps, callback, user_data);
    process_float(fps_decimated, callback, user_data);
    process_float(freq_start, callback, user_data);
    process_float(freq_step, callback, user_data);
    process_float(range, callback, user_data);
    process_floats(pwr, freq_count, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

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
    float range)
{
    return _createAppdataPDMatCommandNoEscape(reverse_injector,
            reverse_max_length, total_packet_length, counter, matrix_counter,
            range_idx, range_bins, freq_count, pd_instance, fps, fps_decimated,
            freq_start, freq_step, range, XTS_ID_NOISEMAP_FLOAT);
}

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
    float range)
{
    return _createAppdataPDMatByteCommandNoEscape(reverse_injector,
            reverse_max_length, total_packet_length, counter, matrix_counter,
            range_idx, range_bins, freq_count, pd_instance, step_start,
            step_size, fps, fps_decimated, freq_start, freq_step, range,
            XTS_ID_NOISEMAP_BYTE);
}

int createDataFloatCommand(
    uint32_t contentid,
    uint32_t info,
    float* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_DATA, callback, user_data);
    process_byte(XTS_SPRD_FLOAT, callback, user_data);
    process_int(contentid, callback, user_data);
    process_int(info, callback, user_data);
    if (length>0)
    {
        process_int(length, callback, user_data);
        process_floats(data, length, callback, user_data);
    }
    packet_end(callback, user_data);
    return 0;
}

int createDataByteCommand(
    uint32_t contentid,
    uint32_t info,
    uint8_t* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_DATA, callback, user_data);
    process_byte(XTS_SPRD_BYTE, callback, user_data);
    process_int(contentid, callback, user_data);
    process_int(info, callback, user_data);
    if (length>0)
    {
        process_int(length, callback, user_data);
        process_bytes(data, length, callback, user_data);
    }
    packet_end(callback, user_data);
    return 0;
}

int createDataByteCommandNoEscape(
    uint8_t **reverse_injector,
    uint8_t reverse_max_length,
    uint32_t *total_packet_length,
    uint32_t contentid,
    uint32_t info,
    uint32_t data_length)
{
    const uint32_t no_escape_header_length =
        sizeof(uint32_t) + // XTS_FLAGSEQUENCE_START_NOESCAPE
        sizeof(uint32_t) + // packet length
        1; // crc

    const uint32_t packet_header_length =
        1 + // XTS_SPR_APPDATA
        1 + // XTS_SPRD_BYTE
        sizeof(contentid) +
        sizeof(info) +
        sizeof(data_length);

    const uint32_t header_length = no_escape_header_length + packet_header_length;
    const uint32_t packet_length = packet_header_length + data_length;

    if (reverse_max_length < header_length)
        return 1;

    uint8_t *start = *reverse_injector - header_length;
    uint8_t *injector = start;

    assign_uint32(injector, XTS_FLAGSEQUENCE_START_NOESCAPE);
    injector += sizeof(uint32_t);
    assign_uint32(injector, packet_length);
    injector += sizeof(packet_length);
    *injector++ = 0; // unused xor
    *injector++ = XTS_SPR_DATA;
    *injector++ = XTS_SPRD_BYTE;
    assign_uint32(injector, contentid);
    injector += sizeof(contentid);
    assign_uint32(injector, info);
    injector += sizeof(info);
    assign_uint32(injector, data_length);
    injector += sizeof(data_length);

    assert(injector == *reverse_injector);

    *reverse_injector = start;
    *total_packet_length = no_escape_header_length + packet_length;
    return 0;
}

int createDataStringnCommand(
    uint32_t contentid,
    uint32_t info,
    unsigned char* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_DATA, callback, user_data);
    process_byte(XTS_SPRD_STRING, callback, user_data);
    process_int(contentid, callback, user_data);
    process_int(info, callback, user_data);
    if (length>0)
    {
        process_int(length, callback, user_data);
        process_bytes(data, length, callback, user_data);
    }
    packet_end(callback, user_data);
    return 0;
}

int createDataUserCommand(
    uint32_t contentid,
    uint32_t info,
    void* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_DATA, callback, user_data);
    process_byte(XTS_SPRD_USER, callback, user_data);
    process_int(contentid, callback, user_data);
    process_int(info, callback, user_data);
    if (length>0)
    {
        process_int(length, callback, user_data);
        process_bytes((unsigned char*)data, length, callback, user_data);
    }
    packet_end(callback, user_data);
    return 0;
}

int createReplyIntCommand(
    uint32_t contentid,
    uint32_t info,
    const int32_t* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_REPLY, callback, user_data);
    process_byte(XTS_SPRD_INT, callback, user_data);
    process_uint(contentid, callback, user_data);
    process_uint(info, callback, user_data);
    if (length>0)
    {
        process_uint(length, callback, user_data);
        process_ints((int*)data, length, callback, user_data);
    }
    packet_end(callback, user_data);
    return 0;
}

int createReplyByteCommand(
    uint32_t contentid,
    uint32_t info,
    const uint8_t * data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_REPLY, callback, user_data);
    process_byte(XTS_SPRD_BYTE, callback, user_data);
    process_uint(contentid, callback, user_data);
    process_uint(info, callback, user_data);
    if (length>0)
    {
        process_uint(length, callback, user_data);
        process_bytes(data, length, callback, user_data);
    }
    process_byte(sizeof(unsigned char), callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createReplyStringnCommand(
    uint32_t contentid,
    uint32_t info,
    const unsigned char * data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_REPLY, callback, user_data);
    process_byte(XTS_SPRD_STRING, callback, user_data);
    process_uint(contentid, callback, user_data);
    process_uint(info, callback, user_data);
    if (length>0)
    {
        process_uint(length, callback, user_data);
        process_bytes(data, length, callback, user_data);
    }
    process_byte(sizeof(char), callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createReplyFloatCommand(
    uint32_t contentid,
    uint32_t info,
    float* data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_REPLY, callback, user_data);
    process_byte(XTS_SPRD_FLOAT, callback, user_data);
    process_uint(contentid, callback, user_data);
    process_uint(info, callback, user_data);
    if (length>0)
    {
        process_uint(length, callback, user_data);
        process_floats(data, length, callback, user_data);
    }
    process_byte(sizeof(float), callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createHilUpCommand(
    serial_protocol_response_datatype_t datatype,
    serial_protocol_hil_t hil_command,
    uint32_t info,
    uint32_t length,
    uint8_t* data,
    uint8_t datasize,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPR_HIL, callback, user_data);
    process_byte(datatype, callback, user_data);
    process_int((uint32_t)hil_command, callback, user_data);
    process_int(info, callback, user_data);
    if (length>0)
    {
        process_int(length, callback, user_data);
        process_bytes(data, length*datasize, callback, user_data);
    }
    packet_end(callback, user_data);
    return 0;
}



static void assign_float(uint8_t * destination, const float value)
{
    float * dest = (float*)(destination);
    *dest = value;
}

int createAppdataBasebandAPCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t * total_packet_length,
    uint32_t counter,
    uint32_t num_of_bins,
    float bin_length,
    float sampling_frequency,
    float carrier_frequency,
    float range_offset)
{
    const uint32_t no_escape_header_length =
        sizeof(uint32_t) + // XTS_FLAGSEQUENCE_START_NOESCAPE
        sizeof(uint32_t) + // packet length
        1; // crc

    const uint32_t packet_header_length =
        1 + // XTS_SPR_APPDATA
        sizeof(uint32_t) + // XTS_ID_BASEBAND_AP
        sizeof(counter) +
        sizeof(num_of_bins) +
        sizeof(bin_length) +
        sizeof(sampling_frequency) +
        sizeof(carrier_frequency) +
        sizeof(range_offset);

    const uint32_t header_length = no_escape_header_length + packet_header_length;

    if(reverse_max_length < header_length)
        return 1;

    uint8_t * const  start = *reverse_injector - header_length;
    uint8_t * injector = *reverse_injector - header_length;
    assign_uint32(injector, XTS_FLAGSEQUENCE_START_NOESCAPE);
    injector += sizeof(uint32_t);
    const uint32_t length = packet_header_length + 2 * num_of_bins * sizeof(float);
    assign_uint32(injector, length);
    injector += sizeof(length);
    const uint8_t unused_xor = 0;
    *injector = unused_xor;
    ++injector;
    *injector = XTS_SPR_APPDATA;
    ++injector;

    assign_uint32(injector, XTS_ID_BASEBAND_AMPLITUDE_PHASE);
    injector += sizeof(uint32_t);
    assign_uint32(injector, counter);
    injector += sizeof(uint32_t);
    assign_uint32(injector, num_of_bins);
    injector += sizeof(num_of_bins);
    assign_float(injector, bin_length);
    injector += sizeof(bin_length);
    assign_float(injector, sampling_frequency);
    injector += sizeof(sampling_frequency);
    assign_float(injector, carrier_frequency);
    injector += sizeof(carrier_frequency);
    assign_float(injector, range_offset);
    injector += sizeof(range_offset);

    *reverse_injector = start;
    *total_packet_length = no_escape_header_length + length;
    return 0;
    return 0;
}

int createAppdataBasebandIQCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t * total_packet_length,
    uint32_t counter,
    uint32_t num_of_bins,
    float bin_length,
    float sampling_frequency,
    float carrier_frequency,
    float range_offset)
{
    const uint32_t no_escape_header_length =
        sizeof(uint32_t) + // XTS_FLAGSEQUENCE_START_NOESCAPE
        sizeof(uint32_t) + // packet length
        1; // crc

    const uint32_t packet_header_length =
        1 + // XTS_SPR_APPDATA
        sizeof(uint32_t) + // XTS_ID_BASEBAND_IQ
        sizeof(counter) +
        sizeof(num_of_bins) +
        sizeof(bin_length) +
        sizeof(sampling_frequency) +
        sizeof(carrier_frequency) +
        sizeof(range_offset);

    const uint32_t header_length = no_escape_header_length + packet_header_length;

    if(reverse_max_length < header_length)
        return 1;

    uint8_t * const  start = *reverse_injector - header_length;
    uint8_t * injector = *reverse_injector - header_length;
    assign_uint32(injector, XTS_FLAGSEQUENCE_START_NOESCAPE);
    injector += sizeof(uint32_t);
    const uint32_t length = packet_header_length + 2 * num_of_bins * sizeof(float);
    assign_uint32(injector, length);
    injector += sizeof(length);
    const uint8_t unused_xor = 0;
    *injector = unused_xor;
    ++injector;
    *injector = XTS_SPR_APPDATA;
    ++injector;

    assign_uint32(injector, XTS_ID_BASEBAND_IQ);
    injector += sizeof(uint32_t);
    assign_uint32(injector, counter);
    injector += sizeof(uint32_t);
    assign_uint32(injector, num_of_bins);
    injector += sizeof(num_of_bins);
    assign_float(injector, bin_length);
    injector += sizeof(bin_length);
    assign_float(injector, sampling_frequency);
    injector += sizeof(sampling_frequency);
    assign_float(injector, carrier_frequency);
    injector += sizeof(carrier_frequency);
    assign_float(injector, range_offset);
    injector += sizeof(range_offset);

    *reverse_injector = start;
    *total_packet_length = no_escape_header_length + length;
    return 0;
}


int createDataFloatCommandNoEscape(
    uint8_t ** reverse_injector,
    uint8_t reverse_max_length,
    uint32_t content_id,
    uint32_t frame_counter,
    uint32_t bin_count,
    uint32_t * packet_length)
{
    const uint32_t no_escape_header_length =
        sizeof(uint32_t) + // XTS_FLAGSEQUENCE_START_NOESCAPE
        sizeof(uint32_t) + // packet length
        1; // crc

    const uint32_t packet_header_length =
        2 * 1 + // XTS_SPR_DATA + XTS_SPRD_FLOAT
        sizeof(frame_counter) +
        sizeof(content_id) +
        sizeof(bin_count);

    const uint32_t header_length = no_escape_header_length + packet_header_length;

    if(reverse_max_length < header_length)
        return 1;

    uint8_t * const  start = *reverse_injector - header_length;
    uint8_t * injector = *reverse_injector - header_length;

    assign_uint32(injector, XTS_FLAGSEQUENCE_START_NOESCAPE);
    injector += sizeof(uint32_t);
    const uint32_t float_data_length = bin_count * sizeof(float);
    *packet_length = header_length + float_data_length;
    const uint32_t length = packet_header_length + float_data_length;
    assign_uint32(injector, length);
    injector += sizeof(uint32_t);
    *injector = 0; //crc
    ++injector;
    *injector = XTS_SPR_DATA;
    ++injector;
    *injector = XTS_SPRD_FLOAT;
    ++injector;
    /* *injector = ID_RAW_FRAME_BUFFER; */
    /* ++injector; */
    assign_uint32(injector, content_id);
    injector += sizeof(content_id);
    assign_uint32(injector, frame_counter);
    injector += sizeof(frame_counter);
    assign_uint32(injector, bin_count);

    *reverse_injector = start;
    return 0;
}


int createParameterFileCommand(
    const char * data,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    return createReplyStringnCommand(
        XTS_SDC_GET_PARAMETER_FILE,
        0,
        (unsigned char *)data,
        length,
        callback,
        user_data);
}



#ifdef __cplusplus
}
#endif /* __cplusplus */


