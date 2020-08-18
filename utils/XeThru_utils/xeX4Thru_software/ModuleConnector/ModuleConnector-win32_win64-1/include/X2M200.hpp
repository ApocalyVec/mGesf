#ifndef X2M200_INTERFACE_HPP
#define X2M200_INTERFACE_HPP

#include "Bytes.hpp"
#include "Data.hpp"
#include "RecordingOptions.hpp"
#include "LockedRadarForward.hpp"

#include <memory>
#include <functional>




namespace XeThru {

class X2M200Private;

/**
 * @class X2M200
 *
 * Interface to the Xethru X2M200 Application module
 * This interface is used to control the XeThruu X2M200 module.
 * See the data sheet of the X2M200 module for more information on the X2M200 module.
 *
 *
 */
class X2M200
{
public:
    /**
     * Constructor
     * @param[in]  a_radar_interface a reference to the internal radar interface
     */
    X2M200(LockedRadarInterfacePtr & a_radar_interface);

    /**
     * Sets debug level in the Xethru module.
     *
     * @param[in]  level New debug level. Legal range [0-9].
     * @return status : success in case of 0 / failure in any other case
     */
    int set_debug_level(unsigned char level);

    /**
     * Set log level of ModuleConnector.
     *
     * This setting does not survive a power cycle
     *
     * @param[in]  level New debug level. Legal range [0-9].
     * @return status : success in case of 0 / failure in any other case
     */
    int set_log_level(unsigned int level);

    /**
     * Send a ping message to the module.
     * This command is typically used to verify connectivity to the module.
     *
     * This setting does not survive a power cycle
     *
     * @param  pong Pointer to where to assign the resulting unsigned int response.
     * @return status : success in case of 0 / failure in any other case
     */
    int ping(unsigned int * pong);

    /**
     * Get the Novelda PCBA Item Number, including revision.
     *
     * @param[out]  item_number Pointer to where to assign the resulting string response.
     * @return status : success in case of 0 / failure in any other case
     */
    int get_item_number(std::string * item_number);

    /**
     * Get Novelda PCBA Order Code.
     *
     * @param[out] order_code Pointer to where to assign the resulting string response.
     * @return status : success in case of 0 / failure in any other case
     */
    int get_order_code(std::string * order_code);

    /**
     * Get the installed Firmware ID.
     *
     * @param[out] firmware_id Pointer to where to assign the resulting firmware id as a string.
     * @return status : success in case of 0 / failure in any other case
     */
    int get_firmware_id(std::string * firmware_id);

    /**
     * Get the installed Firmware version.
     *
     * @param[out] version  Pointer to where to assign the resulting firmware version as a string.
     * @return status : success in case of 0 / failure in any other case
     */
    int get_firmware_version(std::string * version);

    /**
     * Get the module serial number.
     *
     * @param[out] serial_number Pointer to where to assign the resulting serial number as a string.
     * @return status : success in case of 0 / failure in any other case
     */
    int get_serial_number(std::string * serial_number);

    /**
     * Get information about the SW build installed on the Xethru module.
     *
     * @param[out] build_info Pointer to where to assign the resulting build info as a string.
     * @return status : success in case of 0 / failure in any other case
     */
    int get_build_info(std::string * build_info);

    /**
     * Get a list of supported profiles.
     *
     * @param[out] app_id_list Pointer to where to assign the resulting list of supported app ids as a string.
     * @return status : success in case of 0 / failure in any other case
     */
    int get_app_id_list(std::string * app_id_list);

    /**
     * Reset the module.
     *
     * This method sends a reset command to the Xethru module. The module will try to reset and come back up again.
     * During this call the ModuleConnector will try multiple times to reconnect with the rebooted module.
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int reset();

    /**
     * * Enter the bootloader for FW upgrades.
     *
     * All the Xethru modules have a bootloader. This method instructs the Xethru module to reboot into bootloader mode.
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int start_bootloader();

    /**
     * Set the sensor in run mode.
     *
     * In run mode the sensor will send a stream of data back to the host. The
     * data in the data stream depends on the current configuration.
     *
     * This setting survives a power cycle
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int set_sensor_mode_run();

    /**
     * Set the sensor in idle mode.
     *
     * The sensor will stop sending data. The sensor can be put back in run mode with the set_sensor_mode_run().
     *
     * This setting survives a power cycle
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int set_sensor_mode_idle();

    /**
     * Load the sleep profile.
     *
     * A profile is a preset configuration that makes the module behave in a
     * certain way. The module will not start sending data until a
     * set_sensor_mode_run command is executed.
     *
     * This setting survives a power cycle
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int load_sleep_profile();

    /**
     * Load the respiration profile.
     *
     *  A profile is a preset configuration that makes the module behave in a
     * certain way. The module will not start sending data until a
     * set_sensor_mode_run command is executed.
     *
     * This setting survives a power cycle
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int load_respiration_profile();

    /**
     *  Enable amplitude/phase baseband output.
     *
     * For all supported profiles it is possible to turn on Amplitude Phase baseband output.
     *
     * This setting does not survive a power cycle
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int enable_baseband_ap();

    /**
     * Disables amplitude/phase baseband output.
     *
     * @return 0 on success, otherwise returns 1.
     */
    int disable_baseband_ap();

    /**
     * Enable I/Q baseband output.
     *
     * For all supported profiles it is possible to turn on IQ baseband output.
     *
     * This setting does not survive a power cycle
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int enable_baseband_iq();

    /**
     * Disables I/Q baseband output.
     *
     * @return 0 on success, otherwise returns 1.
     */
    int disable_baseband_iq();

    /**
     * Set the desired detection zone.
     *
     * The sensor will detect movements within this area.
     *
     * This setting survives a power cycle
     *
     * @param[in]  start Start of detection zone in meters from the radar.
     * @param[in]  end   End of detection zone in meters from the radar.
     * @return status : success in case of 0 / failure in any other case
     */
    int set_detection_zone(float start, float end);

    /**
     * Set module sensitivity.
     *
     * This setting survives a power cycle.
     *
     * @param  sensitivity Sensitivity level from 0 (low) to 9 (high).
     * @return status : success in case of 0 / failure in any other case
     */
    int set_sensitivity(uint32_t sensitivity);

    /**
     * Configures the module LED mode.
     *
     * This setting survives a power cycle.
     *
     * @param  mode      0: Off, 1: Simple, 2: Full (default).
     * @param  intensity intensity of the led
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int set_led_control(
      uint8_t mode,
      uint8_t intensity
    );

    /**
     * Adds a subscription for respiration data packets.
     *
     * Creates a queue/buffer for incoming respiration packet stream.
     * The queue must be given a name.
     *
     * This setting survives a power cycle
     *
     * @param  name Name of the buffer
     * @return status : success in case of 0 / failure in any other case
     */
    int subscribe_to_resp_status(const std::string & name);

    /**
     * Adds a subscription for respiration data packets.
     *
     * Create a subscription for for incoming respiration packet stream.
     * callback will be called for each incoming respiration packet.
     *
     * @param  name        Name of the buffer
     * @param  callback    The callback to invoke for each packet
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int subscribe_to_resp_status(
        const std::string & name,
        std::function<bool(Bytes)> callback);

    /**
     * Adds a subscription for sleep status data packets.
     *
     * Creates a queue/buffer for incoming sleep packet stream.
     * The queue must be given a name.
     *
     * @param  name Name of the queue/buffer
     * @return status : success in case of 0 / failure in any other case
     */
    int subscribe_to_sleep_status(const std::string & name);

    /**
     * Adds a subscription for sleep data packets.
     *
     * Create a subscription for for incoming sleep packet stream.
     * callback will be called for each incoming sleep packet.
     *
     * @param  name        Name of the buffer
     * @param  callback    The callback to invoke for each sleep packet
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int subscribe_to_sleep_status(
        const std::string & name,
        std::function<bool(Bytes)> callback);

    /**
     * Adds a subscription for amplitude phase baseband  data packets.
     *
     * Creates a queue/buffer for incoming amplitude phase baseband packet stream.
     * The queue must be given a name.
     *
     * @param  name Name of the queue/buffer
     * @return status : success in case of 0 / failure in any other case
     */
    int subscribe_to_baseband_ap(const std::string & name);

    /**
     * Adds a subscription for amplitude phase baseband data packets.
     *
     * Create a subscription for incoming amplitude phase baseband packet stream.
     * callback will be called for each incoming amplitude phase baseband packet.
     *
     * @param  name        Name of the buffer
     * @param  callback    The callback to invoke for each amplitude phase baseband packet
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int subscribe_to_baseband_ap(
        const std::string & name,
        std::function<bool(Bytes)> callback);

    /**
     * Adds a subscription for IQ baseband  data packets.
     *
     * Creates a queue/buffer for incoming IQ baseband packet stream.
     * The queue must be given a name.
     *
     * @param  name Name of the queue/buffer
     * @return status : success in case of 0 / failure in any other case
     */
    int subscribe_to_baseband_iq(const std::string & name);

    /**
     * Adds a subscription for IQ baseband data packets.
     *
     * Create a subscription for incoming IQ baseband packet stream.
     * callback will be called for each incoming IQ baseband packet.
     *
     * @param  name        Name of the buffer
     * @param  callback    The callback to invoke for each IQ baseband packet
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int subscribe_to_baseband_iq(
        const std::string & name,
        std::function<bool(Bytes)> callback);

    /**
     * Turns off a named subscription
     *
     * @param name Name of the subscription to disable
     */
    void unsubscribe(const std::string & name);

    /**
     * Turn off respiration status output from the Xethru X2M200 module
     *
     * This setting does not survive a power cycle
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int disable_resp_output();

    /**
     * Turn on respiration status output from the Xethru X2M200 module
     *
     * This setting does not survive a power cycle
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int enable_resp_output();

    /**
     * Get number of packets in a named subscription queue/buffer
     *
     * @param  name Name of subscription
     * @return Number of packets in the queue
     */
    unsigned int get_number_of_packets(const std::string & name);

    /**
     * Get one respiration data message from subscription queue.
     *
     * @param  name                  Name of subscription
     * @param[out]  respiration_data A data object holding the resulting
     *                               respiration data.
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int get_respiration_data(
        const std::string name,
        RespirationData * respiration_data);

    /**
     * Get one sleep data message from subscription queue.
     *
     * @param  name            Name of subscription
     * @param[out]  sleep_data A data object holding the resulting
     *                         respiration data.
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int get_sleep_data(
        const std::string name,
        SleepData * sleep_data);

    /**
     * Get one baseband data message from subscription queue.
     *
     * @param  name                  Name of subscription
     * @param[out]  baseband_ap_data A data object holding the resulting
     *                               amplitude/phase baseband data.
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int get_baseband_ap_data(
        const std::string name,
        BasebandApData * baseband_ap_data);

    /**
     * Get one baseband data message from subscription queue.
     *
     * @param  name                  Name of subscription
     * @param[out]  baseband_iq_data A data object holding the resulting
     *                               Inphase Quadrature baseband data.
     *
     * @return status : success in case of 0 / failure in any other case
     */
    int get_baseband_iq_data(
        const std::string name,
        BasebandIqData * baseband_iq_data);

    /**
     * Empty the buffer of the named data stream subscription.
     *
     * @param[in] name Name of subscription
     */
    void clear(const std::string & name);

private:
    //RadarInterface & radar_interface;
    std::shared_ptr<X2M200Private> d_ptr;
};

} // namespace Novelda

#endif
