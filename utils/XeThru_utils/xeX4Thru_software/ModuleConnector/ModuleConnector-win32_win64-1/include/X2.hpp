#ifndef X2_INTERFACE_HPP
#define X2_INTERFACE_HPP

#include <string>
#include "Bytes.hpp"
#include "NotSupportedData.hpp"

namespace XeThru {

class X2
{
public:
    virtual ~X2();
    /**
     * Set debug level.
     *
     * Sets debug level for the module.
     * Controls the verbosity of the module debug output.
     *
     * - 0 = No debug messages
     * - ...
     * - 9 = All debug messages
     *
     * @param[in]   level Module debug messages verbosity,
     *              0 = no messages, 9 = all messages
     * @return      0 = Succes, other indicates some error.
     */
    virtual void set_debug_level(unsigned char level) = 0;

    /**
     * Send a ping message to the module. Responds with a pong value to indicate
     * if the module is ready for further interaction or not.
     *
     * @return      Pongvalue. Indicates module readyness:
     *              - Ready: XTS_DEF_PONGVAL_READY (0xaaeeaeea)
     *              - Not ready: XTS_DEF_PONGVAL_NOTREADY (0xaeeaeeaa)
     */
    virtual unsigned int ping() = 0;

    /**
     * Request system info from the module.
     *
     * @param[in]  info_code Select which type of information to request:
     * - Novelda item number: XTS_SSIC_ITEMNUMBER         = 0x00
     * - Novelda ordercode: XTS_SSIC_ORDERCODE          = 0x01
     * - Firmware ID, e.g. X2M200: XTS_SSIC_FIRMWAREID         = 0x02
     * - Firmware version: XTS_SSIC_VERSION            = 0x03
     * - Firmware build information: XTS_SSIC_BUILD              = 0x04
     * - Module serial number: XTS_SSIC_SERIALNUMBER       = 0x06
     *
     * @return       0 = Succes, other indicates some error.
     */
    virtual std::string get_system_info(unsigned char info_code) = 0;

    /**
     * Calls Radarlib method NVA_X2_SetFloatVariable().
     * Refer to Radarlib support for details.
     *
     * @param[in]  id    Radarlib variable ID
     * @param[in]  value Radarlib float variable value
     * @return       0 = Succes, other indicates some error.
     */
    virtual int set_X2_float(uint32_t id, float value) = 0;

    /**
    * Calls Radarlib method NVA_X2_GetFloatVariable().
    * Refer to Radarlib support for details.
     *
     * @param[in]  id    Radarlib variable ID
     * @return      Radarlib float variable value
     */
    virtual float get_x2_float(uint32_t id) = 0;

    /**
    * Calls Radarlib method NVA_X2_SetIntVariable().
    * Refer to Radarlib support for details.
    *
    * @param[in]  id    Radarlib variable ID
    * @param[in]  value Radarlib integer variable value
     * @return       0 = Succes, other indicates some error.
     */
    virtual int set_X2_int(uint32_t id, uint32_t value) = 0;

    /**
    * Calls Radarlib method NVA_X2_GetIntVariable().
    * Refer to Radarlib support for details.
     *
     * @param[in]  id    Radarlib variable ID
     * @return      Radarlib integer variable value
     */
    virtual uint32_t get_x2_int(uint32_t id) = 0;

    /**
    * Calls Radarlib method NVA_X2_WriteRegisterFieldId().
    * Refer to Radarlib support for details.
    *
    * @param[in]  id    Radarlib register field ID
    * @param[in]  value Radarlib register field value
     * @return       0 = Succes, other indicates some error.
     */
    virtual int set_X2_register(uint32_t id, uint32_t value) = 0;

    /**
    * Calls Radarlib method NVA_X2_ReadRegisterFieldId().
    * Refer to Radarlib support for details.
    *
    * @param[in]  id    Radarlib register field ID
     * @return       Radarlib register field value
     */
    virtual uint32_t get_x2_register(uint32_t register_id) = 0;

    /**
     * Starts a timing measurement on module.
     *
     * Timing measurement runs for approximately 1 minute. It is a process to
     * calibrate the X2 radar chip in order to produce accurate results.
     *
     * During timing measurement execution, the module may be less responsive.
     * Returns when the measurement is done.
     *
     * @return       0 = Succes, other indicates some error.
     */
    virtual int run_timing_measurement() = 0;

    /**
     * Select module sweep control frequency. When in a running mode, the
     * module will generate continous radar sweeps at a fixed time interval.
     *
     * @param[in] fps  Frames Per second
     *
     * @return       0 = Succes, other indicates some error.
     */
    virtual int set_fps(unsigned int fps) = 0;

    /**
     * Start a single sweep and capture the radar data.
     *
     * @return       Normalized radar frame
     */
    virtual RawNormalizedData capture_single_normalized_frame() = 0;

    /**
    * Control the sensor module mode of execution.
    *
    * When the module is configured to run an application, such as X2M200,
    * this method is used to control the execution.
    * It can also be used to configure the module to a MANUAL mode where
    * it sends raw data from the X2 chip directly.
    *
    * @param[in]  mode List of valid modes are:
    *     - Application normal execution: XTS_SM_RUN = 0x01
    *     - Pause application execurion: XTS_SM_IDLE = 0x11
    *     - Disable application execution, stream raw data directly from X2: XTS_SM_MANUAL = 0x12
    *
    * @return       0 = Succes, other indicates some error.
    *
    */
    virtual void set_sensor_mode(unsigned char mode) = 0;

    /**
    * Legacy command. Do not use.
     */
    virtual void set_sensor_mode_and_submode(
        const unsigned char mode,
        const unsigned char submode) = 0;

    /**
     * Start a subscription on raw normalized radar data.
     *
     * @param[in]  name Subscription identification
     * @return       0 = Succes, other indicates some error.
     */
    virtual int subscribe_to_raw_normalized(const std::string & name) = 0;

    /**
     * Start a subscription on raw normalized radar data.
     *
     * @param[in]  name  Subscription identification
     * @param[in]  callback Callback method when data is available
     * @return       0 = Succes, other indicates some error.
     */
    virtual int subscribe_to_raw_normalized(const std::string & name, std::function<bool(Bytes)> callback) = 0;

    /**
     * Get a single frame from subscription.
     *
     * Get a frame from the input buffer from the subscription started with
     * subscribe_to_raw_normalized.
     *
     * @param[in]  name Subscription identification
     * @return       0 = Succes, other indicates some error.
     */
    virtual RawNormalizedData get_raw_normalized_frame(const std::string & name) = 0;

    /**
     * Unsubsribe from a previously enabled data subscription.
     *
     * @param[in] name Subscription identification
     */
    virtual void unsubscribe(const std::string & name) = 0;

    /**
     * Get a number of packets available in given input buffer.

     * @param[in]  name Subscription identification
     * @return      Number of packets
     */
    virtual unsigned int get_number_of_packets(const std::string & name) = 0;

    /**
     * Clears the in-buffer of the named subscription.
     *
     * @param[in] name Subscription identification
     */
    virtual void clear(const std::string & name) = 0;

    /**
     * run system test during production
     *
     * @param[in] test_code test code to run
     */
    virtual int system_run_test(unsigned char test_code) = 0;

};

}
#endif
