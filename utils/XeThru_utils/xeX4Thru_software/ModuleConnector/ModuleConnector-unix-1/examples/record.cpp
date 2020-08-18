#include <ModuleConnector.hpp>
#include <DataRecorder.hpp>
#include <X2M200.hpp>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <functional>

using namespace XeThru;

volatile sig_atomic_t stop_recording;
void handle_sigint(int num)
{
    stop_recording = 1;
}

//! [Callback declare]
static void on_file_available(XeThru::DataType type, const std::string &filename)
{
    std::cout << "recorded file available for data type: "
              << DataRecorder::data_type_to_string(type) << std::endl;
    std::cout << "file: " << filename << std::endl;
}
//! [Callback declare]

//! [Callback meta declare]
static void on_meta_file_available(const std::string &session_id, const std::string &meta_filename)
{
    std::cout << "meta file available for recording with id: " << session_id << std::endl;
    std::cout << "file: " << meta_filename << std::endl;
}
//! [Callback meta declare]

int record(const std::string &device_name)
{
//! [Typical usage]
    using namespace XeThru;

    ModuleConnector mc(device_name, 3);
    DataRecorder &recorder = mc.get_data_recorder();

    const DataTypes data_types = BasebandApDataType | SleepDataType;
    const std::string output_directory = ".";

    if (recorder.start_recording(data_types, output_directory) != 0) {
        std::cout << "Failed to start recording" << std::endl;
        return 1;
    }

    // Get interface for physical device and enable data output
//! [Typical usage]

    {
//! [Callback subscribe]
    DataRecorder::FileAvailableCallback callback = std::bind(&on_file_available,
                                                             std::placeholders::_1,
                                                             std::placeholders::_2);
    recorder.subscribe_to_file_available(BasebandApDataType | SleepDataType, callback);
//! [Callback subscribe]
    }

    {
//! [Callback meta subscribe]
    DataRecorder::MetaFileAvailableCallback callback = std::bind(&on_meta_file_available,
                                                                 std::placeholders::_1,
                                                                 std::placeholders::_2);
    recorder.subscribe_to_meta_file_available(callback);
//! [Callback meta subscribe]
    }

    X2M200 &x2m200 = mc.get_x2m200();
    x2m200.load_sleep_profile();
    x2m200.enable_baseband_ap();
    x2m200.set_sensor_mode_run();

    while (!stop_recording) {
        usleep(1000);
    }

    return 0;
}


int main(int argc, char ** argv)
{
    if (argc < 2) {
        std::cout << "record <com port or device file>" << std::endl;
        return 1;
    }

    stop_recording = 0;
    signal(SIGINT, handle_sigint);

    return record(argv[1]);
}
