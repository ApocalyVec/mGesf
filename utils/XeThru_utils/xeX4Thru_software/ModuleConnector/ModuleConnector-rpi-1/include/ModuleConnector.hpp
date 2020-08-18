#ifndef MODULECONNECTOR_HPP
#define MODULECONNECTOR_HPP

#include "Bytes.hpp"

#include <string>
#include <memory>
#include <iostream>

#if defined(_WIN32) || defined(__MINGW32__)
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
#else
#include <netinet/in.h>
#endif

namespace XeThru {

class X2M200;
class XEP;
class X4M300;
class X4M200;
class X4M210;
class X2;
class NotSupported;
class Transport;
class AbstractLoggerIo;
class DataRecorder;
class DataPlayer;
class FBDataRecorder;
class RadarEngine;
class LaptopPresenceApp;
class ModuleConnectorImpl;

/**
 * @class ModuleConnector
 *
 * This class is responsible for establishing contact with the Xethru module.
 * Use one of the get_XXX to access the modules in various ways.
 */
class ModuleConnector
{
public:

    /**
     * Constructor
     *
     * The constructor will open the device file or COM port given.
     *
     * @param device_name Name of the device file for example /dev/ttyACM0 or COM4
     * @param log_level     The log level to use during operation
     * @param logger_io     The logging io implementation to use
     *
     */
    ModuleConnector(
        const std::string & device_name,
        int log_level,
        AbstractLoggerIo * logger_io);

    /**
     * Constructor
     *
     * The constructor will open the device file or COM port given.
     *
     * @param device_name Name of the device file for example /dev/ttyACM0 or COM4
     *
     */
    ModuleConnector(const std::string & device_name);

    /**
     * Constructor
     *
     * The constructor will open the device file or COM port given.
     *
     * @param device_name Name of the device file for example /dev/ttyACM0 or COM4
     * @param log_level     The log level to use during operation
     *
     */
    ModuleConnector(const std::string & device_name, int log_level);

    /**
     * Constructor
     *
     * When constructing ModuleConnector with a \ref DataPlayer object, ModuleConnector will
     * read data from a recording via the \a player reference and convert disk data into
     * binary packets as one would normally receive from a physical XeThru Device.
     *
     * Moreover, it is possible to control the output via functions such as
     * \ref DataPlayer::play, DataPlayer::pause, DataPlayer::set_filter, DataPlayer::set_playback_rate.
     *
     * @param player        DataPlayer reference
     * @param log_level     The log level to use during operation
     * @param logger_io     The logging io implementation to use
     *
     * @see DataPlayer
     */
    ModuleConnector(DataPlayer &player, int log_level, AbstractLoggerIo *logger_io);

    /**
     * Constructor
     *
     * When constructing ModuleConnector with a \ref DataPlayer object, ModuleConnector will
     * read data from a recording via the \a player reference and convert disk data into
     * binary packets as one would normally receive from a physical XeThru Device.
     *
     * Moreover, it is possible to control the output via functions such as
     * \ref DataPlayer::play, DataPlayer::pause, DataPlayer::set_filter, DataPlayer::set_playback_rate.
     *
     * @param player        DataPlayer reference
     * @param log_level     The log level to use during operation
     *
     * @see DataPlayer
     */
    ModuleConnector(DataPlayer &player, int log_level);

    /**
     * Constructor
     *
     * When constructing ModuleConnector with a \ref DataPlayer object, ModuleConnector will
     * read data from a recording via the \a player reference and convert disk data into
     * binary packets as one would normally receive from a physical XeThru Device.
     *
     * Moreover, it is possible to control the output via functions such as
     * \ref DataPlayer::play, DataPlayer::pause, DataPlayer::set_filter, DataPlayer::set_playback_rate.
     *
     * @param player        DataPlayer reference
     *
     * @see DataPlayer
     */
    ModuleConnector(DataPlayer &player);

    /**
     * Constructor
     *
     * The constructor will open the address and port given using TCP/IP.
     *
     * XXX: The open/close mechanisms will give undefined behaviour if started
     * this way.
     *
     * @param ip            The IP to connect to in network byte order
     * @param port          The TCP port to connect to in network byte order
     * @param log_level     The log level to use during operation
     * @param logger_io     The logging io implementation to use
     *
     */
    ModuleConnector(
        in_addr_t ip,
        in_port_t port,
        int log_level,
        AbstractLoggerIo * logger_io);

    /**
     * Constructor
     *
     * The constructor will open the address and port given using TCP/IP.
     *
     * XXX: The open/close mechanisms will give undefined behaviour if started
     * this way.
     *
     * @param ip            The IP to connect to in network byte order
     * @param port          The TCP port to connect to in network byte order
     */
    ModuleConnector(in_addr_t ip, in_port_t port);

    /**
     * Constructor
     *
     * The constructor will open the address and port given using TCP/IP.
     *
     * XXX: The open/close mechanisms will give undefined behaviour if started
     * this way.
     *
     * @param ip            The IP to connect to in network byte order
     * @param port          The TCP port to connect to in network byte order
     * @param log_level     The log level to use during operation
     *
     */
    ModuleConnector(in_addr_t ip, in_port_t port, int log_level);

    /**
     * Destructor
     *
     * It invoces close() and cleans up all resources in use.
     * It will no longer be possible to use this object or any of its interfaces.
     *
     */
    virtual ~ModuleConnector();

    /**
     * Opens a new connection to a module via some serial device
     *
     * @param device_name Name of the device file for example /dev/ttyACM0 or COM4
     */
    int open(const std::string device_name);

    /**
     * Close an open connection to the module
     *
     * @return 0 on success, otherwise return 1
     */
    void close();


    /**
     * Provides the git sha of the ModuleConnector repository
     *
     */
    std::string git_sha();

    /**
     * Set log level during ModuleConnector operation
     *
     * @param new_log_level The new log level to use
     */
    void set_log_level(int new_log_level);

    /**
     * Set default timeout for commands sent to the module.
     *
     * @param new_default_timeout
     */
    void set_default_timeout(unsigned int new_default_timeout);

    /**
     * Returns a reference to the DataRecorder application interface
     * @return DataRecorder The reference to the DataRecorder interface
     * @see DataRecorder
     */
    DataRecorder & get_data_recorder();

    /**
     * Returns a recorder using the flatbuffers format.
     * @see FBDataRecorder
     */
    FBDataRecorder &get_fb_data_recorder();

    /**
     * Not supported
     * @return Not supported
     */
    Transport & get_transport();

    /**
     * Returns a reference to the X2M200 module application interface
     * @return X2M200 The reference to the X2M200 interface
     */
    X2M200 & get_x2m200();

    /**
     * Not supported
     * @return Not supported
     */
    X2 & get_x2();

    /**
     * Returns a reference to the low level XEP interface.
     *
     * @return The reference to the XEP interface.
     *
     */
    XEP & get_xep();

    /**
     * Returns a reference to the X4M300 module application interface.
     *
     * @return The reference to the X4M300 interface.
     *
     */
    X4M300 & get_x4m300();


    /**
     * Returns a reference to the X4M200 module application interface.
     *
     * @return The reference to the X4M200 interface.
     *
     */
    X4M200 & get_x4m200();

    /**
     * Returns a reference to the X4M210 module application interface.
     *
     * @return The reference to the X4M210 interface.
     *
     */
    X4M210 & get_x4m210();

    /**
     * Not supported
     * @return Not supported
     */
    NotSupported & get_not_supported();

    /**
     * Returns a reference to the Radar Engine application interface.
     *
     * @return The reference to the Radar Engine interface.
     *
     */
    RadarEngine & get_radar_engine();

    /**
     * Returns a reference to the Laptop Presence application interface.
     *
     * @return The reference to the Laptop Presence application interface.
     *
     */
    LaptopPresenceApp & get_laptop_presence_app();

private:
    std::unique_ptr<ModuleConnectorImpl> pimpl;
};

} // namespace

#endif
