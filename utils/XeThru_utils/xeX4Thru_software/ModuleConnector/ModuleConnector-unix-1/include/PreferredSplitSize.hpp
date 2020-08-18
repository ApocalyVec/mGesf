#ifndef PREFERREDSPLITSIZE_H
#define PREFERREDSPLITSIZE_H

#include <cinttypes>

namespace XeThru {

/**
 * @class PreferredSplitSize
 *
 * The PreferredSplitSize class allows specifying a split size.
 *
 * The PreferredSplitSize class contains functions to define a preferred split size for files
 * and directories specified in either duration (seconds) or as number of bytes. Specifying
 * both is not supported; the last specified type will rule.
 *
 * @note As the class name indicates, the size is only a *preferred* size. There is
 * no guarantee that the actual size will match 100%.
 *
 * @see DataRecorder, RecordingOptions
 */
class PreferredSplitSizePrivate;
class PreferredSplitSize
{
public:
    /**
     * This enum is used to specify the type of the preferred split size.
     */
    enum SizeType {
        DurationType,
        ByteCountType,
        FixedDailyHour,
        InvalidType,
    };

    /**
     * Constructs split size.
     */
    PreferredSplitSize();

    /**
     * Copy constructor.
     */
    PreferredSplitSize(const PreferredSplitSize &other);

    /**
     * Move constructor.
     */
    PreferredSplitSize(PreferredSplitSize &&other);

    /**
     * Destroys the split size.
     */
    ~PreferredSplitSize();

    /**
     * @return the preferred split size in seconds if set, otherwise -1.
     */
    int get_duration() const;

    /**
     * Sets the preferred split size specified in seconds.
     * @param seconds
     */
    void set_duration(int seconds);

    /**
     * @return the preferred file size as number of bytes if set, otherwise -1.
     */
    int64_t get_byte_count() const;

    /**
     * Sets the preferred split size specified as number of bytes.
     * @param count
     */
    void set_byte_count(int64_t count);

    /**
     * @return the fixed daily hour if set (0-23), otherwise returns -1.
     */
    int get_fixed_daily_hour() const;

    /**
     * Sets the preferred split size to a fixed daily hour. Range is 24 hour clock (0-23).
     * For example, if splitting every day at 2pm is desired, the parameter to this function
     * would be 14. By default, no fixed daily hour is set.
     * @param hour
     */
    void set_fixed_daily_hour(int hour);

    /**
     * The value reflects the last specified type, e.g. if calling set_duration(15 * 60) this
     * function returns DurationType.
     * @return the type of the preferred size.
     * @see SizeType
     */
    SizeType get_type() const;

    /**
     * Copy assignment operator.
     */
    PreferredSplitSize& operator= (const PreferredSplitSize &other);

    /**
     * Move assignment operator.
     */
    PreferredSplitSize& operator= (PreferredSplitSize &&other);

private:
    PreferredSplitSizePrivate *d_ptr;
};

} // namespace Novelda

#endif // PREFERREDSPLITSIZE_H
