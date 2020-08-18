#ifndef TRANSPORTINTERFACE_HPP
#define TRANSPORTINTERFACE_HPP

#include "Bytes.hpp"
#include "LockedRadarForward.hpp"
#include <functional>

struct RadarInterface;

namespace XeThru
{

class TransportPrivate;

class Transport
{
public:
    Transport(LockedRadarInterfacePtr & radar_interface);
    ~Transport();
    int send_command(const Bytes & command);
    Bytes send_command_single(
        const Bytes & command,
        const Bytes & comparator);
    Bytes send_command_multi(
        const Bytes & command,
        const std::vector<Bytes> & comparator);
    int subscribe(const std::string & name, const Bytes & comparator);
    int subscribe(const std::string & name, const Bytes & comparator, std::function<bool(Bytes)> callback);
    void unsubscribe(const std::string & name);
    //std::vector<Bytes> get_all_packets(const std::string & name);
    Bytes get_packet(const std::string & name);
    unsigned int get_number_of_packets(const std::string & name);
    void clear(const std::string & name);

private:
    TransportPrivate * pimpl;
};


}

#endif
