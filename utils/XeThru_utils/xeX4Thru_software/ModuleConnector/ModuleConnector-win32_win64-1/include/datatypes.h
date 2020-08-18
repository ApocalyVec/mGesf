#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdint.h>

#ifndef SWIG
/**
 * @internal
 */
#define ENUM_BEGIN(x) enum x
/**
 * @internal
 */
#define ENUM_END
#endif

#ifdef __cplusplus
extern "C" {
namespace XeThru {
#endif

/**
 * This enum type is used to specify which data type to record from the module.
 *
 * @see DataRecorder
 */
ENUM_BEGIN(DataType) {
    BasebandApDataType                   = 1 << 0,
    BasebandIqDataType                   = 1 << 1,
    SleepDataType                        = 1 << 2,
    RespirationDataType                  = 1 << 3,
    PerformanceStatusType                = 1 << 4,
    StringDataType                       = 1 << 5,
    PulseDopplerFloatDataType            = 1 << 6,
    PulseDopplerByteDataType             = 1 << 7,
    NoiseMapFloatDataType                = 1 << 8,
    NoiseMapByteDataType                 = 1 << 9,
    FloatDataType                        = 1 << 10,
    ByteDataType                         = 1 << 11,
    PresenceSingleDataType               = 1 << 12,
    PresenceMovingListDataType           = 1 << 13,
    RespirationDetectionListDataType     = 1 << 14,
    RespirationMovingListDataType        = 1 << 15,
    VitalSignsDataType                   = 1 << 16,
    SleepStageDataType                   = 1 << 17,
    RespirationNormalizedMovementListDataType = 1 << 18,
    RadarRfDataType                     = 1 << 19,
    RadarRfNormalizedDataType           = 1 << 20,
    RadarBasebandFloatDataType          = 1 << 21,
    RadarBasebandQ15DataType            = 1 << 22,
}; ENUM_END

/**
 * @typedef DataTypes
 * DataTypes is a bitmask that consists of a combination of \ref DataType flags.
 * These flags can be combined with the bitwise OR operator (|).
 * For example: BasebandApDataType | SleepDataType.
 * A convenience value \ref AllDataTypes can also be used.
 */
typedef uint32_t DataTypes;

/**
 * Invalid data type, do not use.
 */
static const uint32_t InvalidDataType = 0;

/**
 * A convenience value used to specify all data types.
 */
static const uint32_t AllDataTypes = 0xffffffff;

#ifdef __cplusplus
} // namespace XeThru
} // extern C
#endif

#endif // DATATYPES_H
