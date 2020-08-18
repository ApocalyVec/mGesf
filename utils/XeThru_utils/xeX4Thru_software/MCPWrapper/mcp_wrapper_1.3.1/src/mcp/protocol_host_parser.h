#ifndef PROTOCOL_HOST_PARSER_H
#define PROTOCOL_HOST_PARSER_H

#include "xtserial_definitions.h"

#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif

    static const uint32_t UNINITILIZED_ERROR_CODE = 0xdeadbeef;
    static const uint32_t PARSE_OK = 0;
    static const uint32_t ERROR = 1;
    static const uint32_t ERROR_SHORT_DATA = 2;
    static const uint32_t ERROR_WRONG_CONTENT = 3;
    static const uint32_t ERROR_UNKNOWN_PACKET = 4;
    static const uint32_t ERROR_NO_DATA = 5;

    struct Error
    {
        uint32_t error_code;
    };

    typedef struct
    {
        char dummy;
    } Ack;

    typedef struct
    {
        serial_protocol_response_datatype_t data_type;
        uint32_t content_id;
        uint32_t info;
        uint32_t length;
        uint8_t *data;
        uint8_t data_size;
    } Reply;

    typedef struct FloatData_
    {
        uint32_t content_id;
        uint32_t info;
        uint32_t length;
        const float *data;
    } FloatData;

    typedef struct
    {
        uint32_t frame_counter;
        uint32_t sensor_state;
        uint32_t respiration_rate;
        float distance;
        float movement;
        uint32_t signal_quality;
    } RespirationData;

    typedef struct
    {
        uint32_t frame_counter;
        uint32_t sensor_state;
        float respiration_rate;
        float distance;
        uint32_t signal_quality;
        float movement_slow;
        float movement_fast;
    } SleepData;

    typedef struct
    {
        uint32_t frame_counter;
        uint32_t num_bins;
        float bin_length;
        float sample_frequency;
        float carrier_frequency;
        float range_offset;
        const float *amplitude;
        const float *phase;
    } BasebandApData;

    typedef struct
    {
        uint32_t frame_counter;
        uint32_t num_bins;
        float bin_length;
        float sample_frequency;
        float carrier_frequency;
        float range_offset;
        const float *i_data;
        const float *q_data;
    } BasebandIqData;

    typedef struct
    {
        uint32_t status;
    } SystemStatus;

    typedef struct
    {
        uint32_t frame_counter;
        uint32_t presence_state;
        float distance;
        uint8_t direction;
        uint32_t signal_quality;
    } PresenceSingleData;

    typedef struct
    {
        uint32_t frame_counter;
        uint32_t presence_state;
        uint32_t interval_count;
        uint32_t detection_count;
        const float *movement_slow_items;
        const float *movement_fast_items;
        const float *detection_distance_items;
        const float *radar_cross_section_items;
        const float *detection_velocity_items;
    } PresenceMovingListData;

    typedef struct
    {
        uint32_t counter;
        uint32_t interval_count;
        const float *movement_slow_items;
        const float *movement_fast_items;

    } RespirationMovingListData;

    typedef struct
    {
        uint32_t frame_counter;
        float start;
        float bin_length;
        uint32_t count;
        float *normalized_movement_slow_items;
        float *normalized_movement_fast_items;
    } RespirationNormalizedMovementListData;

    typedef struct
    {
        uint32_t counter;
        uint32_t detection_count;
        float *detection_distance_items;
        float *detection_radar_cross_section_items;
        float *detection_velocity_items;
    } RespirationDetectionListData;

    typedef void (*McpPongCallback)(uint32_t pong, void *user_data);
    typedef void (*McpErrorCallback)(uint32_t data, void *user_data);
    typedef void (*McpDataFloatCallback)(FloatData data, void *user_data);
    typedef void (*McpAckCallback)(Ack data, void *user_data);
    typedef void (*McpReplyCallback)(Reply data, void *user_data);
    typedef void (*McpRespirationDataCallback)(RespirationData data, void *user_data);
    typedef void (*McpSleepDataCallback)(SleepData data, void *user_data);
    typedef void (*McpBasebandApDataCallback)(BasebandApData data, void *user_data);
    typedef void (*McpBasebandIqDataCallback)(BasebandIqData data, void *user_data);
    typedef void (*McpPresenceSingleCallback)(PresenceSingleData data, void *user_data);
    typedef void (*McpPresenceMovingListCallback)(PresenceMovingListData data, void *user_data);
    typedef void (*McpSystemStatusCallback)(SystemStatus ss, void *user_data);
    typedef void (*McpRespirationMovingListCallback)(RespirationMovingListData data, void *user_data);
    typedef void (*McpRespirationDetectionListCallback)(RespirationDetectionListData data, void *user_data);
    typedef void (*McpRespirationNormalizedMovementListCallback)(RespirationNormalizedMovementListData data, void *user_data);

    typedef struct
    {
        McpPongCallback pong;
        McpErrorCallback error;
        McpAckCallback ack;
        McpDataFloatCallback data_float;
        McpReplyCallback reply;
        McpRespirationDataCallback respiration;
        McpSleepDataCallback sleep;
        McpBasebandApDataCallback baseband_ap;
        McpBasebandIqDataCallback baseband_iq;
        McpPresenceSingleCallback presence_single;
        McpPresenceMovingListCallback presence_movinglist;
        McpSystemStatusCallback system_status;
        McpRespirationMovingListCallback respiration_movinglist;
        McpRespirationNormalizedMovementListCallback respiration_normalized_movinglist;
        McpRespirationDetectionListCallback respiration_detectionlist;
    } HostParser;

    void init_host_parser(HostParser *parser);

    int parse(HostParser *p, const unsigned char *data, unsigned int length, void *user_data);

    int parse_error(
        HostParser *p,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_system_status(
        HostParser *parser,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_baseband_ap(
        HostParser *parser,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_baseband_iq(
        HostParser *parser,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_respiration_status(
        HostParser *parser,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_sleep_status(
        HostParser *parser,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_data_float(
        HostParser *p,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_data(
        HostParser *p,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_appdata(
        HostParser *p,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_ack(
        HostParser *p,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_reply(
        HostParser *p,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_pong(
        HostParser *p,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_presence_single(
        HostParser *p,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_presence_movinglist(
        HostParser *parser,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_respiration_movinglist(
        HostParser *parser,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

    int parse_respiration_detectionlist(
        HostParser *parser,
        const unsigned char *data,
        unsigned int length,
        void *user_data);
    int parse_respiration_normalizedmovementlist(
        HostParser *parser,
        const unsigned char *data,
        unsigned int length,
        void *user_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
