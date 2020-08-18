#include "protocol_host_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>


static uint32_t bytes_to_uint32(const unsigned char *data)
{
    union {
        uint32_t ui;
        unsigned char c[sizeof(uint32_t)];
    } u;
    memcpy(u.c, data, sizeof(uint32_t));
    return u.ui;
}

static float bytes_to_float(const unsigned char *data)
{
    union {
        float f;
        char c[sizeof(float)];
    } u;
    memcpy(u.c, data, sizeof(float));
    return u.f;
}


static void default_pong_callback(uint32_t pong, void * user_data) {(void)pong; (void)user_data;}
static void default_ack_callback(Ack ack, void* user_data) {(void)ack; (void)user_data;}
static void default_error_callback(uint32_t error, void * user_data) {(void)error; (void)user_data;}
static void default_datafloat_callback(FloatData fd, void * user_data) {(void)fd; (void)user_data;}
static void default_reply_callback(Reply reply, void * user_data) {(void)reply; (void)user_data;}
static void default_respiration_callback(RespirationData rd, void * user_data){(void)rd; (void)user_data;}
static void default_sleep_callback(SleepData rd, void * user_data){(void)rd; (void)user_data;}
static void default_vitalsigns_callback(VitalSignsData rd, void * user_data){(void)rd; (void)user_data;}
static void default_sleepstage_callback(SleepStageData rd, void * user_data){(void)rd; (void)user_data;}
static void default_baseband_ap_callback(BasebandApData bap, void * user_data){(void)bap; (void)user_data;}
static void default_baseband_iq_callback(BasebandIqData bap, void * user_data){(void)bap; (void)user_data;}
static void default_presence_single(PresenceSingleData tps, void * user_data){(void)tps; (void)user_data;}
static void default_presence_movinglist(PresenceMovingListData tpml, void * user_data){(void)tpml; (void)user_data;}
static void default_system_status(SystemStatus ss, void * user_data){(void)ss; (void)user_data;}
static void default_respiration_movinglist(RespirationMovingListData arg1, void * arg2)
{(void)arg1; (void)arg2;}
static void default_respiration_detectionlist(RespirationDetectionListData arg1, void * arg2)
{(void)arg1; (void)arg2;}
static void default_respiration_normalizedmovementlist(RespirationNormalizedMovementListData arg1, void * arg2)
{(void)arg1; (void)arg2;}


void init_host_parser(HostParser * parser)
{
    parser->pong = default_pong_callback;
    parser->ack = default_ack_callback;
    parser->error = default_error_callback;
    parser->reply = default_reply_callback;
    parser->data_float = default_datafloat_callback;
    parser->respiration = default_respiration_callback;
    parser->sleep = default_sleep_callback;
    parser->vitalsigns = default_vitalsigns_callback;
    parser->sleepstage = default_sleepstage_callback;
    parser->baseband_ap = default_baseband_ap_callback;
    parser->baseband_iq = default_baseband_iq_callback;
    parser->presence_single = default_presence_single;
    parser->presence_movinglist = default_presence_movinglist;
    parser->system_status = default_system_status;
    parser->respiration_movinglist = default_respiration_movinglist;
    parser->respiration_detectionlist = default_respiration_detectionlist;
    parser->respiration_normalizedmovementlist = default_respiration_normalizedmovementlist;
}


int parse_error(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

     if(data[0] != XTS_SPR_ERROR)
         parser->error(UNINITILIZED_ERROR_CODE, user_data);

     const unsigned int offset = 1;
     const uint32_t error_code = bytes_to_uint32(&data[offset]);
     parser->error(error_code, user_data);
     return error_code;
}


int parse_data_float(
    HostParser * p,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        p->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    if (data[0] != XTS_SPR_DATA ||
        data[1] != XTS_SPRD_FLOAT) {
        return parse_error(p, data, length, user_data);
    }

    const unsigned int header_size = 14;

    if(length < header_size) {
        p->error(UNINITILIZED_ERROR_CODE, user_data);
        return 1;
    }

    FloatData fd;
    unsigned int offset = 2;
    fd.content_id = *(const uint32_t *)(&data[offset]);
    offset += sizeof(fd.content_id);
    fd.info = *(const uint32_t *)(&data[offset]);
    const unsigned int data_offset = header_size;
    const unsigned int data_size = length - header_size;
    fd.length = data_size/sizeof(float);
    fd.data = (float*)&data[data_offset];
    p->data_float(fd, user_data);
    return 0;
}


int parse_ack(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    if (data[0] != XTS_SPR_ACK) {
        return parse_error(parser, data, length, user_data);
    }
    Ack ack = {0};
    parser->ack(ack, user_data);
    return 0;
}


int parse_system_status(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if (data[offset] != XTS_SPR_SYSTEM) {
        return parse_error(parser, data, length, user_data);
    }
    ++offset;
    SystemStatus system_status;
    system_status.status = *(const uint32_t *)(&data[offset]);

    parser->system_status(system_status, user_data);
    return 0;
}


int parse_reply(
    HostParser * p,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        p->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if (data[offset] != XTS_SPR_REPLY) {
        return parse_error(p, data, length, user_data);
    }
    offset += 1;
    Reply reply;
    reply.data_type = (serial_protocol_response_datatype_t)data[offset];
    offset += 1;
    reply.content_id = bytes_to_uint32(&data[offset]);
    offset += sizeof(uint32_t);
    reply.info = bytes_to_uint32(&data[offset]);
    offset += sizeof(uint32_t);

    if (offset+1 >= length) {
        reply.data_size = (uint8_t)data[offset];
        reply.length = 0;
        p->reply(reply, user_data);
        return 0;
    }

    reply.length = bytes_to_uint32(&data[offset]);
    offset += sizeof(uint32_t);
    reply.data = (uint8_t*)&data[offset];
    offset += reply.length;
    reply.data_size = (uint8_t)data[offset];
    p->reply(reply, user_data);
    return 0;
}


int parse_pong(
    HostParser * p,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        p->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    if (data[0] != XTS_SPR_PONG) {
        return parse_error(p, data, length, user_data);
    }

    if (length < 1 + sizeof(uint32_t)) {
        p->error(UNINITILIZED_ERROR_CODE, user_data);
        return 1;
    }
    const unsigned int offset = 1;
    const uint32_t pongval = bytes_to_uint32(&data[offset]);
    p->pong(pongval, user_data);
    return PARSE_OK;
}


int parse_data(
    HostParser * p,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        p->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    const unsigned int offset = 1;
    if(data[offset] == XTS_SPRD_FLOAT) {
        return parse_data_float(p, data, length, user_data);
    }
    return ERROR;
}




int parse_presence_movinglist(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if(data[offset] != XTS_SPR_APPDATA) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    offset += sizeof(appdata_id);
    if(appdata_id != XTS_ID_PRESENCE_MOVINGLIST) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    PresenceMovingListData presence;
    presence.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(presence.frame_counter);

    presence.presence_state = bytes_to_uint32(&data[offset]);
    offset += sizeof(presence.presence_state);

    presence.interval_count = bytes_to_uint32(&data[offset]);
    offset += sizeof(presence.interval_count);

    presence.detection_count = bytes_to_uint32(&data[offset]);
    offset += sizeof(presence.detection_count);

    presence.movement_slow_items = (float*)&data[offset];
    offset += presence.interval_count * sizeof(*presence.movement_slow_items);

    presence.movement_fast_items = (float*)&data[offset];
    offset += presence.interval_count * sizeof(*presence.movement_slow_items);

    presence.detection_distance_items = (float*)&data[offset];
    offset += presence.detection_count * sizeof(*presence.detection_distance_items);

    presence.radar_cross_section_items = (float*)&data[offset];
    offset += presence.detection_count * sizeof(*presence.radar_cross_section_items);

    presence.detection_velocity_items = (float*)&data[offset];
    offset += presence.detection_count * sizeof(*presence.detection_velocity_items);

    parser->presence_movinglist(presence, user_data);
    return 0;
}


int parse_presence_single(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if(data[offset] != XTS_SPR_APPDATA) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    offset += sizeof(appdata_id);
    if(appdata_id != XTS_ID_PRESENCE_SINGLE) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    PresenceSingleData presence;
    presence.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(presence.frame_counter);

    presence.presence_state = bytes_to_uint32(&data[offset]);
    offset += sizeof(presence.presence_state);

    presence.distance = bytes_to_float(&data[offset]);
    offset += sizeof(presence.distance);

    presence.direction = data[offset];
    offset += sizeof(presence.direction);

    presence.signal_quality = bytes_to_uint32(&data[offset]);
    parser->presence_single(presence, user_data);
    return PARSE_OK;
}


int parse_baseband_iq(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if(data[offset] != XTS_SPR_APPDATA) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    if(appdata_id != XTS_ID_BASEBAND_IQ) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    if (length < sizeof(BasebandApData)) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return ERROR_SHORT_DATA;
    }

    BasebandIqData bap;
    offset += sizeof(appdata_id);
    bap.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(bap.frame_counter);
    bap.num_bins = bytes_to_uint32(&data[offset]);
    offset += sizeof(bap.num_bins);
    bap.bin_length = bytes_to_float(&data[offset]);
    offset += sizeof(bap.bin_length);
    bap.sample_frequency = bytes_to_float(&data[offset]);
    offset += sizeof(bap.sample_frequency);
    bap.carrier_frequency = bytes_to_float(&data[offset]);
    offset += sizeof(bap.carrier_frequency);
    bap.range_offset = bytes_to_float(&data[offset]);
    offset += sizeof(bap.range_offset);
    bap.i_data = (const float *)(&data[offset]);
    offset += sizeof(float) * bap.num_bins;
    bap.q_data = (const float *)(&data[offset]);

    parser->baseband_iq(bap, user_data);
    return PARSE_OK;
}


int parse_baseband_ap(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if (data[offset] != XTS_SPR_APPDATA) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    if (appdata_id != XTS_ID_BASEBAND_AMPLITUDE_PHASE) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    if (length < sizeof(BasebandApData)) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return ERROR_SHORT_DATA;
    }

    BasebandApData bap;
    offset += sizeof(appdata_id);
    bap.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(bap.frame_counter);
    bap.num_bins = bytes_to_uint32(&data[offset]);
    offset += sizeof(bap.num_bins);
    bap.bin_length = bytes_to_float(&data[offset]);
    offset += sizeof(bap.bin_length);
    bap.sample_frequency = bytes_to_float(&data[offset]);
    offset += sizeof(bap.sample_frequency);
    bap.carrier_frequency = bytes_to_float(&data[offset]);
    offset += sizeof(bap.carrier_frequency);
    bap.range_offset = bytes_to_float(&data[offset]);
    offset += sizeof(bap.range_offset);
    bap.amplitude = (const float *)(&data[offset]);
    offset += sizeof(float) * bap.num_bins;
    bap.phase = (const float *)(&data[offset]);

    parser->baseband_ap(bap, user_data);
    return PARSE_OK;
}



int parse_sleep_status(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    if(appdata_id != XTS_ID_SLEEP_STATUS) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return 1;
    }
    offset = 1 + 4;
    if (length < sizeof(SleepData)) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return 1;
    }

    SleepData sd;
    sd.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(sd.frame_counter);
    sd.sensor_state = bytes_to_uint32(&data[offset]);
    offset += sizeof(sd.sensor_state);
    sd.respiration_rate = bytes_to_float(&data[offset]);
    offset += sizeof(sd.respiration_rate);
    sd.distance = bytes_to_float(&data[offset]);
    offset += sizeof(sd.distance);
    sd.signal_quality = bytes_to_uint32(&data[offset]);
    offset += sizeof(sd.signal_quality);
    sd.movement_slow = bytes_to_float(&data[offset]);
    offset += sizeof(sd.movement_slow);
    sd.movement_fast = bytes_to_float(&data[offset]);
    offset += sizeof(sd.movement_fast);
    parser->sleep(sd, user_data);
    return 0;
}
int parse_vitalsigns_status(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    if(appdata_id != XTS_ID_VITAL_SIGNS) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return 1;
    }
    offset = 1 + 4;
    if (length < sizeof(VitalSignsData)) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return 1;
    }

    VitalSignsData sd;
    sd.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(sd.frame_counter);
    sd.sensor_state = bytes_to_uint32(&data[offset]);
    offset += sizeof(sd.sensor_state);
    sd.respiration_rate = bytes_to_float(&data[offset]);
    offset += sizeof(sd.respiration_rate);
    sd.respiration_distance = bytes_to_float(&data[offset]);
    offset += sizeof(sd.respiration_distance);
    sd.respiration_confidence = bytes_to_float(&data[offset]);
    offset += sizeof(sd.respiration_confidence);
    sd.heart_rate = bytes_to_float(&data[offset]);
    offset += sizeof(sd.heart_rate);
    sd.heart_distance = bytes_to_float(&data[offset]);
    offset += sizeof(sd.heart_distance);
    sd.heart_confidence = bytes_to_float(&data[offset]);
    offset += sizeof(sd.heart_confidence);
    sd.normalized_movement_slow = bytes_to_float(&data[offset]);
    offset += sizeof(sd.normalized_movement_slow);
    sd.normalized_movement_fast = bytes_to_float(&data[offset]);
    offset += sizeof(sd.normalized_movement_fast);
    sd.normalized_movement_start = bytes_to_float(&data[offset]);
    offset += sizeof(sd.normalized_movement_start);
    sd.normalized_movement_end = bytes_to_float(&data[offset]);
    offset += sizeof(sd.normalized_movement_end);
    parser->vitalsigns(sd, user_data);
    return 0;
}
int parse_sleepstage_status(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    if(appdata_id != XTS_ID_SLEEPSTAGE) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return 1;
    }
    offset = 1 + 4;
    if (length < sizeof(SleepStageData)) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return 1;
    }

    SleepStageData sd;
    sd.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(sd.frame_counter);
    sd.sleepstage = bytes_to_uint32(&data[offset]);
    offset += sizeof(sd.sleepstage);
    parser->sleepstage(sd, user_data);
    return 0;
}


int parse_respiration_status(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    if(appdata_id != XTS_ID_RESP_STATUS) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return 1;
    }
    offset = 1 + 4;
    if (length < sizeof(RespirationData)) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return 1;
    }

    RespirationData rd;
    rd.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(rd.frame_counter);
    rd.sensor_state = bytes_to_uint32(&data[offset]);
    offset += sizeof(rd.sensor_state);
    rd.respiration_rate = bytes_to_uint32(&data[offset]);
    offset += sizeof(rd.respiration_rate);
    rd.distance = bytes_to_float(&data[offset]);
    offset += sizeof(rd.distance);
    rd.movement = bytes_to_float(&data[offset]);
    offset += sizeof(rd.movement);
    rd.signal_quality = bytes_to_uint32(&data[offset]);
    parser->respiration(rd, user_data);
    return 0;
}


int parse_appdata(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    const unsigned int offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);

    if(appdata_id == XTS_ID_RESP_STATUS) {
        return parse_respiration_status(parser, data, length, user_data);
    }

    if(appdata_id == XTS_ID_SLEEP_STATUS) {
        return parse_sleep_status(parser, data, length, user_data);
    }

    if(appdata_id == XTS_ID_VITAL_SIGNS) {
        return parse_vitalsigns_status(parser, data, length, user_data);
    }

    if(appdata_id == XTS_ID_SLEEPSTAGE) {
        return parse_sleepstage_status(parser, data, length, user_data);
    }

    if(appdata_id == XTS_ID_BASEBAND_AMPLITUDE_PHASE) {
        return parse_baseband_ap(parser, data, length, user_data);
    }

    if(appdata_id == XTS_ID_BASEBAND_IQ) {
        return parse_baseband_iq(parser, data, length, user_data);
    }

    if(appdata_id == XTS_ID_PRESENCE_SINGLE) {
        return parse_presence_single(parser, data, length, user_data);
    }
    if(appdata_id == XTS_ID_PRESENCE_MOVINGLIST) {
        return parse_presence_movinglist(parser, data, length, user_data);
    }

    return PARSE_OK;
}



int parse_respiration_movinglist(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if(data[offset] != XTS_SPR_APPDATA) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    offset += sizeof(appdata_id);
    if(appdata_id != XTS_ID_RESPIRATION_MOVINGLIST) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    RespirationMovingListData result;
    result.counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(result.counter);

    result.interval_count = bytes_to_uint32(&data[offset]);
    offset += sizeof(result.interval_count);

    result.movement_slow_items = (float*)&data[offset];
    offset += result.interval_count * sizeof(*result.movement_slow_items);

    result.movement_fast_items = (float*)&data[offset];
    offset += result.interval_count * sizeof(*result.movement_slow_items);

    parser->respiration_movinglist(result, user_data);
    return 0;
}


int parse_respiration_detectionlist(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if(data[offset] != XTS_SPR_APPDATA) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    offset += sizeof(appdata_id);
    if(appdata_id != XTS_ID_RESPIRATION_DETECTIONLIST) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    RespirationDetectionListData result;
    result.counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(result.counter);
    result.detection_count = bytes_to_uint32(&data[offset]);
    offset += sizeof(result.detection_count);
    result.detection_distance_items = (float*)&data[offset];
    offset += result.detection_count * sizeof(*result.detection_distance_items);
    result.detection_radar_cross_section_items = (float*)&data[offset];
    offset += result.detection_count * sizeof(*result.detection_radar_cross_section_items);
    result.detection_velocity_items = (float*)&data[offset];
    offset += result.detection_count * sizeof(*result.detection_velocity_items);

    parser->respiration_detectionlist(result, user_data);
    return 0;
}

int parse_respiration_normalizedmovementlist(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    if(length < 21) { // headers only
        parser->error(ERROR_SHORT_DATA, user_data);
        return ERROR_SHORT_DATA;;
    }

    unsigned int offset = 0;
    if(data[offset] != XTS_SPR_APPDATA) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    offset += sizeof(appdata_id);
    if(appdata_id != XTS_ID_RESPIRATION_NORMALIZEDMOVEMENTLIST) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    RespirationNormalizedMovementListData result;
    result.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(result.frame_counter);
    result.start = bytes_to_float(&data[offset]);
    offset += sizeof(result.start);
    result.bin_length = bytes_to_float(&data[offset]);
    offset += sizeof(result.bin_length);
    result.count = bytes_to_uint32(&data[offset]);
    offset += sizeof(result.count);

    result.normalized_movement_slow_items = (float*)&data[offset];
    offset += result.count * sizeof(*result.normalized_movement_slow_items);
    result.normalized_movement_fast_items = (float*)&data[offset];
    offset += result.count * sizeof(*result.normalized_movement_fast_items);

    if (length < offset) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return ERROR_SHORT_DATA;;
    }

    parser->respiration_normalizedmovementlist(result, user_data);
    return 0;
}

int parse_pulsedoppler_float(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if(data[offset] != XTS_SPR_APPDATA) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    if(appdata_id != XTS_ID_PULSEDOPPLER_FLOAT &&
            appdata_id != XTS_ID_NOISEMAP_FLOAT) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    if (length < sizeof(PulseDopplerFloatData)) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return ERROR_SHORT_DATA;
    }

    PulseDopplerFloatData pdf;
    offset += sizeof(appdata_id);
    pdf.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdf.frame_counter);

    pdf.matrix_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdf.matrix_counter);

    pdf.range_idx = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdf.range_idx);

    pdf.range_bins = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdf.range_bins);

    pdf.frequency_count = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdf.frequency_count);

    pdf.pulsedoppler_instance = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdf.pulsedoppler_instance);

    pdf.fps = bytes_to_float(&data[offset]);
    offset += sizeof(pdf.fps);

    pdf.fps_decimated = bytes_to_float(&data[offset]);
    offset += sizeof(pdf.fps_decimated);

    pdf.frequency_start = bytes_to_float(&data[offset]);
    offset += sizeof(pdf.frequency_start);

    pdf.frequency_step = bytes_to_float(&data[offset]);
    offset += sizeof(pdf.frequency_step);

    pdf.range = bytes_to_float(&data[offset]);
    offset += sizeof(pdf.range);

    if (length != offset + pdf.frequency_count*sizeof(float)) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return ERROR_SHORT_DATA;
    }

    pdf.data = (const float *)(&data[offset]);


    parser->pulsedoppler_float(pdf, user_data);
    return PARSE_OK;
}

int parse_pulsedoppler_byte(
    HostParser * parser,
    const unsigned char * data,
    unsigned int length,
    void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    unsigned int offset = 0;
    if(data[offset] != XTS_SPR_APPDATA) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    offset = 1;
    const uint32_t appdata_id = bytes_to_uint32(&data[offset]);
    if(appdata_id != XTS_ID_PULSEDOPPLER_BYTE &&
            appdata_id != XTS_ID_NOISEMAP_BYTE) {
        parser->error(ERROR_WRONG_CONTENT, user_data);
        return ERROR_WRONG_CONTENT;
    }

    if (length < sizeof(PulseDopplerFloatData)) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return ERROR_SHORT_DATA;
    }

    PulseDopplerByteData pdb;
    offset += sizeof(appdata_id);
    pdb.frame_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdb.frame_counter);

    pdb.matrix_counter = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdb.matrix_counter);

    pdb.range_idx = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdb.range_idx);

    pdb.range_bins = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdb.range_bins);

    pdb.frequency_count = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdb.frequency_count);

    pdb.pulsedoppler_instance = bytes_to_uint32(&data[offset]);
    offset += sizeof(pdb.pulsedoppler_instance);

    pdb.byte_step_start = bytes_to_float(&data[offset]);
    offset += sizeof(pdb.byte_step_start);

    pdb.byte_step_size = bytes_to_float(&data[offset]);
    offset += sizeof(pdb.byte_step_size);

    pdb.fps = bytes_to_float(&data[offset]);
    offset += sizeof(pdb.fps);

    pdb.fps_decimated = bytes_to_float(&data[offset]);
    offset += sizeof(pdb.fps_decimated);

    pdb.frequency_start = bytes_to_float(&data[offset]);
    offset += sizeof(pdb.frequency_start);

    pdb.frequency_step = bytes_to_float(&data[offset]);
    offset += sizeof(pdb.frequency_step);

    pdb.range = bytes_to_float(&data[offset]);
    offset += sizeof(pdb.range);

    if (length != offset + pdb.frequency_count) {
        parser->error(ERROR_SHORT_DATA, user_data);
        return ERROR_SHORT_DATA;
    }

    pdb.data = (const uint8_t *)(&data[offset]);

    parser->pulsedoppler_byte(pdb, user_data);
    return PARSE_OK;
}



int parse(HostParser * parser, const unsigned char * data, unsigned int length, void * user_data)
{
    if (!length) {
        parser->error(ERROR_NO_DATA, user_data);
        return ERROR_NO_DATA;
    }

    const unsigned int offset = 0;
    if (data[offset] == XTS_SPR_ERROR) {
        return parse_error(parser, data, length, user_data);
    }

    if(data[offset] == XTS_SPR_ACK) {
        return parse_ack(parser, data, length, user_data);
    }
    else if (data[offset] == XTS_SPR_PONG) {
        return parse_pong(parser, data, length, user_data);
    }
    else if (data[offset] == XTS_SPR_DATA) {
        return parse_data(parser, data, length, user_data);
    }
    else if (data[offset] == XTS_SPR_APPDATA) {
        return parse_appdata(parser, data, length, user_data);
    }
    else {
        parser->error(2,user_data);
    }

    return ERROR_UNKNOWN_PACKET;
}






#ifdef __cplusplus
}
#endif /* __cplusplus */


