#ifndef RECORDINGOPTIONS_H
#define RECORDINGOPTIONS_H

#include "PreferredSplitSize.hpp"
#include "Bytes.hpp"
#include <string>

namespace XeThru {

/**
 * @class RecordingOptions
 *
 * The RecordingOptions class allows specifying options for recording.
 *
 * The RecordingOptions class contains more advance options for recording. It can be used to specify things
 * like splitting of files and directories.
 *
 * @see DataRecorder
 */
class RecordingOptionsPrivate;
class RecordingOptions
{
public:
    /**
     * Constructs options.
     */
    RecordingOptions();

    /**
     * Copy constructor.
     */
    RecordingOptions(const RecordingOptions &other);

    /**
     * Move constructor.
     */
    RecordingOptions(RecordingOptions &&other);

    /**
     * Destroys options.
     */
    ~RecordingOptions();

    /**
     * Sets the session id as specified, overriding the default constructed value which
     * is an universally unique identifier (UUID).
     *
     * @param id Specifies the new id
     * @see get_session_id
     */
    void set_session_id(const std::string &id);

    /**
     * By default, this function returns an universally unique identifier (UUID) if no custom id is set.
     * @return the session id
     * @see set_session_id
     */
    std::string get_session_id() const;

    /**
     * Sets the preferred file split size as specified.
     * @param size Specifies the preferred split size
     */
    void set_file_split_size(const PreferredSplitSize &size);

    /**
     * By default, this function returns a default constructed value (no file split size) if no custom size is set.
     * @return the preferred file split size
     * @see PreferredSplitSize
     */
    PreferredSplitSize get_file_split_size() const;

    /**
     * Sets the preferred directory split size as specified.
     * @param size Specifies the preferred split size
     */
    void set_directory_split_size(const PreferredSplitSize &size);

    /**
     * By default, this function returns a default constructed value (no directory split size) if no custom size is set.
     * @return the preferred directory split size
     * @see PreferredSplitSize
     */
    PreferredSplitSize get_directory_split_size() const;

    /**
     * Sets the data rate (ms) the recorder will read data from the module.
     * A value of 1000 ms means data is read every second. Data in between is discarded.
     * By default, this parameter is -1 (no data rate limit).
     * @param limit Specifies the data rate limit
     */
    void set_data_rate_limit(int limit);

    /**
     * @return the data rate (ms) the recorder will read data from the module if set,
     * otherwise returns -1 (no data rate limit). A value of 1000 ms means data is read every second.
     * Data in between is discarded.
     */
    int get_data_rate_limit() const;

    /**
     * Sets a custom header applied to the beginning of the recorded file. By default, this parameter
     * is Bytes() (no custom header).
     * @param header Specifies the header
     */
    void set_user_header(const Bytes &header);

    /**
     * @return the custom user header. By default, this parameter is Bytes() (no custom header).
     */
    Bytes get_user_header() const;

    /**
     * Specify whether to flush on every write. Note that this will impact the performance and should
     * only be used for debugging purposes. The default is false.
     * @param do_flush If true, recording files are flushed after every write.
     */
    void set_flush_on_write(bool do_flush);

    /**
     * @return Whether flushing for every write to file is enabled.
     */
     bool get_flush_on_write() const;

    /**
     * Copy assignment operator.
     */
    RecordingOptions& operator= (const RecordingOptions &other);

    /**
     * Move assignment operator.
     */
    RecordingOptions& operator= (RecordingOptions &&other);

private:
    RecordingOptionsPrivate *d_ptr;
};

} // namespace Novelda

#endif // RECORDINGOPTIONS_H
