#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include "XEP.hpp"
#include "Data.hpp"
#include "xtid.h"
#include "X4M300.hpp"
#include "ModuleConnector.hpp"

/** \example XEP_configure_and_run.cpp
 */

using namespace XeThru;

void usage()
{
    std::cout << "Enter the port number of the device" << std::endl;
}

int handle_error(std::string message)
{
    std::cerr << "ERROR: " << message << std::endl;
    return 1;
}

void xep_app_init(XEP* xep)
{
    char configure;
    std::cout << "Would you like to customize XEP configurations(y/n)? ";
    std::cin >> configure;
    xep->x4driver_init();

    //Setting default values for XEP
    int dac_min = 949;
    int dac_max = 1100;
    int iteration = 32;
    int pps = 150;
    int fps = 17;
    float offset = 0.18;
    float fa1 = 0.4;
    float fa2 = 5.0;
    int dc = 0;

    if (configure == 'y') {
        //Getting user input for configuration of xep
        std::cout << "Set dac min: "<< std::endl; std::cin >> dac_min;
        std::cout << "Set dac max: "<< std::endl; std::cin >> dac_max;
        std::cout << "Set iterations: "<< std::endl; std::cin >> iteration;
        std::cout << "Set pulse per step: "<< std::endl; std::cin >> pps;
        std::cout << "Set fps: "<< std::endl; std::cin >> fps;
        std::cout << "Set frame area offset: "<< std::endl; std::cin >> offset;
        std::cout << "Set frame area closest: "<< std::endl; std::cin >> fa1;
        std::cout << "Set frame area furthest: "<< std::endl; std::cin >> fa2;
        std::cout << "Would you like to enable downconversion (1/0): " << std::endl; std::cin >> dc;
        std::cout << std::endl;
        //Configure custom XEP
    }

    std::cout << "Configuring XEP" << std::endl;
    //Writing to module XEP
    xep->x4driver_set_dac_min(dac_min);
    xep->x4driver_set_dac_max(dac_max);
    xep->x4driver_set_iterations(iteration);
    xep->x4driver_set_pulses_per_step(pps);
    xep->x4driver_set_fps(fps);
    xep->x4driver_set_frame_area_offset(offset);
    xep->x4driver_set_frame_area(fa1, fa2);
    xep->x4driver_set_downconversion(dc);
}


int read_frame(const std::string & device_name)
{
    const unsigned int log_level = 0;
    ModuleConnector mc(device_name, log_level);
    XEP & xep = mc.get_xep();

    std::string FWID;
    //If the module is a X4M200 or X4M300 it needs to be put in manual mode
    xep.get_system_info(0x02, &FWID);
    if (FWID != "XEP") {
        //Module X4M300 or X4M200
        std::string Module;
        X4M300 & x4m300 = mc.get_x4m300();
        x4m300.set_sensor_mode(XTID_SM_STOP,0);
        x4m300.set_sensor_mode(XTID_SM_MANUAL,0);
        xep.get_system_info(0x01, &Module);
        std::cout << "Module " << Module << " set to XEP mode"<< std::endl;
    }

    // Configure XEP
    xep_app_init(&xep);
    // DataFloat
    XeThru::DataFloat test;
    // Check packets queue
    int packets = xep.peek_message_data_float();
    std::cout << "Packets in queue: " << packets<<'\n';

    // Printing output until KeyboardInterrupt
    // This data is not readable.
    std::cout << "Printing out data: " << '\n';
    while (true) {
        if (xep.read_message_data_float(&test)) {
            return handle_error("read_message_data_float failed");
        }
        std::vector<float> outData = test.data;
        int sz = outData.size();
        for (int x = 0; x < sz; ++x) {
            //printing out unreadable RF-data
            std::cout << test.data[x] << " ";
        }
        std::cout << '\n';
    }
    return 0;
}

int main(int argc, char ** argv)
{
    if (argc < 2) {
        usage();
        return 2;
    }
    const std::string device_name = argv[1];
    return read_frame(device_name);
}
