#include "ModuleConnector.hpp"
#include "X4M300.hpp"
#include "xtid.h"
#include <iostream>

/** \example presence_single.cpp
 * this is a small ModuleConnector usage example
 */

using namespace XeThru;

void usage()
{
    std::cout << "presence_single <com port or device file>" << std::endl;
}

int handle_error(std::string message)
{
    std::cerr << "ERROR: " << message << std::endl;
    return 1;
}

int presence_single(const std::string & device_name)
{
    const unsigned int log_level = 0;
    ModuleConnector mc(device_name, log_level);
    X4M300 & x4m300 = mc.get_x4m300();

    // ignore status. This might fail and that is ok.
    std::cout << "Stop the module" << std::endl;
    x4m300.set_sensor_mode(XTID_SM_STOP, 0);


    std::cout << "Load presence profile" << std::endl;
    const unsigned int ProfileID = 0x014d4ab8;
    if (x4m300.load_profile(ProfileID)) {
        return handle_error("load_profile failed");
    }

    std::cout << "Turn on presence single packages" << std::endl;
    if (x4m300.set_output_control(XTS_ID_PRESENCE_SINGLE, XTID_OUTPUT_CONTROL_ENABLE)) {
        return handle_error("set output controll failed");
    }

    // start the module and profile
    std::cout << "Set the module in RUN state" << std::endl;
    if (x4m300.set_sensor_mode(XTID_SM_RUN, 0)) {
        return handle_error("Set sensor mode to running failed");
    }

    std::cout << "Block until first package of presence arrives" << std::endl;
    std::cout << "This may take some time..." << std::endl;
    XeThru::PresenceSingleData presence_single;
    if (x4m300.read_message_presence_single(&presence_single)) {
        return handle_error("set output controll failed");
    }

    std::cout << "frame_counter:  " << presence_single.frame_counter << std::endl;
    std::cout << "presence_state: " << presence_single.presence_state << std::endl;
    std::cout << "distance:       " << presence_single.distance << std::endl;
    std::cout << "direction:      " << static_cast<unsigned int>(presence_single.direction) << std::endl;
    std::cout << "signal_quality: " << presence_single.signal_quality << std::endl;


    std::cout << "Wait for another one presence package" << std::endl;
    if (x4m300.read_message_presence_single(&presence_single)) {
        return handle_error("set output controll failed");
    }

    std::cout << "frame_counter:  " << presence_single.frame_counter << std::endl;
    std::cout << "presence_state: " << presence_single.presence_state << std::endl;
    std::cout << "distance:       " << presence_single.distance << std::endl;
    std::cout << "direction:      " << static_cast<unsigned int>(presence_single.direction) << std::endl;
    std::cout << "signal_quality: " << presence_single.signal_quality << std::endl;


    std::cout << "Set the module in STOP state" << std::endl;
    if (x4m300.set_sensor_mode(XTID_SM_STOP, 0)) {
        return handle_error("set output controll failed");
    }

    return 0;
}


int main(int argc, char ** argv)
{
    if (argc < 2) {
        usage();
        return -1;
    }

    const std::string device_name = argv[1];

    return presence_single(device_name);
}
