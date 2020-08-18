#ifndef XEP_INTERFACE_H
#define XEP_INTERFACE_H

#include "RecordingOptions.hpp"
#include "LockedRadarForward.hpp"
#include "Data.hpp"

#include <cinttypes>
#include <string>
#include <vector>
#include <memory>

struct RadarInterface;

namespace XeThru {

struct FrameArea;
struct DataFloat;
class XEPPrivate;

/**
 * @class XEP
 *
 * XEP class gives access to XEP functionality on target via module connector.
 *
 */
class XEP
{
public:
    /**
    * @brief XEP constructor.
    *
    * @return instance
    */
    XEP(LockedRadarInterfacePtr & radar_interface);

   /**
    * @brief XEP destructor
    *
    * @return destroys instance.
    */
    ~XEP();

    /**
    * @brief Resets module
    *
    * @return execution status.
    */
    int module_reset();


    /**
     * Set baudrate for serial communication during ModuleConnector operation.
     *
     * @param baudrate: enum representing the baudrate, defined in xtid.h
     */
    int set_baudrate(uint32_t baudrate);

    /**
    * @brief Returns a string containing system information given by infocode:
    XTS_SSIC_FIRMWAREID = 0x02 -> Returns the installed Firmware ID, "XEP"
    XTS_SSIC_VERSION = 0x03 -> Returns the installed Firmware Version.
    XTS_SSIC_BUILD = 0x04 -> Returns information of the SW Build installed on the device
    XTS_SSIC_VERSIONLIST = 0x07 ->
    Returns ID and version of all components. Calls all components and compound a string.
    E.g. "XEP:2.3.4.5;X4C51:1.0.0.0"
    *
    * @return execution status
    */
    int get_system_info(uint8_t info_code, std::string *system_info);

    /**
    * @brief Send ping to module in order to validate that connection both ways is OK
    *
    * @return execution status
    */
    int ping(uint32_t *pong_value);


    int set_normalization(uint8_t normalization);
    int get_normalization(uint8_t * normalization);
    int set_phase_noise_correction(uint8_t enable, float correction_distance);
    int get_phase_noise_correction(float * correction_distance);
    int set_decimation_factor(uint32_t decimation_factor);
    int get_decimation_factor(uint32_t * decimation_factor);
    int set_number_format(uint8_t number_format);
    int get_number_format(uint8_t * number_format);
    int set_legacy_output(uint8_t legacy_output);
    int get_legacy_output(uint8_t * legacy_output);

    /**
    * @brief Sets frame rate for frame streaming.
    *
    * @return execution status
    */
    int x4driver_set_fps(float fps);

    /**
    * @brief Gets configured FPS.
    *
    * @return execution status
    */
    int x4driver_get_fps(float * fps);

    /**
    * @brief Set enable for X4 enable pin.
    *
    * @return execution status
    */
    int x4driver_set_enable(uint8_t value);

    /**
    * @brief Will make sure that enable is set, X4 controller is programmed, ldos are enabled, and that the external oscillator has been enabled.
    *
    * @return execution status
    */
    int x4driver_init();

    /**
    * @brief Sets Iterations. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_set_iterations(uint32_t iterations);

    /**
    * @brief Gets Iterations. See X4 datasheet for details.
    *
    * @return execution status
    */    
    int x4driver_get_iterations(uint32_t *iterations);

    /**
    * @brief Sets pulses per step. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_set_pulses_per_step(uint32_t pps);
    /**
    * @brief Gets pulses per step. See X4 datasheet for details.
    *
    * @return execution status
    */    
    int x4driver_get_pulses_per_step(uint32_t *pps);

    /**
    * @brief Sets dac min. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_set_dac_min(uint32_t dac_min);
    /**
    * @brief Gets dac min. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_get_dac_min(uint32_t *dac_min);

    /**
    * @brief Sets dac max. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_set_dac_max(uint32_t dac_max);
    /**
    * @brief Gets dac max. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_get_dac_max(uint32_t *dac_max);

    /**
    * @brief Set the radar transmitter power. 0 = transmitter off. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_set_tx_power(uint8_t tx_power);


     /**
    * @brief Get the radar transmitter power. 0 = transmitter off. See X4 datasheet for details.
    *
    * @return execution status
    */
   int x4driver_get_tx_power(uint8_t *tx_power);

    /**
    * @brief Sets downconversion. 0=no downconversion, i.e. rf data. 1=downconversion.
    *
    * @return execution status
    */
    int x4driver_set_downconversion(uint8_t enable);
    /**
    * @brief Gets downconversion. 0=no downconversion, i.e. rf data. 1=downconversion.
    *
    * @return execution status
    */
    int x4driver_get_downconversion(uint8_t *enable);

    /**
    * @brief Gets Number of bins
    *
    * @return execution status
    */
    int x4driver_get_frame_bin_count(uint32_t *bins);

    /**
    * @brief Set frame area, i.e. the range in distance where the radar is observing. Assume air as transmitter medium. Start and end in meter.
    *
    * @return execution status
    */
    int x4driver_set_frame_area(float start, float end);

    /**
    * @brief Get frame area
    *
    * @return execution status
    */
    int x4driver_get_frame_area(FrameArea * frame_area);

    /**
    * @brief Set offset to adjust frame area reference point (location of frame area start at zero) depending on sensor hardware.
    *
    * @return execution status
    */
    int x4driver_set_frame_area_offset(float offset);

    /**
    * @brief Get offset to adjust frame area reference.
    *
    * @return execution status
    */
    int x4driver_get_frame_area_offset(float * offset);

    /**
    * @brief Set radar TX center frequency
    *
    * @return execution status
    */
    int x4driver_set_tx_center_frequency(uint8_t tx_frequency);
    /**
    * @brief Get radar TX center frequency
    *
    * @return execution status
    */    
    int x4driver_get_tx_center_frequency(uint8_t *tx_frequency);

    /**
    * @brief Set spi register on radar chip. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_set_spi_register(uint8_t address, uint8_t value);

    /**
    * @brief Get spi register on radar chip. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_get_spi_register(uint8_t address, uint8_t *value);

    /**
    * @brief Sets PIF register value. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_set_pif_register(uint8_t address, uint8_t value);

    /**
     * Write to a spi register on radar chip.
     *
     * @param address Specifies the address
     * @param values Specifies the values
     * @return execution status
     */
    int x4driver_write_to_spi_register(uint8_t address, const Bytes &values);

    /**
     * Read from a spi register
     *
     * @param address Specifies the address
     * @param values The result is copied into this parameter
     * @param length Specifies how many bytes to read
     * @return execution status
     */
    int x4driver_read_from_spi_register(uint8_t address, Bytes *values, const uint32_t length);

    /**
     * Write to a i2c register on radar chip.
     *
     * @param address Specifies the address
     * @param values Specifies the values
     * @return execution status
     */
    int x4driver_write_to_i2c_register(uint8_t address, const Bytes &values);

    /**
     * Read from a i2c register
     *
     * @param values The result is copied into this parameter
     * @param length Specifies how many bytes to read
     * @return execution status
     */
    int x4driver_read_from_i2c_register(Bytes *values, const uint32_t length);
    

    /**
    * @brief Gets PIF register value. See X4 datasheet for details.
    *
    * @return  execution status
    */
    int x4driver_get_pif_register(uint8_t address, uint8_t *value);

    /**
    * @brief Sets XIF register value. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_set_xif_register(uint8_t address, uint8_t value);

    /**
    * @brief Gets XIF register value. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_get_xif_register(uint8_t address, uint8_t *value);

    /**
    * @brief Sets Pulse Repetition Frequency(PRF) divider. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_set_prf_div(uint8_t prf_div);

    /**
    * @brief Gets Pulse Repetition Frequency(PRF) divider. See X4 datasheet for details.
    *
    * @return execution status
    */
    int x4driver_get_prf_div(uint8_t *prf_div);

    /**
    * @brief Enable or disable GPIO feature.
    * pin_id : which io pin to configure
    *
    * pin_setup : control pin setup
    * 0 = input
    * 1 = output
    *
    * pin_feature : control pin feature
    * 0 = Disable all iopin features ( not available, will return error)
    * 1 = Configure according to datasheet default( not available, will return error)
    * 2 = Passive - Set and get iopin level from host
    *
    * @return execution status
    */
    int set_iopin_control(uint32_t pin_id, uint32_t pin_setup, uint32_t pin_feature);

    /**
    * @brief If IO pin control is used to set pin_id as output, the pin level or value will be set to pin_value.
    *
    * @return execution status
    */
    int set_iopin_value(uint32_t pin_id, uint32_t pin_value);

    /**
    * @brief Read IO pin level or value.
    *
    * @return execution status
    */
    int get_iopin_value(uint32_t pin_id, uint32_t *pin_value);

    /**
    * @brief Returns number of data float packets in internal queue.
    *
    * @return Returns number of data float packets in internal queue
    */
    int peek_message_data_float();

    /**
    * @brief Reads a single data float message from internal queue.
    *
    * @return execution status
    */
    int read_message_data_float(XeThru::DataFloat * data_float);

    /**
     * Return number of RadarRf messages available in queue.
     *
     * @return size: number of messages in buffer
     */
    int peek_message_radar_rf();

    /**
     * Read a single RadarRf item from the queue. Blocks if queue is empty.
     *
     * @param[out] radar_rf: pointer to returned RadarRfData item
     * @return status : success in case of 0 / failure in any other case
     */
    int read_message_radar_rf(XeThru::RadarRfData * radar_rf);

    /**
     * Return number of RadarRfNormalized messages available in queue.
     *
     * @return size: number of messages in buffer
     */
    int peek_message_radar_rf_normalized();

    /**
     * Read a single RadarRfNormalized item from the queue. Blocks if queue is empty.
     *
     * @param[out] radar_rf_normalized: pointer to returned RadarRfNormalizedData item
     * @return status : success in case of 0 / failure in any other case
     */
    int read_message_radar_rf_normalized(XeThru::RadarRfNormalizedData * radar_rf_normalized);

    /**
     * Return number of RadarBasebandFloat messages available in queue.
     *
     * @return size: number of messages in buffer
     */
    int peek_message_radar_baseband_float();

    /**
     * Read a single RadarBasebandFloat item from the queue. Blocks if queue is empty.
     *
     * @param[out] radar_baseband_float: pointer to returned RadarBasebandFloatData item
     * @return status : success in case of 0 / failure in any other case
     */
    int read_message_radar_baseband_float(XeThru::RadarBasebandFloatData * radar_baseband_float);

    /**
     * Return number of RadarBasebandQ15 messages available in queue.
     *
     * @return size: number of messages in buffer
     */
    int peek_message_radar_baseband_q15();

    /**
     * Read a single RadarBasebandQ15 item from the queue. Blocks if queue is empty.
     *
     * @param[out] radar_baseband_q15: pointer to returned RadarBasebandQ15 item
     * @return status : success in case of 0 / failure in any other case
     */
    int read_message_radar_baseband_q15(XeThru::RadarBasebandQ15Data * radar_baseband_q15);

    /**
    * @brief Returns number of data string packets in internal queue.
    *
    * @return  Returns number of data string packets in internal queue.
    */
    int peek_message_data_string();

    /**
    * @brief Reads a single data string message from internal queue.
    *
    * @return execution status
    */
    int read_message_data_string(uint32_t *content_id, uint32_t *info, std::string *data);

    /**
     * Returns number of data byte packets in internal queue.
     *
     * @return the number of data byte packets in internal queue.
     */
    int peek_message_data_byte();

    /**
     * Reads a single data byte message from internal queue.
     *
     * @return execution status
     */
    int read_message_data_byte(uint32_t *content_id, uint32_t *info, Bytes *data);

   /**
    * @brief Returns number of system packets in internal queue.
    *
    * @return void
    */
   int peek_message_system();

    /**
    * @brief Reads a single data system message from internal queue.
    *
    * @return execution status
    */
    int read_message_system(uint32_t *responsecode);

    /**
     * @brief Searches for and returns a list of identifiers for all files of the specified type.
     *
     * @param type: The type of file.
     * @param result: vector of file identifiers are copied here as result.
     *
     * @return execution status
     */
    int search_for_file_by_type(uint32_t type, std::vector<int32_t> & result);

    /**
     * @brief Finds all files and stores the type and identifier of each file in the given buffers.
     *
     * @param result: The result from the search is copied into the result parameter.
     *
     * @return execution status
     */
    int  find_all_files(XeThru::Files & result);

    /**
     * @brief Creates and opens a new file with given type, identifiers and length.
     *
     * @param type: The type of file.
     * @param identifier: The identifer of the file.
     * @param length: length of the file to create
     *
     * @return execution status
     */
    int create_file(uint32_t file_type, uint32_t identifier, uint32_t length);

    /**
     * @brief Opens the file with given type band identifiers.
     *
     * @param type: The type of file.
     * @param identifier: The identifer of the file.
     *
     * @return execution status
     */
    int open_file(uint32_t file_type, uint32_t identifier);

    /**
     * @brief Writes data at offset to the file.
     *
     * @param type: The type of file.
     * @param identifier: The identifer of the file.
     * @param offset: Offset where to write the data to.
     * @param data: The data to write.
     *
     * @return execution status
     */
    int set_file_data(
        uint32_t type,
        uint32_t identifier,
        uint32_t offset,
        const Bytes & data);

    /**
     * @brief Close the file.
     *
     * @param type: The type of file.
     * @param identifier: The identifer of the file.
     * @param commit: wheter to save changes made to the file closing.
     *
     * @return execution status
     */
    int close_file(uint32_t type, uint32_t identifier, bool commit);

    /**
     * @brief Gets the length of a file.
     *
     * @param type: The type of file.
     * @param identifier: The identifer of the file.
     *
     * @return execution status
     */
    int get_file_length(uint32_t type, uint32_t identifier, uint32_t & result);

    /**
     * @brief Deletes a file
     *
     * @param type: The type of file.
     * @param identifier: The identifer of the file.
     *
     * @return execution status
     */
    int delete_file(uint32_t type, uint32_t identifier);

    /**
     * @brief Reads a subsection of the file at an offset.
     *
     * @param type: The type of file.
     * @param identifier: The identifer of the file.
     * @param offset: The offset to read from.
     * @param length: Number of bytes to read.
     * @param result: The resuling data is copied into this parameter.
     *
     * @return execution status
     */
    int get_file_data(uint32_t type, uint32_t identifier, uint32_t offset, uint32_t length, Bytes & result);

    int format_filesystem(uint32_t key);
    int set_file(uint32_t type, uint32_t identifier, const Bytes & data);
    int get_file(uint32_t type, uint32_t identifier, Bytes * result);

    
private:
    std::unique_ptr<XEPPrivate> d_ptr;
};

} // namespace Novelda

#endif // XEP_INTERFACE_H
