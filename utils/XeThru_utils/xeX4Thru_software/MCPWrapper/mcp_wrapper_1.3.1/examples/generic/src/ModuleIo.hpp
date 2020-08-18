#ifndef MODULEIO_HPP
#define MODULEIO_HPP

#include <string>

class ModuleIo
{
public:
    virtual ~ModuleIo(){}

    virtual int open(const std::string & device) = 0;
	virtual void setBaudrate(unsigned int baudrate) = 0;
    virtual int write(
        const unsigned char * data,
        unsigned int length) = 0;

    virtual unsigned int read(
        unsigned char * destination,
        const unsigned int max_length,
        const unsigned int millis) = 0;
};

#endif
