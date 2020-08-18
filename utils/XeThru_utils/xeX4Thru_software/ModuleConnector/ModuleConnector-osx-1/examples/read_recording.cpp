#include <DataReader.hpp>
#include <iostream>
#include <memory>
#include <cassert>

using namespace XeThru;

static void process_baseband_ap(const DataRecord &record)
{
    assert(record.data_type == BasebandApDataType);
    std::cout << "processing baseband ap data: " << record.data.size() << std::endl;
    if (record.is_user_header)
        std::cout << "  |-- custom user header" << std::endl;
}

static void process_sleep(const DataRecord &record)
{
    assert(record.data_type == SleepDataType);
    std::cout << "processing sleep data: " << record.data.size() << std::endl;
    if (record.is_user_header)
        std::cout << "  |-- custom user header" << std::endl;
}

int read_recording(const std::string &meta_filename)
{
//! [Typical usage]
    using namespace XeThru;

    DataReader reader;

    if (reader.open(meta_filename) != 0) {
        std::cout << "ERROR: failed to open recording" << std::endl;
        return 1;
    }

    // Only interested in 'BasebandApDataType' and 'SleepDataType'
    reader.set_filter(BasebandApDataType | SleepDataType);

    // Read all records
    while (!reader.at_end()) {
        const DataRecord record = reader.read_record();
        if (!record.is_valid) {
            std::cout << "ERROR: failed to read record" << std::endl;
            return 1;
        }

        // record.data contains data fields (bytes) as specified by the
        // XeThru File Formats document unless record.is_user_header is true.
        std::cout << "read record of data type: " << record.data_type
                  << ", size: " << record.data.size() << std::endl;
        if (record.is_user_header)
            std::cout << "(custom user header)" << std::endl;

        switch (record.data_type) {
        case BasebandApDataType:
            process_baseband_ap(record);
            break;
        case SleepDataType:
            process_sleep(record);
            break;
        default:
            assert(false);
            break;
        }
    }
//! [Typical usage]

    std::cout << "--- Seek into the middle of recording ---" << std::endl;
    if (reader.seek_byte(reader.get_size() / 2) != 0) {
        std::cout << "ERROR: failed to seek" << std::endl;
        return 1;
    }

//! [Typical usage with raw buffer]
    const uint32_t max_record_size = reader.get_max_record_size();
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[max_record_size]);

    while (!reader.at_end()) {
        uint32_t record_size;
        uint32_t data_type;
        int64_t epoch;
        uint8_t is_user_header;

        if (reader.read_record(buffer.get(), max_record_size, &record_size, &data_type, &epoch, &is_user_header) != 0) {
            std::cout << "ERROR: failed to read record" << std::endl;
            return 1;
        }

        // ...
    }
//! [Typical usage with raw buffer]

    return 0;
}


int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cout << "read_recording <xethru recording meta file>" << std::endl;
        return 1;
    }

    return read_recording(argv[1]);
}
