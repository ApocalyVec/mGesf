#include "ModuleConnector.hpp"
#include "X4M300.hpp"
#include <iostream>

/** \example ping_x4m300_tcpip.cpp
 */

/*
 * Create a server with, for example, connecting a module to a raspberry pi,
 * installing socat on it ("sudo apt-get install socat") and running:
 * "socat TCP4-LISTEN:3000,fork,reuseaddr GOPEN:/dev/ttyACM0,rawer".
 */

using namespace XeThru;

void usage()
{
    std::cout << "ping <ipv4> <tcp port>" << std::endl;
}


int ping(in_addr_t ip, in_port_t port)
{
    const unsigned int log_level = 5;
    ModuleConnector mc(ip, port, log_level);
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
    if (argc < 3) {
        usage();
        return 1;
    }

    int a, b, c, d;
    if (sscanf(argv[1], "%d.%d.%d.%d", &a, &b, &c, &d) != 4) {
        usage();
        return 1;
    }
    short int port = atoi(argv[2]);

    return ping(htonl(a << 24 | b << 16 | c << 8 | d), htons(port));
}
