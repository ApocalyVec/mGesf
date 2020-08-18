#ifndef DATARECORDER_H
#define DATARECORDER_H


#include "NotSupportedData.hpp"
#include "RecordingOptions.hpp"
#include "LockedRadarForward.hpp"

struct Logger;

namespace XeThru {

class ModuleConnectorImpl;
/**
 * @class DataRecorder
 *
 * The DataRecorder class allows recording of xethru data types to disk.
 *
 * The DataRecorder class is a high level recorder class. It can be used to record all data types
 * sent by a xethru device over serial port or similar. All low-level I/O is handled by the recorder
 * itself with no setup required. Data is stored on disk as specified by the XeThru File Formats document
 * and can be easily read back using the \ref DataReader class (read disk records) or
 * \ref DataPlayer class (playback disk records as binary packets).
 *
 * @snippet record.cpp Typical usage
 *
 * @note The DataRecorder has no means to enable or disable data output on a physical xethru device.
 * It simply records the specified data type(s) if sent by the module. Therefore, it is the user's
 * responsibility to enable or disable data ouput on the module itself. If no data is sent, no data
 * will be recorded.
 *
 * DataRecorder also supports more advance \a RecordingOptions such as splitting of files and directories.
 *
 * @see start_recording, RecordingOptions, DataReader, DataPlayer
 */
class DataRecorderPrivate;
class DataRecorder
{
public:
    /**
     * Typedef for std::function<void(Xethru::DataType, const std::string&)>.
     *
     * This type is used as callback parameter to subscribe_to_file_available.
     *
     * Declare a standard function:
     * @snippet record.cpp Callback declare
     *
     * Then construct the callback and pass as parameter:
     * @snippet record.cpp Callback subscribe
     *
     * @note For C++ member functions use std::bind(&Class::function, this, ...)
     *
     * @see subscribe_to_file_available
     */
    typedef std::function<void(XeThru::DataType, const std::string&)> FileAvailableCallback;

    /**
     * Typedef for std::function<void(const std::string&, const std::string&)>.
     *
     * This type is used as callback parameter to subscribe_to_meta_file_available.
     *
     * Declare a standard function:
     * @snippet record.cpp Callback meta declare
     *
     * Then construct the callback and pass as parameter:
     * @snippet record.cpp Callback meta subscribe
     *
     * @note For C++ member functions use std::bind(&Class::function, this, ...)
     *
     * @see subscribe_to_meta_file_available
     */
    typedef std::function<void(const std::string &, const std::string&)> MetaFileAvailableCallback;

    /**
     * Typedef for std::function<void(const std::string &)>.
     *
     * This type is used as a callback parameter to set_error_callback().
     * @see set_error_callback
     */
    typedef std::function<void(const std::string &)> RecordingErrorCallback;

    /**
     * Constructs recorder.
     */
    DataRecorder();

    /**
     * Constructs recorder. Only useful for internal use.
     * @internal
     */
    DataRecorder(LockedRadarInterfacePtr &radar_interface);

    /**
     * Constructs recorder initialized with a logger.
     * @param logger Specifies a logger to use when printing messages
     */
    DataRecorder(Logger &logger);

    /**
     * Destroys the recorder
     */
    ~DataRecorder();

    /**
     * Starts recording the specified data type(s) and output recorded files to the directory specified.
     * This command does not enable data output on the module.
     *
     * @param data_types DataTypes is a bitmask that consists of a combination of DataType flags.
     * These flags can be combined with the bitwise OR operator (|). For example: BasebandIqDataType | SleepDataType.
     * A convenience value AllDataTypes can also be specified.
     *
     * @param directory Output folder where recorded files will be stored.
     *
     * @param options Specifies advanced options such as splitting of files and directories.
     * By default, this parameter is RecordingOptions() (no advanced options).
     *
     * @return 0 on success, otherwise returns 1
     *
     * @see RecordingOptions, stop_recording
     */
    int start_recording(XeThru::DataTypes data_types, const std::string &directory,
                        const XeThru::RecordingOptions &options = XeThru::RecordingOptions());

    /**
     * Stops recording the specified data type(s).
     * This command does not disable data output on the module.
     *
     * @param data_types DataTypes is a bitmask that consists of a combination of DataType flags.
     * These flags can be combined with the bitwise OR operator (|). For example: BasebandIqDataType | SleepDataType.
     * A convenience value AllDataTypes can also be specified.
     *
     * @see start_recording
     */
    void stop_recording(XeThru::DataTypes data_types);

    /**
     * @return true if recording is started for the specified data type, otherwise returns false
     * @see start_recording
     */
    bool is_recording(XeThru::DataType data_type) const;

    /**
     * This function should not be used when recording data from a physical xethru device. It is only relevant
     * for recording data generated elsewhere. Most users will not need this functionality.
     *
     * Process the specified data for a given data type.
     *
     * @param data_type Specifies the data type to process
     * @param data Specifies the bytes to process
     *
     * @return true on success, otherwise returns false
     */
    bool process(XeThru::DataType data_type, const Bytes &data);

    /**
     * Subscribes to notifications when a recorded file for a data type is available. The callback is
     * triggered when a new file is stored on disk, i.e. when module connector is done with the file
     * and no longer will perform I/O operations on it. This usually triggers when recording is stopped
     * or as a result of file splitting. In either case module connector will no longer hold any reference
     * to the file.
     *
     * @param data_types DataTypes is a bitmask that consists of a combination of DataType flags.
     * These flags can be combined with the bitwise OR operator (|). For example: BasebandIqDataType | SleepDataType.
     * A convenience value AllDataTypes can also be specified.
     *
     * @param callback Specifies the function to be called when a recorded file is available.
     * For example: void on_file_available(DataType type, const std::string &filename)
     *
     * @return 0 on success, otherwise returns 1
     *
     * @see start_recording, unsubscribe_to_file_available, FileAvailableCallback
     */
    int subscribe_to_file_available(DataTypes data_types, const FileAvailableCallback &callback);

    /**
     * Unsubscribes to notifications when a recorded file for a data type is available.
     *
     * @param data_types DataTypes is a bitmask that consists of a combination of DataType flags.
     * These flags can be combined with the bitwise OR operator (|). For example: BasebandIqDataType | SleepDataType.
     * A convenience value AllDataTypes can also be specified.
     *
     * @see subscribe_to_file_available
     */
    void unsubscribe_to_file_available(XeThru::DataTypes data_types);

    /**
     * Subscribes to notifications when a meta file is available. The callback is triggered when a new
     * meta file is stored on disk, i.e. when module connector is done with the file and no longer perform
     * I/O operations on it. This usually triggers when recording is stopped or as a result of
     * file/directory splitting. In either case module connector will no longer hold any reference
     * to the file.
     *
     * The meta file contains detailed information about a particular recording session. For example
     * which data types and files were written during the session. This information is possible to
     * extract via \a DataReader and allows for convenient readback of data from disk. The meta file
     * is only intended for use with \a DataReader.
     *
     * @param callback Specifies the function to be called when a meta file is available.
     * For example: void on_meta_file_available(const std::string &session_id, const std::string &filename)
     *
     * @return 0 on success, otherwise returns 1
     *
     * @see DataReader, start_recording, unsubscribe_to_meta_file_available,
     * MetaFileAvailableCallback, subscribe_to_file_available
     */
    int subscribe_to_meta_file_available(const MetaFileAvailableCallback &callback);

    /**
     * Unsubscribes to notifications when a meta file is available.
     *
     * @see subscribe_to_meta_file_available
     */
    int unsubscribe_to_meta_file_available();

    /**
     * Sets the basename for the specified data type. Most users will not need this functionality.
     *
     * @param data_type Specifies the data type
     * @param name Specifies the name
     * @see get_basename_for_data_type, clear_basename_for_data_types
     */
    void set_basename_for_data_type(XeThru::DataType data_type, const std::string &name);

    /**
     * Gets the basename for the specified data type. Most users will not need this functionality.
     *
     * @param data_type Specifies the data type.
     * @return the basename for the specified data type
     * @see set_basename_for_data_type
     */
    std::string get_basename_for_data_type(XeThru::DataType data_type) const;

    /**
     * Resets the basename(s) to default value(s) for the specified data type(s).
     * Most users will not need this functionality.
     *
     * @param data_types DataTypes is a bitmask that consists of a combination of DataType flags.
     * These flags can be combined with the bitwise OR operator (|). For example: BasebandIqDataType | SleepDataType.
     * A convenience value AllDataTypes can also be specified.
     *
     * @see set_basename_for_data_type
     */
    void clear_basename_for_data_types(XeThru::DataTypes data_types);

    /**
     * Gets the output folder for the specified session id.
     * The folder is relative to the directory specified in start_recording.
     *
     * @param session_id Specifies the session id.
     * @return the output folder if a recording for the given session id is active,
     * otherwise returns an empty string.
     */
    std::string get_recording_directory(const std::string &session_id) const;

    /**
     * Gets the output folder for the specified data type.
     * The folder is relative to the directory specified in start_recording.
     *
     * @param session_id Specifies the data type.
     * @return the output folder if a recording for the given data type is active,
     * otherwise returns an empty string.
     * @see is_recording
     */
    std::string get_recording_directory(XeThru::DataType data_type) const;

    /**
     * Convenience method for converting data type to human readable string representation.
     * @param data_type
     * @return the string representation of the specified data type
     */
    static std::string data_type_to_string(XeThru::DataType data_type);

    /**
     * Supply a function that will be called if an error occurs during recording.
     * Call this method with nullptr as argument to clear the callback.
     */
    void set_error_callback(RecordingErrorCallback callback);

private:
    DataRecorder(const DataRecorder &other) = delete;
    DataRecorder(DataRecorder &&other) = delete;
    DataRecorder& operator= (const DataRecorder &other) = delete;
    DataRecorder& operator= (DataRecorder &&other) = delete;

    friend RadarInterface;
    DataRecorderPrivate *d_ptr;
};

} // namespace XeThru

#endif // DATARECORDER_H
