#ifndef NOTSUPPORTEDINTERFACE_HPP
#define NOTSUPPORTEDINTERFACE_HPP

#include "Bytes.hpp"
#include "LockedRadarForward.hpp"
#include <stdint.h>
#include <string>
#include <functional>

struct RadarInterface;

namespace XeThru {

class NotSupportedPrivate;
struct DataFloat;

class NotSupported
{
public:
    NotSupported(LockedRadarInterfacePtr & radar_interface);
    ~NotSupported();
    Byte get_x4_io_pin_value(unsigned char pin);
    int set_x4_io_pin_value(unsigned char pin, unsigned char value);

    int set_x4_io_pin_mode(
        const unsigned char pin,
        const unsigned char mode);
    int set_x4_io_pin_dir(
        const unsigned char pin,
        const unsigned char direction);
    int set_x4_io_pin_enable(const unsigned char pin);
    int set_x4_io_pin_disable(const unsigned char pin);
    unsigned char read_x4_spi(const unsigned char address);
    int write_x4_spi(
        unsigned char address,
        const unsigned char value);
    int set_x4_fps(const uint32_t fps);
    int subscribe_to_x4_desim(const std::string & name);
    int get_x4_decim_frame(
        const std::string & name,
         uint32_t * frame_counter,
        double * frame_data,
        unsigned int max_length);
    int subscribe_to_data_float(const std::string & name);
    int subscribe_to_data_float(const std::string & name, std::function<bool(Bytes)> callback);
    int peek_message_data_float(const std::string & name);
    int read_message_data_float(XeThru::DataFloat * data_float);

    int peek_message_pulsedoppler_float();
    Bytes read_message_pulsedoppler_float();

    int peek_message_noisemap_float();
    Bytes read_message_noisemap_float();

    int peek_message_pulsedoppler_byte();
    Bytes read_message_pulsedoppler_byte();

    int peek_message_noisemap_byte();
    Bytes read_message_noisemap_byte();

    int get_application_user_zone(float *start, float *end);
    int set_application_user_zone(float start, float end);

    unsigned int get_number_of_packets(const std::string & name);
    Bytes get_packet(const std::string & name);
    void clear(const std::string & name);
    void set_parameter_file(
        const std::string & filename,
        const std::string & data);
    std::string get_parameter_file(
        const std::string & filename);
    void load_profile(const uint32_t profile_id);
    int subscribe_to_trace(const std::string & name);
    std::string get_trace(const std::string & name);
    int subscribe_to_data_byte(const std::string & name);
    int subscribe_to_data_byte(const std::string & name, std::function<bool(Bytes)> callback);


private:
    NotSupportedPrivate * pimpl;
};

}

#endif
