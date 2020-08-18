#ifndef OVERLAPPEDMODULEIO_HPP
#define OVERLAPPEDMODULEIO_HPP

#include "ModuleIo.hpp"
#include <windows.h>
#include <string>

class OverlappedModuleIo : public ModuleIo
{
public:
    OverlappedModuleIo();
    ~OverlappedModuleIo();
    virtual int open(const std::string & device_name);
	virtual void setBaudrate(unsigned int baudrate);

    virtual int write(
        const unsigned char * data,
	unsigned int length);

    virtual unsigned int read(
        unsigned char * destination,
	const unsigned int max_length,
	const unsigned int millis);

private:
    void setTimeout(unsigned int ms);
    void reconfigurePort();
    int do_single_read(unsigned char * destination, unsigned int length);

    HANDLE hSerial;
	unsigned int baudrate_;
};



#endif
