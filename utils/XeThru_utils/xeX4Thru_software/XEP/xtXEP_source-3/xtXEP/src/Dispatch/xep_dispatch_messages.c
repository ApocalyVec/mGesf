/**
 * @file
 *
 * 
 */

#include "xep_dispatch_messages.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

uint32_t dispatch_message_hostcom_send_ack(XepDispatch_t* dispatch, uint32_t route)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomAck_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAck_t* message_content = (XepDispatchMessageContentHostcomAck_t*)memoryblock->buffer;
	message_content->common.content_ref = XDMCR_HOSTCOM_ACK;
	message_content->common.message_size = size;
	uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_error(XepDispatch_t* dispatch, uint32_t route, uint32_t errorcode)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomError_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomError_t* message_content = (XepDispatchMessageContentHostcomError_t*)memoryblock->buffer;
	message_content->common.content_ref = XDMCR_HOSTCOM_ERROR;
	message_content->common.message_size = size;
	message_content->errorcode = errorcode;
	uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_pong(XepDispatch_t* dispatch, uint32_t route, uint32_t pongval)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomPong_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomPong_t* message_content = (XepDispatchMessageContentHostcomPong_t*)memoryblock->buffer;
	message_content->common.content_ref = XDMCR_HOSTCOM_PONG;
	message_content->common.message_size = size;
	message_content->pongval = pongval;
	uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_appdata_sleep(XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t state_code, float state_data, float distance, uint32_t signal_quality, float movement_slow, float movement_fast)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomAppdataSleep_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataSleep_t* message_content = (XepDispatchMessageContentHostcomAppdataSleep_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_SLEEP;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->state_code = state_code;
    message_content->state_data = state_data;
    message_content->distance = distance;
    message_content->signal_quality = signal_quality;
    message_content->movement_slow = movement_slow;
    message_content->movement_fast = movement_fast;
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}
uint32_t dispatch_message_hostcom_send_appdata_vitalsigns(XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t state_code, float respiration_rate, float respiration_distance, float respiration_confidence, float heart_rate, float heart_distance, float heart_confidence, float movement_power_slow, float movement_power_fast, float movement_power_start, float movement_power_end)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomAppdataVitalSigns_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataVitalSigns_t* message_content = (XepDispatchMessageContentHostcomAppdataVitalSigns_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_VITALSIGNS; 
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->state_code = state_code;
    message_content->respiration_rate = respiration_rate;
    message_content->respiration_distance = respiration_distance;
    message_content->respiration_confidence = respiration_confidence;
    message_content->heart_rate = heart_rate;
    message_content->heart_distance = heart_distance;
    message_content->heart_confidence = heart_confidence;
    message_content->movement_power_slow = movement_power_slow;
    message_content->movement_power_fast = movement_power_fast;
    message_content->movement_power_start = movement_power_start;
    message_content->movement_power_end = movement_power_end;
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}
uint32_t dispatch_message_hostcom_send_appdata_respiration(XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t state_code, uint32_t state_data, float distance, float movement, uint32_t signal_quality)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomAppdataRespiration_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataRespiration_t* message_content = (XepDispatchMessageContentHostcomAppdataRespiration_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_RESPIRATION;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->state_code = state_code;
    message_content->state_data = state_data;
    message_content->distance = distance;
    message_content->movement = movement;
    message_content->signal_quality = signal_quality;
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_appdata_respiration_movinglist(
    XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t interval_count,
    const float *movement_slow_item, const float *movement_fast_item)
{
    uint32_t size =
        sizeof(XepDispatchMessageContentHostcomAppdataRespirationMovinglist_t) +
        2 * interval_count * sizeof(float);
    MemoryBlock_t *memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataRespirationMovinglist_t *message_content =
        (XepDispatchMessageContentHostcomAppdataRespirationMovinglist_t *)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_RESPIRATION_MOVINGLIST;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->interval_count = interval_count;
    message_content->movement_slow_item =
        (float*) ((uint32_t)(void*)message_content +
                  sizeof(XepDispatchMessageContentHostcomAppdataRespirationMovinglist_t));
    message_content->movement_fast_item =
        (float*) ((uint32_t)(void*)message_content +
                  sizeof(XepDispatchMessageContentHostcomAppdataRespirationMovinglist_t) +
                  interval_count*sizeof(float));
    memcpy(message_content->movement_slow_item, movement_slow_item,
            interval_count * sizeof(float));
    memcpy(message_content->movement_fast_item, movement_fast_item,
            interval_count * sizeof(float));
    return dispatch_send_message(
            NULL, dispatch, route, memoryblock,
            message_content->common.message_size);
}

uint32_t dispatch_message_hostcom_send_appdata_respiration_detectionlist(
    XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t detection_count,
    const float *detection_distance, const float *detection_rcs,
    const float *detection_velocity)
{
    uint32_t size =
        sizeof(XepDispatchMessageContentHostcomAppdataRespirationMovinglist_t) +
        3 * detection_count * sizeof(float);
    MemoryBlock_t *memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataRespirationDetectionlist_t *message_content =
        (XepDispatchMessageContentHostcomAppdataRespirationDetectionlist_t *)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_RESPIRATION_DETECTIONLIST;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->detection_count = detection_count;
    message_content->detection_distance_items =
        (float*) ((uint32_t)(void*)message_content +
                  sizeof(XepDispatchMessageContentHostcomAppdataRespirationDetectionlist_t));
    message_content->detection_radar_cross_section_items =
        (float*) ((uint32_t)(void*)message_content +
                  sizeof(XepDispatchMessageContentHostcomAppdataRespirationDetectionlist_t) +
                  detection_count*sizeof(float));
    message_content->detection_velocity_items =
        (float*) ((uint32_t)(void*)message_content +
                  sizeof(XepDispatchMessageContentHostcomAppdataRespirationDetectionlist_t) +
                  2*detection_count*sizeof(float));
    memcpy(message_content->detection_distance_items, detection_distance,
            detection_count * sizeof(float));
    memcpy(message_content->detection_radar_cross_section_items, detection_rcs,
            detection_count * sizeof(float));
    memcpy(message_content->detection_velocity_items, detection_velocity,
            detection_count * sizeof(float));
    return dispatch_send_message(
            NULL, dispatch, route, memoryblock,
            message_content->common.message_size);
}

uint32_t dispatch_message_hostcom_send_appdata_respiration_normalizedmovementlist(
    XepDispatch_t* dispatch, uint32_t route, uint32_t frame_counter,
    float start, float bin_length, uint32_t count,
    const float *normalized_movement_slow_items,
    const float *normalized_movement_fast_items)
{
    const uint32_t structsize =
        sizeof(XepDispatchMessageContentHostcomAppdataRespirationNormalizedMovementlist_t);
    uint32_t size = structsize + 2 * count * sizeof(float);

    MemoryBlock_t *memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataRespirationNormalizedMovementlist_t *message_content =
        (XepDispatchMessageContentHostcomAppdataRespirationNormalizedMovementlist_t *)memoryblock->buffer;

    message_content->common.content_ref =
        XDMCR_HOSTCOM_APPDATA_RESPIRATION_NORMALIZEDMOVEMENTLIST;
    message_content->common.message_size = size;
    message_content->frame_counter = frame_counter;
    message_content->start = start;
    message_content->bin_length = bin_length;
    message_content->count = count;

    message_content->normalized_movement_slow_items =
        (float*)((uintptr_t)message_content + structsize);
    message_content->normalized_movement_fast_items =
        (float*)((uintptr_t)message_content + structsize + count*sizeof(float));

    memcpy(message_content->normalized_movement_slow_items,
           normalized_movement_slow_items, count * sizeof(float));
    memcpy(message_content->normalized_movement_fast_items,
           normalized_movement_fast_items, count * sizeof(float));

    return dispatch_send_message(
            NULL, dispatch, route, memoryblock,
            message_content->common.message_size);
}

uint32_t dispatch_message_hostcom_send_appdata_presence_single(XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t presence_state, float distance, uint8_t direction, uint32_t signal_quality)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomAppdataPresenceSingle_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataPresenceSingle_t* message_content = (XepDispatchMessageContentHostcomAppdataPresenceSingle_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_PRESENCE_SINGLE;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->presence_state = presence_state;
    message_content->distance = distance;
    message_content->direction = direction;
    message_content->signal_quality = signal_quality;
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_appdata_presence_movinglist(XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t presence_state, uint32_t movement_interval_count, uint32_t detection_count, const float* movement_slow_item, const float* movement_fast_item, const float* detection_distance, const float* detection_radar_cross_section, const float* detection_velocity)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomAppdataPresenceMovinglist_t) + movement_interval_count * sizeof(float) * 2 + detection_count * sizeof(float) * 3;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataPresenceMovinglist_t* message_content = (XepDispatchMessageContentHostcomAppdataPresenceMovinglist_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_PRESENCE_MOVINGLIST;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->presence_state = presence_state;
    message_content->movement_interval_count = movement_interval_count;
    message_content->detection_count = detection_count;
    message_content->movement_slow_item = (float*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomAppdataPresenceMovinglist_t) );
    message_content->movement_fast_item = (float*) ( (uint32_t)(void*)message_content->movement_slow_item + movement_interval_count * sizeof(float) );
    message_content->detection_distance = (float*) ( (uint32_t)(void*)message_content->movement_fast_item + movement_interval_count * sizeof(float) );
    message_content->detection_radar_cross_section = (float*) ( (uint32_t)(void*)message_content->detection_distance + detection_count * sizeof(float) );
    message_content->detection_velocity = (float*) ( (uint32_t)(void*)message_content->detection_radar_cross_section + detection_count * sizeof(float) );
    memcpy(message_content->movement_slow_item, movement_slow_item, movement_interval_count * sizeof(float));
    memcpy(message_content->movement_fast_item, movement_fast_item, movement_interval_count * sizeof(float));
    memcpy(message_content->detection_distance, detection_distance, detection_count * sizeof(float));
    memcpy(message_content->detection_radar_cross_section, detection_radar_cross_section, detection_count * sizeof(float));
    memcpy(message_content->detection_velocity, detection_velocity, detection_count * sizeof(float));
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_appdata_baseband_amplitude_phase(XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t num_of_bins, float bin_length, float sampling_frequency, float carrier_frequency, float range_offset, const float* data_amplitude, const float* data_phase)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomAppdataBasebandAmplitudePhase_t) + num_of_bins * sizeof(float) * 2;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataBasebandAmplitudePhase_t* message_content = (XepDispatchMessageContentHostcomAppdataBasebandAmplitudePhase_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_BASEBAND_AMPLITUDE_PHASE;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->num_of_bins = num_of_bins;
    message_content->bin_length = bin_length;
    message_content->sampling_frequency = sampling_frequency;
    message_content->carrier_frequency = carrier_frequency;
    message_content->range_offset = range_offset;
    message_content->data_amplitude = (float*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomAppdataBasebandAmplitudePhase_t) );
    message_content->data_phase = (float*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomAppdataBasebandAmplitudePhase_t) + num_of_bins * sizeof(float) );
    memcpy(message_content->data_amplitude, data_amplitude, num_of_bins * sizeof(float));
    memcpy(message_content->data_phase, data_phase, num_of_bins * sizeof(float));
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_appdata_baseband_iq(XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t num_of_bins, float bin_length, float sampling_frequency, float carrier_frequency, float range_offset, const float* signal_i, const float* signal_q)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomAppdataBasebandIQ_t) + num_of_bins * sizeof(float) * 2;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataBasebandIQ_t* message_content = (XepDispatchMessageContentHostcomAppdataBasebandIQ_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_BASEBAND_IQ;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->num_of_bins = num_of_bins;
    message_content->bin_length = bin_length;
    message_content->sampling_frequency = sampling_frequency;
    message_content->carrier_frequency = carrier_frequency;
    message_content->range_offset = range_offset;
    message_content->signal_i = (float*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomAppdataBasebandIQ_t) );
    message_content->signal_q = (float*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomAppdataBasebandIQ_t) + num_of_bins * sizeof(float) );
    memcpy(message_content->signal_i, signal_i, num_of_bins * sizeof(float));
    memcpy(message_content->signal_q, signal_q, num_of_bins * sizeof(float));
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_appdata_profileparameterfile(XepDispatch_t* dispatch, uint32_t route, uint32_t filename_length, uint32_t data_length, const char * filename, const char * data)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomAppdataProfileParameterFile_t) + filename_length + data_length;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataProfileParameterFile_t* message_content = (XepDispatchMessageContentHostcomAppdataProfileParameterFile_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_PROFILEPARAMETERFILE;
    message_content->common.message_size = size;
    message_content->filename_length = filename_length;
    message_content->data_length = data_length;
    message_content->filename = (unsigned char *) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomAppdataProfileParameterFile_t) );
    message_content->data = (unsigned char *) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomAppdataProfileParameterFile_t) + filename_length );
    memcpy(message_content->filename, filename, filename_length);
    memcpy(message_content->data, data, data_length);
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_appdata_pulsedoppler_float(
        XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t matrix_counter,
        uint32_t range_idx, uint32_t range_bins, uint32_t freq_count, uint32_t
        pd_instance, float fps, float fps_decimated, float freq_start, float
        freq_step, float range, const float * pwr)
{
    uint32_t size =
        sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t) +
        freq_count * sizeof(float);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t* message_content =
        (XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_PULSEDOPPLER_FLOAT;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->matrix_counter = matrix_counter;
    message_content->range_idx = range_idx;
    message_content->range_bins = range_bins;
    message_content->freq_count = freq_count;
    message_content->pd_instance = pd_instance;
    message_content->fps = fps;
    message_content->fps_decimated = fps_decimated;
    message_content->freq_start = freq_start;
    message_content->freq_step = freq_step;
    message_content->range = range;
    message_content->pwr = (float*) ((uint32_t)(void*)message_content +
            sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t));
    memcpy(message_content->pwr, pwr, freq_count * sizeof(float));
    return dispatch_send_message(
            NULL, dispatch, route, memoryblock,
            message_content->common.message_size);
}

uint32_t dispatch_message_hostcom_send_appdata_noisemap_float(
        XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t matrix_counter,
        uint32_t range_idx, uint32_t range_bins, uint32_t freq_count, uint32_t
        pd_instance, float fps, float fps_decimated, float freq_start, float
        freq_step, float range, const float * pwr)
{
    uint32_t size =
        sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t) +
        freq_count * sizeof(float);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t* message_content =
        (XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_NOISEMAP_FLOAT;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->matrix_counter = matrix_counter;
    message_content->range_idx = range_idx;
    message_content->range_bins = range_bins;
    message_content->freq_count = freq_count;
    message_content->pd_instance = pd_instance;
    message_content->fps = fps;
    message_content->fps_decimated = fps_decimated;
    message_content->freq_start = freq_start;
    message_content->freq_step = freq_step;
    message_content->range = range;
    message_content->pwr = (float*) ((uint32_t)(void*)message_content +
            sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerFloat_t));
    memcpy(message_content->pwr, pwr, freq_count * sizeof(float));
    return dispatch_send_message(
            NULL, dispatch, route, memoryblock,
            message_content->common.message_size);
}

uint32_t dispatch_message_hostcom_send_appdata_pulsedoppler_byte(
        XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t matrix_counter,
        uint32_t range_idx, uint32_t range_bins, uint32_t freq_count,
        uint32_t pd_instance, float step_start, float step_size, float fps,
        float fps_decimated, float freq_start, float freq_step, float range,
        const uint8_t * pwr)
{
    uint32_t size =
        sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t) +
        freq_count * sizeof(uint8_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t* message_content =
        (XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_PULSEDOPPLER_BYTE;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->matrix_counter = matrix_counter;
    message_content->range_idx = range_idx;
    message_content->range_bins = range_bins;
    message_content->freq_count = freq_count;
    message_content->pd_instance = pd_instance;
    message_content->step_start = step_start;
    message_content->step_size = step_size;
    message_content->fps = fps;
    message_content->fps_decimated = fps_decimated;
    message_content->freq_start = freq_start;
    message_content->freq_step = freq_step;
    message_content->range = range;
    message_content->pwr = (uint8_t*) ((uint32_t)(void*)message_content +
            sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t));
    memcpy(message_content->pwr, pwr, freq_count * sizeof(uint8_t));
    return dispatch_send_message(
            NULL, dispatch, route, memoryblock,
            message_content->common.message_size);
}

uint32_t dispatch_message_hostcom_send_appdata_noisemap_byte(
        XepDispatch_t* dispatch, uint32_t route, uint32_t counter, uint32_t matrix_counter,
        uint32_t range_idx, uint32_t range_bins, uint32_t freq_count,
        uint32_t pd_instance, float step_start, float step_size, float fps,
        float fps_decimated, float freq_start, float freq_step, float range,
        const uint8_t * pwr)
{
    uint32_t size =
        sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t) +
        freq_count * sizeof(uint8_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t* message_content =
        (XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_APPDATA_NOISEMAP_BYTE;
    message_content->common.message_size = size;
    message_content->counter = counter;
    message_content->matrix_counter = matrix_counter;
    message_content->range_idx = range_idx;
    message_content->range_bins = range_bins;
    message_content->freq_count = freq_count;
    message_content->pd_instance = pd_instance;
    message_content->step_start = step_start;
    message_content->step_size = step_size;
    message_content->fps = fps;
    message_content->fps_decimated = fps_decimated;
    message_content->freq_start = freq_start;
    message_content->freq_step = freq_step;
    message_content->range = range;
    message_content->pwr = (uint8_t*) ((uint32_t)(void*)message_content +
            sizeof(XepDispatchMessageContentHostcomAppdataPulseDopplerByte_t));
    memcpy(message_content->pwr, pwr, freq_count * sizeof(uint8_t));
    return dispatch_send_message(
            NULL, dispatch, route, memoryblock,
            message_content->common.message_size);
}

uint32_t dispatch_message_hostcom_send_system(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomSystem_t);
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomSystem_t* message_content = (XepDispatchMessageContentHostcomSystem_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_SYSTEM;
    message_content->common.message_size = size;
    message_content->contentid = contentid;
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_data_string(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid, uint32_t info, const char* format, ...)
{
    char buf[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, 256, format, args);
    va_end(args);
    int length = strlen(buf);
    return dispatch_message_hostcom_send_data_stringn(dispatch, route, contentid, info, (unsigned char *)buf, length);
}

uint32_t dispatch_message_hostcom_send_data_stringn(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid, uint32_t info, unsigned char* data, uint32_t length)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomDataString_t) + length;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomDataString_t* message_content = (XepDispatchMessageContentHostcomDataString_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_DATA_STRING;
    message_content->common.message_size = size;
    message_content->contentid = contentid;
    message_content->info = info;
    message_content->length = length;
    message_content->data = (unsigned char*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomDataString_t) );
    memcpy(message_content->data, (void*)data, length);
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_data_byte(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid, uint32_t info, uint8_t* data, uint32_t length)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomDataByte_t) + length;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomDataByte_t* message_content = (XepDispatchMessageContentHostcomDataByte_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_DATA_BYTE;
    message_content->common.message_size = size;
    message_content->contentid = contentid;
    message_content->info = info;
    message_content->length = length;
    message_content->data = (uint8_t*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomDataByte_t) );
    memcpy(message_content->data, (void*)data, length);
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_data_float(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid, uint32_t info, float* data, uint32_t length)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomDataFloat_t) + sizeof(float)*length;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomDataFloat_t* message_content = (XepDispatchMessageContentHostcomDataFloat_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_DATA_FLOAT;
    message_content->common.message_size = size;
    message_content->contentid = contentid;
    message_content->info = info;
    message_content->length = length;
    message_content->data = (float*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomDataFloat_t) );
    memcpy(message_content->data, (void*)data, length*sizeof(float));
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_reply_int(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid, uint32_t info, int32_t* data, uint32_t length)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomReplyInt_t) + sizeof(int)*length;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomReplyInt_t* message_content = (XepDispatchMessageContentHostcomReplyInt_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_REPLY_INT;
    message_content->common.message_size = size;
    message_content->contentid = contentid;
    message_content->info = info;
    message_content->length = length;
    message_content->data = (int32_t*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomReplyInt_t) );
    memcpy(message_content->data, (void*)data, length*sizeof(int));
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_reply_byte(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid, uint32_t info, uint8_t* data, uint32_t length)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomReplyByte_t) + length;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomReplyByte_t* message_content = (XepDispatchMessageContentHostcomReplyByte_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_REPLY_BYTE;
    message_content->common.message_size = size;
    message_content->contentid = contentid;
    message_content->info = info;
    message_content->length = length;
    message_content->data = (uint8_t*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomReplyByte_t) );
    for (int i = 0; i < length; i++)
        ((uint8_t*)message_content->data)[i] = ((uint8_t*)data)[i];
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_reply_string(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid, uint32_t info, char* format, ...)
{
    char buf[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, 256, format, args);
    va_end(args);
    int length = strlen(buf);
    return dispatch_message_hostcom_send_reply_stringn(dispatch, route, contentid, info, (unsigned char * )buf, length);
}

uint32_t dispatch_message_hostcom_send_reply_stringn(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid, uint32_t info, unsigned char* data, uint32_t length)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomReplyString_t) + length;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomReplyString_t* message_content = (XepDispatchMessageContentHostcomReplyString_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_REPLY_STRING;
    message_content->common.message_size = size;
    message_content->contentid = contentid;
    message_content->info = info;
    message_content->length = length;
    message_content->data = (unsigned char*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomReplyString_t) );
    memcpy(message_content->data, (void*)data, length);
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}

uint32_t dispatch_message_hostcom_send_reply_float(XepDispatch_t* dispatch, uint32_t route, uint32_t contentid, uint32_t info, float* data, uint32_t length)
{
    uint32_t size = sizeof(XepDispatchMessageContentHostcomReplyFloat_t) + sizeof(float)*length;
    MemoryBlock_t* memoryblock;
    uint32_t status = memorypoolset_take_auto(&memoryblock, dispatch->memorypoolset, size);
    if (status != XEP_ERROR_OK)
        return status;
    XepDispatchMessageContentHostcomReplyFloat_t* message_content = (XepDispatchMessageContentHostcomReplyFloat_t*)memoryblock->buffer;
    message_content->common.content_ref = XDMCR_HOSTCOM_REPLY_FLOAT;
    message_content->common.message_size = size;
    message_content->contentid = contentid;
    message_content->info = info;
    message_content->length = length;
    message_content->data = (float*) ( (uint32_t)(void*)message_content + sizeof(XepDispatchMessageContentHostcomReplyFloat_t) );
    memcpy(message_content->data, (void*)data, length*sizeof(float));
    uint32_t message_id=0;
    status = dispatch_send_message(&message_id, dispatch, route, memoryblock, message_content->common.message_size);
    return status;
}



uint32_t dispatch_message_hostcom_forward_radardata_frame_packet(XepDispatch_t* dispatch, uint32_t route, XepDispatchMessage_t* message)
{
    // Repurpuse radar dispatch message directly, with no copy.
    int status = dispatch_forward_message(dispatch, route, message);
    return status;
}

uint32_t dispatch_message_radardata_prepare_frame(XepDispatchMessageContentRadardataFramePacket_t** frame_packet, MemoryBlock_t** memoryblock, XepDispatch_t* dispatch, uint32_t bin_count)
{
    uint32_t size = (uint32_t)sizeof(XepDispatchMessageContentRadardataFramePacket_t) + bin_count*(uint32_t)sizeof(float);
    uint32_t status = dispatch_get_message_memoryblock(memoryblock, dispatch, size);
    if (status != XEP_ERROR_OK)
    {
        return status;
    }
    *frame_packet = (XepDispatchMessageContentRadardataFramePacket_t*)(void*)(*memoryblock)->buffer;
    (*frame_packet)->bin_count = bin_count;
    (*frame_packet)->common.content_ref = XDMCR_RADARDATA_FRAME;
    (*frame_packet)->common.message_size = size;
    (*frame_packet)->framedata = (float*)((uint32_t)(void*)*frame_packet + (uint32_t)sizeof(XepDispatchMessageContentRadardataFramePacket_t));
    return status;
}

