#ifndef ABSTRACTLOGGERIO_HPP
#define ABSTRACTLOGGERIO_HPP

namespace XeThru {

class AbstractLoggerIo
{
public:
    virtual ~AbstractLoggerIo(){}
    virtual void log(const std::string & message) = 0;
};

}

#endif
