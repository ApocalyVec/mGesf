#ifndef TTY_HPP
#define TTY_HPP

#include "ModuleIo.hpp"
#include <termios.h>
#include <thread>
#include <string>



class LinuxModuleIo : public ModuleIo
{
public:
    LinuxModuleIo();
    ~LinuxModuleIo();

    virtual int open(const std::string & device);
    virtual void setBaudrate(unsigned int baudrate);

    virtual int write(
        const unsigned char * data,
        unsigned int length);

    virtual unsigned int read(
        unsigned char * destination,
        const unsigned int max_length,
        const unsigned int millis);

private:

    int tty_fd;
    struct termios tio;
    std::string filename;
    unsigned int baudrate_;
};


#endif
