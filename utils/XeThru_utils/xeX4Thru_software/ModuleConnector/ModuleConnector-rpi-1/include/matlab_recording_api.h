#ifndef RECORDING_API_C_DEFINITIONS_H
#define RECORDING_API_C_DEFINITIONS_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PreferredSplitSize PreferredSplitSize;
typedef struct RecordingOptions RecordingOptions;
typedef struct DataRecorder DataRecorder;
typedef struct DataReader DataReader;
typedef struct DataPlayer DataPlayer;

struct PreferredSplitSize
{
    void *priv;
};

struct RecordingOptions
{
    void *priv;
};

struct DataRecorder
{
    void *priv;
};

struct DataReader
{
    void *priv;
};

struct DataPlayer
{
    void *priv;
};

// PreferredSplitSize
extern void set_duration(PreferredSplitSize *self, int duration);
extern void set_byte_count(PreferredSplitSize *self, int64_t count);
extern void set_fixed_daily_hour(PreferredSplitSize *self, int hour);

// RecordingOptions
extern void set_session_id(RecordingOptions *self, const char *id, uint32_t length);
extern int get_session_id(RecordingOptions *self, char *result, uint32_t *length, uint32_t max_length);
extern void set_file_split_size(RecordingOptions *self, PreferredSplitSize *size);
extern void set_directory_split_size(RecordingOptions *self, PreferredSplitSize *size);
extern void set_data_rate_limit(RecordingOptions *self, int limit);
extern void set_user_header(RecordingOptions *self, const char *header, uint32_t length);

extern PreferredSplitSize *nva_create_recording_split_size();
extern void nva_destroy_recording_split_size(PreferredSplitSize *instance);

extern RecordingOptions *nva_create_recording_options();
extern void nva_destroy_recording_options(RecordingOptions *instance);

// DataRecorder
extern DataRecorder *nva_create_data_recorder();
extern DataRecorder *internal_nva_create_data_recorder(void *locked_radar_interface);
extern void nva_destroy_data_recorder(DataRecorder *recorder);
extern int nva_start_recording(DataRecorder *recorder, uint32_t data_types,
                               const char *directory, uint32_t length,
                               RecordingOptions *options);
extern void nva_stop_recording(DataRecorder *recorder, uint32_t data_types);
extern int nva_process(DataRecorder *recorder, uint32_t data_type, const uint8_t *bytes, uint32_t length);
extern void nva_set_basename_for_data_type(DataRecorder *recorder, uint32_t data_type, const char *name, uint32_t length);
extern int nva_get_basename_for_data_type(DataRecorder *recorder, uint32_t data_type, char *dst,
                                          uint32_t *length, uint32_t max_length);
extern void nva_clear_basename_for_data_types(DataRecorder *recorder, uint32_t data_types);
extern int nva_get_recording_dir_for_session_id(DataRecorder *recorder, const char *id, uint32_t id_length,
                                                char *dir, uint32_t *dir_length, uint32_t dir_max_length);
extern int nva_get_recording_dir_for_data_type(DataRecorder *recorder, uint32_t data_type,
                                               char *dir, uint32_t *dir_length, uint32_t max_dir_length);

// DataReader
extern DataReader *data_reader_create();
extern void data_reader_destroy(DataReader *reader);
extern int data_reader_open(DataReader *reader, const char *meta_filename, uint32_t length, int depth);
extern void data_reader_close(DataReader *reader);
extern int data_reader_is_open(DataReader *reader);
extern int data_reader_at_end(DataReader *reader);
extern int data_reader_read_record(DataReader *reader, uint8_t *data, uint32_t *length, uint32_t max_length,
                                   uint32_t *data_type, int64_t *epoch, uint8_t *is_user_header);
extern int data_reader_peek_record(DataReader *reader, uint8_t *data, uint32_t *length, uint32_t max_length,
                                   uint32_t *data_type, int64_t *epoch, uint8_t *is_user_header);
extern int data_reader_seek_ms(DataReader *reader, int64_t position);
extern int data_reader_seek_byte(DataReader *reader, int64_t position);
extern int data_reader_set_filter(DataReader *reader, uint32_t data_types);
extern int data_reader_get_filter(DataReader *reader, uint32_t *data_types);
extern int data_reader_get_start_epoch(DataReader *reader, int64_t *epoch);
extern int data_reader_get_duration(DataReader *reader, int64_t *duration);
extern int data_reader_get_size(DataReader *reader, int64_t *size);
extern int data_reader_get_data_types(DataReader *reader, uint32_t *data_types);
extern int data_reader_get_max_record_size(DataReader *reader, unsigned int *max_record_size);
extern int data_reader_get_session_id(DataReader *reader, char *result, uint32_t *length, uint32_t max_length);

// Conversion API
// from disk data (read_record) to binary packet
extern int data_record_to_binary_packet(DataReader *reader,
                                        const uint8_t *data, uint32_t length, uint32_t data_type, uint8_t is_user_header,
                                        uint8_t *packet, uint32_t *packet_length, uint32_t max_packet_length);
// from disk data (read_record) to parsed data fields
extern int data_record_to_sleep_data(DataReader *reader,
                                     const uint8_t *data, uint32_t length, uint32_t data_type, uint8_t is_user_header,
                                     uint32_t *frame_counter, uint32_t *sensor_state, float *respiration_rate,
                                     float *distance, uint32_t *signal_quality, float *movement_slow, float *movement_fast);

// DataPlayer
extern DataPlayer *data_player_create(const char *meta_filename, uint32_t length, int depth);
extern void data_player_destroy(DataPlayer *player);
extern int data_player_play(DataPlayer *player); // Convenience, equivalent to set_state(DataPlayer::PlayingState).
extern int data_player_pause(DataPlayer *player); // Convenience, equivalent to set_state(DataPlayer::PausedState).
extern int data_player_stop(DataPlayer *player); // Convenience, equivalent to set_state(DataPlayer::StoppedState).
extern int data_player_set_state(DataPlayer *player, uint32_t state);
extern int data_player_get_state(DataPlayer *player, uint32_t *state);
extern int data_player_set_filter(DataPlayer *player, uint32_t data_types);
extern int data_player_get_filter(DataPlayer *player, uint32_t *data_types);
extern int data_player_set_position(DataPlayer *player, int64_t position);
extern int data_player_get_position(DataPlayer *player, int64_t *position);
extern int data_player_set_playback_rate(DataPlayer *player, float rate);
extern int data_player_get_playback_rate(DataPlayer *player, float *rate);
extern int data_player_set_loop_mode_enabled(DataPlayer *player, uint32_t enabled);
extern int data_player_get_loop_mode_enabled(DataPlayer *player, uint32_t *enabled);
extern int data_player_get_duration(DataPlayer *player, int64_t *duration);
extern int data_player_get_meta_filename(DataPlayer *player, char *meta_filename, uint32_t *length, uint32_t max_length);

#ifdef __cplusplus
} // extern C
#endif

#endif // RECORDING_API_C_DEFINITIONS_H
