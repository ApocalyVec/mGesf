#include "ModuleConnector.hpp"
#include "X4M300.hpp"
#include "xtid.h"

#include <iostream>


#define NUM_PD_MESSAGES 60000

/** \example pulsedoppler.cpp
 * this is a small usage example for getting pulsedoppler data
 */

using namespace XeThru;

void usage()
{
    std::cout << "pulsedoppler <com port or device file>" << std::endl;
}

int pulsedoppler(const std::string & device_name)
{
    const unsigned int log_level = 0;
    ModuleConnector mc(device_name, log_level);
    X4M300 & x4m300 = mc.get_x4m300();

    uint32_t pong;
    if (x4m300.ping(&pong) != 0) {
        std::cerr << "Error: ping failed. Module connected?" << std::endl;
        return 1;
    }

    if (x4m300.load_profile(XTS_ID_APP_PRESENCE_2) != 0) {
        if (x4m300.load_profile(XTS_ID_APP_RESPIRATION_2) != 0) {
            std::cerr << "Error: load profile failed." << std::endl;
            return 1;
        }
    }

    if (x4m300.set_output_control(XTS_ID_PULSEDOPPLER_FLOAT,
                XTID_OUTPUT_CONTROL_PD_FAST_ENABLE |
                XTID_OUTPUT_CONTROL_PD_SLOW_ENABLE) != 0) {
        std::cerr << "Error: set output control failed." << std::endl;
        return 1;
    }

    if (x4m300.set_sensor_mode(XTID_SM_RUN, 0) != 0) {
        std::cerr << "Error: couldn't set sensor mode." << std::endl;
        return 1;
    }

    for (int i = 0; i < NUM_PD_MESSAGES; ++i) {
        PulseDopplerFloatData data;
        if (x4m300.read_message_pulsedoppler_float(&data) != 0) {
            std::cerr << "Error: couldn't read pd message #" << i << std::endl;
            return 1;
        }
        std::cout << "PD " << data.pulsedoppler_instance << ", frame " <<
            data.frame_counter << ", range idx " << data.range_idx << std::endl;
    }

    x4m300.set_sensor_mode(XTID_SM_STOP, 0);

    return 0;
}


int main(int argc, char ** argv)
{
    if (argc < 2) {
        usage();
        return -1;
    }

    const std::string device_name = argv[1];

    return pulsedoppler(device_name);
}
