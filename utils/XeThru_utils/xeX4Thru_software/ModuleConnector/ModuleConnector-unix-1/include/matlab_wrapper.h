#ifndef LIBFILE1_HPP
#define LIBFILE1_HPP

#include "matlab_recording_api.h"
#include "datatypes.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RadarInterfacePrivate;
struct Logger;

const unsigned int ERROR = 1;
const unsigned int OK = 0;

typedef struct ModuleConnector_Type
{
    struct RadarInterfacePrivate *radar_interface;
    void * factory;
    void * io;
    struct Logger * logger;
} ModuleConnector;

typedef struct X2_Type
{
    void *priv;
} X2;


/*********************************************************************************
 * ModuleConnector
 *********************************************************************************/
ModuleConnector * nva_create_module_connector(
    const char * device_name,
    unsigned int level);

ModuleConnector * nva_create_module_connector_tcpip(
    uint32_t ip,
    uint16_t port,
    const char * device_name,
    unsigned int log_level);

ModuleConnector * nva_create_module_connector_playback(DataPlayer *player, unsigned int level);

void nva_destroy_module_connector(
    ModuleConnector * mc);

// DataRecorder
DataRecorder * nva_get_data_recorder(ModuleConnector *mc);

// X2
X2 * nva_get_x2(ModuleConnector * mc);
void nva_destroy_X2_interface(X2 * x2);

void nva_set_log_level(ModuleConnector * mc, int log_level);
const char * nva_git_sha();


//*********************************************************************************
//*  X2M200
//*********************************************************************************
int nva_get_item_number(X2 * instance, char * result, unsigned int max_length);
int nva_get_order_code(X2 * instance, char * result, unsigned int max_length);
int nva_get_firmware_id(X2 * instance, char * result, unsigned int max_length);
int nva_get_firmware_version(X2 * instance, char * result, unsigned int max_length);
int nva_get_serial_number(X2 * instance, char * result, unsigned int max_length);
int nva_get_build_info(X2 * instance, char * result, unsigned int max_length);
int nva_get_app_id_list(X2 * instance, char * result, unsigned int max_length);
int nva_module_reset(X2 * instance);
int nva_reset_to_factory_preset(X2 * instance);
int nva_enter_bootloader(X2 * instance);
int nva_set_sensor_mode_run(X2 * instance);
int nva_set_sensor_mode_idle(X2 * instance);
int nva_load_sleep_profile(X2 * instance);
int nva_load_respiration_profile(X2 * instance);
int nva_enable_baseband(X2 * instance, int );
int nva_enable_baseband_ap(X2 * instance);
int nva_disable_baseband_ap(X2 * instance);
int nva_enable_baseband_iq(X2 * instance);
int nva_disable_baseband_iq(X2 * instance);
int nva_set_detection_zone(X2 * instance, float start, float end);
int nva_set_sensitivity(X2 * instance, uint32_t new_sensitivity);
int nva_get_sensitivity(X2 * instance, uint32_t *const sensitivity);
int nva_set_led_control(X2 * instance, uint8_t mode, uint8_t intensity);
int nva_get_led_control(X2 * instance, uint8_t * const mode);
int nva_subscribe_to_resp_status(X2 * instance, const char * name);
int nva_subscribe_to_sleep_status(X2 * instance, const char * name);
int nva_subscribe_to_baseband_ap(X2 * instance, const char * name);
int nva_subscribe_to_baseband_iq(X2 * instance, const char * name);

int nva_disable_resp_output(X2 * instance);
int nva_enable_resp_output(X2 * instance);
int nva_get_respiration_data(
    X2 * instance,
    const char * name,
    uint32_t * frame_counter,
    uint32_t * sensor_state,
    uint32_t * respiration_rate,
    float * distance,
    float * movement,
    uint32_t * signal_quality);
int nva_get_sleep_data(
    X2 * instance,
    const char * name,
    uint32_t * frame_counter,
    uint32_t * sensor_state,
    float * respiration_rate,
    float * distance,
    uint32_t * signal_quality,
    float * movement_slow,
    float * movement_fast);


int nva_get_baseband_iq_data(
    X2 * instance,
    const char * name,
    uint32_t * frame_counter,
    uint32_t * num_bins,
    float * bin_length,
    float * sample_frequency,
    float * carrier_frequency,
    float * range_offset,
    float * i_data,
    float * q_data,
    uint32_t max_length);
int nva_get_baseband_ap_data(
    X2 * instance,
    const char * name,
    uint32_t * frame_counter,
    uint32_t * num_bins,
    float * bin_length,
    float * sample_frequency,
    float * carrier_frequency,
    float * range_offset,
    float * amplitude,
    float * phase,
    uint32_t max_length);






//*********************************************************************************
//*  X2
//*********************************************************************************
int nva_set_debug_level(X2 * instance, unsigned char level);
int nva_ping(X2 * instance, unsigned int * pong);
int nva_get_system_info(
    X2 * instance,
    uint8_t info_code,
    char * result,
    unsigned int max_length);
int nva_set_X2_float(X2 * instance, uint32_t id, float value);
int nva_get_x2_float(X2 * instance, uint32_t id, float * result);
int nva_set_X2_int(X2 * instance, uint32_t id, uint32_t value);
int nva_get_x2_int(X2 * instance, uint32_t id, uint32_t * result);
int nva_set_X2_register(X2 * instance, uint32_t register_id, uint32_t value);
int nva_get_x2_register(
    X2 * instance,
    uint32_t register_id,
    uint32_t * value);
int nva_run_timing_measurement(X2 * instance);
int nva_set_fps(X2 * instance, unsigned int fps);
int nva_capture_single_normalized_frame(
    X2 * instance,
    uint32_t * frame_counter,
    float * frame_data,
    unsigned int * length,
    unsigned int max_length);
int nva_set_sensor_mode(X2 * instance, unsigned char mode);
int nva_set_sensor_mode_and_submode(
    X2 * instance,
    unsigned char mode,
    unsigned char submode);
int nva_subscribe_to_raw_normalized(X2 * instance, const char * name);
int nva_unsubscribe(X2 * instance, const char * name);
int nva_get_raw_normalized_frame(
    X2 * instance,
    const char * name,
    uint32_t * frame_counter,
    float * frame_data,
    uint32_t * length,
    unsigned int max_length);
int nva_get_number_of_packets(
    X2 * instance,
    const char * name,
    uint32_t * result);
int nva_clear(X2 * instance, const char * name);


// should maybe be removed
void nva_wait(unsigned int seconds);
void nva_log(X2 * instance, const char * string);
int nva_set_sensor_mode_manual(X2 * instance);


//*********************************************************************************
//*  Transport
//*********************************************************************************
int nva_send_command(
    X2 * instance,
    const unsigned char * command,
    unsigned int length);

//Bytes send_command_single(const Bytes & command, const Bytes & comparator);
int nva_send_command_single(
    X2 * instance,
    const unsigned char * command,
    unsigned int length,
    const unsigned char * comparator,
    unsigned int comp_length,
    unsigned char * response,
    unsigned int max_length);

/* virtual Bytes send_command_multi( */
/*     const Bytes & command, */
/*     const std::vector<Bytes> & comparator) = 0; */

int nva_subscribe(
    X2 * instance,
    const char * name,
    const unsigned char * comparator,
    unsigned int comp_length);

int nva_get_packet(
    X2 * instance,
    const char * name,
    unsigned char * packet,
    unsigned int max_length);



/*********************************************************************************/
/*  NotSupported API */
/*********************************************************************************/
int nva_get_x4_io_pin_value(
    X2 * instance,
    unsigned char pin,
    unsigned char * result );

int nva_set_x4_io_pin_value(
    X2 * instance,
    unsigned char pin,
    unsigned char value);

int nva_set_x4_io_pin_mode(
    X2 * instance,
    const unsigned char pin,
    const unsigned char mode);

int nva_set_x4_io_pin_dir(
    X2 * instance,
    const unsigned char pin,
    const unsigned char direction);

int nva_set_x4_io_pin_enable(
    X2 * instance,
    const unsigned char pin);

int nva_set_x4_io_pin_disable(
    X2 * instance,
    const unsigned char pin);

int nva_read_x4_spi(
    X2 * instance,
    const unsigned char address,
    unsigned char * result);

int nva_write_x4_spi(
    X2 * instance,
    unsigned char address,
    const unsigned char value);

int nva_set_x4_fps(
    X2 * instance,
    const uint32_t fps);

int nva_subscribe_to_x4_decim(
    X2 * instance,
    const char * name);

int nva_get_x4_decim_frame(
    X2 * instance,
    const char * name,
    uint32_t * frame_counter,
    double * frame_data,
    unsigned int max_length);

int nva_subscribe_to_data_float(
    X2 * instance,
    const char * name);

int nva_peek_message_data_float(X2 * instance);

int nva_read_message_data_float(
    X2 * instance,
    uint32_t * content_id,
    uint32_t * info,
    float * frame_data,
    uint32_t * length,
    unsigned int max_length);

int nva_set_parameter_file(
    X2 * instance,
    const char * filename,
    const char * data);

int nva_get_parameter_file(
        X2 * instance,
        const char * filename,
        char * data,
        unsigned int max_length);


int nva_load_profile(
    X2 * instance,
    const uint32_t profile_id);

int nva_get_trace(X2 * instance, const char * name, char * trace, unsigned int max_length);
int nva_subscribe_to_trace(X2 * instance, const char * name);
int nva_subscribe_to_data_byte(X2 * instance, const char * name);


//*********************************************************************************
//*  X4M200
//*********************************************************************************

int nva_peek_message_respiration_sleep(X2 * instance);
int nva_read_message_respiration_sleep(
    X2 * instance,
    uint32_t * counter,
    uint32_t * respirationState,
    float * rpm,
    float * distance,
    uint32_t * signalQuality,
    float * movementSlow,
    float * movementFast);

int nva_peek_message_respiration_legacy(X2 * instance);
int nva_read_message_respiration_legacy(
    X2 * instance,
    uint32_t * counter,
    uint32_t * stateCode,
    uint32_t * stateData,
    float * distance,
    float * movement,
    uint32_t * signalQuality);

int nva_peek_message_respiration_movinglist(X2 * instance);

int nva_read_message_respiration_movinglist(
    X2 * instance,
    uint32_t * counter,
    uint32_t * movementIntervalCount,
    float * movement_slow_items,
    float * movement_fast_items,
    uint32_t max_length);

int nva_peek_message_respiration_detectionlist(X2 * instance);
int nva_read_message_respiration_detectionlist(
    X2 * instance,
    uint32_t * counter,
    uint32_t * detection_count,
    float * detection_distance_items,
    float * detection_radar_cross_section_items,
    float * detection_velocity_items,
    uint32_t max_items);

int nva_peek_message_respiration_normalizedmovementlist(X2 * instance);
int nva_read_message_respiration_normalizedmovementlist(
    X2 * instance,
    uint32_t * frame_counter,
    float * start,
    float * bin_length,
    uint32_t * count,
    float * normalized_movement_slow_items,
    float * normalized_movement_fast_items,
    uint32_t max_items);

int nva_peek_message_vital_signs(X2 * instance);
int nva_read_message_vital_signs(
    X2 * instance,
    uint32_t *frame_counter,
    uint32_t *sensor_state,
    float *respiration_rate,
    float *respiration_distance,
    float *respiration_confidence,
    float *heart_rate,
    float *heart_distance,
    float *heart_confidence,
    float *normalized_movement_slow,
    float *normalized_movement_fast,
    float *normalized_movement_start,
    float *normalized_movement_end);

//*********************************************************************************
//*  X4M300
//*********************************************************************************
int nva_get_detection_zone(X2 * instance, float * start, float * end);
int nva_get_detection_zone_limits(X2 * instance, float * start, float * end, float * step);

//int nva_subscribe_to_presence_single(X2 * instance);
int nva_peek_message_presence_single(X2 * instance);
int nva_read_message_presence_single(
    X2 * instance,
    uint32_t * frame_counter,
    uint32_t * presence_state,
    float * distance,
    uint8_t * direction,
    uint32_t * signal_quality);

//int nva_subscribe_to_presence_movinglist(X2 * instance);
int nva_peek_message_presence_movinglist(X2 * instance);
int nva_read_message_presence_movinglist(
    X2 * instance,
    uint32_t * frame_counter,
    uint32_t * state,
    uint32_t * interval_count,
    uint32_t * detection_count,
    float * movement_slow,
    float * movement_fast,
    float * distance,
    float * radar_cross_section,
    float * velocity,
    uint32_t max_length);






//*********************************************************************************
//*  XEP
//*********************************************************************************
int xep_x4driver_set_fps(X2 * instance, const float fps);
int xep_x4driver_get_fps(X2 * instance, float * fps);
int xep_x4driver_set_iterations(X2 * instance, const uint32_t iterations);
int xep_x4driver_get_iterations(X2 * instance, uint32_t *iterations);
int xep_x4driver_set_pulsesperstep(X2 * instance, const uint32_t pulsesperstep);
int xep_x4driver_get_pulsesperstep(X2 * instance, uint32_t *pulsesperstep);
int xep_x4driver_set_downconversion(X2 * instance, const uint8_t downconversion);
int xep_x4driver_get_downconversion(X2 * instance, uint8_t *downconversion);
int xep_x4driver_set_frame_area(X2 * instance, const float start, const float end);
int xep_x4driver_init(X2 * instance);
int xep_x4driver_set_dac_min(X2 * instance, const uint32_t dac_min);
int xep_x4driver_get_dac_min(X2 * instance, uint32_t *dac_min);
int xep_x4driver_set_dac_max(X2 * instance, const uint32_t dac_max);
int xep_x4driver_get_dac_max(X2 * instance, uint32_t *dac_max);
int xep_x4driver_get_framebincount(X2 * instance, uint32_t *framebincount);
int xep_x4driver_set_frame_area_offset(X2 * instance, const float offset);
int xep_x4driver_set_enable(X2 * instance, const uint8_t enable);
int xep_x4driver_set_tx_center_frequency(X2 * instance, const uint8_t tx_center_frequency);
int xep_x4driver_get_tx_center_frequency(X2 * instance, uint8_t *tx_center_frequency);
int xep_x4driver_set_tx_power(X2 * instance, const uint8_t tx_power);
int xep_x4driver_get_tx_power(X2 * instance,uint8_t *tx_power);
int xep_x4driver_set_spi_register(X2 * instance, const uint8_t address, const uint8_t value);
int xep_x4driver_get_spi_register(X2 * instance, const uint8_t address, uint8_t * value);
int xep_x4driver_set_pif_register(X2 * instance, const uint8_t address, const uint8_t value);
int xep_x4driver_get_pif_register(X2 * instance, const uint8_t address, uint8_t * value);
int xep_x4driver_set_xif_register(X2 * instance, const uint8_t address, const uint8_t value);
int xep_x4driver_get_xif_register(X2 * instance, const uint8_t address, uint8_t * value);
int xep_x4driver_set_prf_div(X2 * instance, const uint8_t prf_div);
int xep_x4driver_get_prf_div(X2 * instance, uint8_t * prf_div);
int xep_x4driver_get_frame_area(X2 * instance, float * start, float * end);
int xep_x4driver_get_frame_area_offset(X2 * instance, float * offset);
int xep_set_iopin_control(X2 * instance, const uint32_t pin_id, const uint32_t pin_setup, const uint32_t pin_feature);
int xep_get_iopin_control(X2 * instance, const uint32_t pin_id, uint32_t * pin_setup, uint32_t * pin_feature);
int xep_set_iopin_value(X2 * instance, const uint32_t pin_id, const uint32_t pin_value);
int xep_get_iopin_value(X2 * instance, const uint32_t pin_id, uint32_t * pin_value);
int xep_peek_message_data_string(X2 * instance);
int xep_read_message_data_string(X2 * instance, uint32_t *content_id, uint32_t *info,
                                 char *data, uint32_t *length, uint32_t max_length);
int xep_peek_message_data_byte(X2 * instance);
int xep_read_message_data_byte(X2 * instance, uint32_t *content_id, uint32_t *info,
                               char *data, uint32_t *length, uint32_t max_length);

int nva_peek_message_baseband_iq(X2 * instance);
int nva_peek_message_baseband_ap(X2 * instance);
int nva_read_message_baseband_iq(
    X2 * instance,
    uint32_t * frame_counter,
    uint32_t * num_bins,
    float * bin_length,
    float * sample_frequency,
    float * carrier_frequency,
    float * range_offset,
    float * i_data,
    float * q_data,
    uint32_t max_length);

int nva_read_message_baseband_ap(
    X2 * instance,
    uint32_t * frame_counter,
    uint32_t * num_bins,
    float * bin_length,
    float * sample_frequency,
    float * carrier_frequency,
    float * range_offset,
    float * amplitude,
    float * phase,
    uint32_t max_length);

int nva_set_output_control(
    X2 * instance,
    const uint32_t output_feature,
    const uint32_t output_control);

int nva_get_output_control(
    X2 * instance,
    const uint32_t output_feature,
    uint32_t * output_control);    

int nva_set_debug_output_control(
    X2 * instance,
    const uint32_t output_feature,
    const uint32_t output_control);

int nva_get_debug_output_control(
    X2 * instance,
    const uint32_t output_feature,
    uint32_t * output_control);        

/********************************************************************************
 *  File API
 *******************************************************************************/
int nva_search_for_file_by_type(
    X2 * instance,
    uint32_t type,
    uint32_t * files,
    uint32_t * length,
    uint32_t max_length);


int nva_find_all_files(
    X2 * instance,
    int32_t * file_type_items,
    int32_t * file_identifier_items,
    uint32_t * items,
    uint32_t max_items);

int nva_create_file(
    X2 * interface,
    uint32_t file_type,
    uint32_t identifier,
    uint32_t length);

int nva_open_file(
    X2 * instance,
    uint32_t file_type,
    uint32_t identifier);


int nva_set_file_data(
    X2 * instance,
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    const uint8_t * data,
    uint32_t length);


int nva_close_file(
    X2 * instance,
    uint32_t type,
    uint32_t identifier,
    bool commit);

int nva_get_file_length(
    X2 * instance,
    uint32_t type,
    uint32_t identifier,
    uint32_t * length);


int nva_delete_file(
    X2 * instance,
    uint32_t type,
    uint32_t identifier);


int nva_get_file_data(
    X2 * instance,
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    uint8_t * data);

int nva_set_file(
    X2 * instance,
    uint32_t type,
    uint32_t identifier,
    uint32_t length,
    uint8_t * data);

int nva_get_file(
    X2 * instance,
    uint32_t type,
    uint32_t identifier,
    uint32_t max_length,
    uint8_t * data);

int nva_format_filesystem(
    X2 * instance,
    uint32_t key);

void nva_set_default_timeout(
    X2 * instance,
    unsigned int new_default_timeout);

int nva_get_profileid(
    X2 * instance,
    uint32_t * result);

int nva_store_noisemap(X2 * instance);

int nva_delete_noisemap(X2 * instance);

int nva_set_noisemap_control(
    X2 * instance,
    const uint32_t noisemap_control);

int nva_get_noisemap_control(
    X2 * instance,
    uint32_t *const noisemap_control);

int nva_peek_message_pulsedoppler_float(X2 * instance);
int nva_read_message_pulsedoppler_float_data(
        X2 *instance,
        uint32_t *frame_counter,
        uint32_t *matrix_counter,
        uint32_t *range_idx,
        uint32_t *range_bins,
        uint32_t *frequency_count,
        uint32_t *pulsedoppler_instance,
        float *fps,
        float *fps_decim,
        float *frequency_start,
        float *frequency_step,
        float *range,
        float *data,
        uint32_t max_length);

int nva_peek_message_pulsedoppler_byte(X2 * instance);
int nva_read_message_pulsedoppler_byte_data(
        X2 *instance,
        uint32_t *frame_counter,
        uint32_t *matrix_counter,
        uint32_t *range_idx,
        uint32_t *range_bins,
        uint32_t *frequency_count,
        uint32_t *pulsedoppler_instance,
        float *byte_step_start,
        float *byte_step_size,
        float *fps,
        float *fps_decim,
        float *frequency_start,
        float *frequency_step,
        float *range,
        uint8_t *data,
        uint32_t max_length);

int nva_peek_message_noisemap_float(X2 * instance);
int nva_read_message_noisemap_float_data(
        X2 *instance,
        uint32_t *frame_counter,
        uint32_t *matrix_counter,
        uint32_t *range_idx,
        uint32_t *range_bins,
        uint32_t *frequency_count,
        uint32_t *pulsedoppler_instance,
        float *fps,
        float *fps_decim,
        float *frequency_start,
        float *frequency_step,
        float *range,
        float *data,
        uint32_t max_length);

int nva_peek_message_noisemap_byte(X2 * instance);
int nva_read_message_noisemap_byte_data(
        X2 *instance,
        uint32_t *frame_counter,
        uint32_t *matrix_counter,
        uint32_t *range_idx,
        uint32_t *range_bins,
        uint32_t *frequency_count,
        uint32_t *pulsedoppler_instance,
        float *byte_step_start,
        float *byte_step_size,
        float *fps,
        float *fps_decim,
        float *frequency_start,
        float *frequency_step,
        float *range,
        uint8_t *data,
        uint32_t max_length);

#ifdef __cplusplus
}
#endif


#endif
