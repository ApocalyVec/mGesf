#ifndef BOOTLOADERINTERFACE_HPP
#define BOOTLOADERINTERFACE_HPP

#include "Bytes.hpp"
#include <memory>

#if defined(_WIN32) || defined(__MINGW32__)
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
#else
#include <netinet/in.h>
#endif

struct Logger;

namespace XeThru {
class AbstractLoggerIo;
class BootloaderImpl;

class Bootloader
{
public:
    Bootloader(
        const std::string & device_name,
        bool log_to_stdout,
        bool log_to_file,
        unsigned int log_level);
    Bootloader(
        const std::string & device_name,
        unsigned int log_level);
    Bootloader(
        const std::string & device_name,
        unsigned int log_level,
        AbstractLoggerIo * logger_io);
    Bootloader(
        const std::string & device_name,
        Logger * logger);
    Bootloader(
        in_addr_t ip,
        in_port_t port,
        int log_level,
        AbstractLoggerIo * logger_io);
    Bootloader(in_addr_t ip, in_port_t port);
    Bootloader(in_addr_t ip, in_port_t port, int log_level);
    ~Bootloader();

    int write_page(
        unsigned int page_address,
        const Bytes & page_data);
    int start_application();
    int start_application(unsigned int timeout);
    std::string get_bootloader_info();

private:
    std::unique_ptr<BootloaderImpl> pimpl;
};

}

#endif
