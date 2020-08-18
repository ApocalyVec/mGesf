#include "ModuleConnector.hpp"
#include "X4M300.hpp"
#include <iostream>

/** \example ping_x4m300.cpp
 * this is a small ModuleConnector usage example
 */

using namespace XeThru;

void usage()
{
    std::cout << "ping <com port or device file>" << std::endl;
}


int ping(const std::string & device_name)
{
    const unsigned int log_level = 5;
    ModuleConnector mc(device_name, log_level);
    X4M300 & x4m300 = mc.get_x4m300();
    unsigned int pong = 0;
    const int status = x4m300.ping(&pong);

    if(status != 0) {
        std::cout << "Something went wrong - error code: " << status << std::endl;
        return status;
    }

    std::cout << "pong: " << pong << std::endl;
    return 0;
}


int main(int argc, char ** argv)
{
    if (argc < 2) {
        usage();
        return -1;
    }

    const std::string device_name = argv[1];

    return ping(device_name);
}
