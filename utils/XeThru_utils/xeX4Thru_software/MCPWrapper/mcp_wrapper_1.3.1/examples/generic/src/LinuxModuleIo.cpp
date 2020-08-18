#include "LinuxModuleIo.hpp"

#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <iostream>


LinuxModuleIo::LinuxModuleIo()
    :
    tty_fd(-1),
    filename("")
{
    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;
}

LinuxModuleIo::~LinuxModuleIo()
{
    if(tty_fd < 0)
        return;

    ::close(tty_fd);
}


int LinuxModuleIo::open(const std::string & device)
{
    filename = device;
    tty_fd=::open(filename.c_str(), O_RDWR);

    if(tty_fd < 0) {
        return tty_fd;
    }

    cfsetospeed(&tio,B115200);            // 115200 baud
    cfsetispeed(&tio,B115200);            // 115200 baud
    tcsetattr(tty_fd,TCSANOW,&tio);
    return 0;
}

void LinuxModuleIo::setBaudrate(unsigned int baudrate)
{
    if(tty_fd < 0) {
        return;
    }

	baudrate_ = baudrate;
    speed_t speed = B0;
    switch (baudrate_)
    {
        case 115200:
            speed = B115200;
            break;
        case 921600:
            speed = B921600;
            break;
        default:
            speed = B115200;
            break;
    }
    cfsetospeed(&tio,speed);
    cfsetispeed(&tio,speed);            
    tcsetattr(tty_fd,TCSANOW,&tio);
}


int LinuxModuleIo::write(
    const unsigned char * data,
    unsigned int length)
{
    return ::write(tty_fd, data, length);
}


unsigned int LinuxModuleIo::read(
    unsigned char * destination,
    const unsigned int max_length,
    const unsigned int  /*millis*/)
{
    fd_set set;

    FD_ZERO (&set);
    FD_SET (tty_fd, &set);

    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;

    int result = select(
        tty_fd+1,
        &set,
        NULL,
        NULL,
        &timeout);

    if (result <= 0) {
        return result;
    }

    const unsigned int len = ::read(tty_fd, destination, max_length);

    return len;
}
