/*
X4M200-recording example code
*/
#include "ModuleConnector.hpp"
#include "DataRecorder.hpp"
#include "functional"
#include "X4M200.hpp"
#include "iostream"
#include "unistd.h"
#include "stdio.h"
#include "signal.h"
#include "xtid.h"

using namespace XeThru;

volatile sig_atomic_t stop_recording;
void handle_sigint(int num)
{
    stop_recording = 1;
}

static void on_file_available(XeThru::DataType type, const std::string &filename)
{
    std::cout << "recorded file available for data type: "
              << DataRecorder::data_type_to_string(type) << std::endl;
    std::cout << "file: " << filename << std::endl;
}

static void on_meta_file_available(const std::string &session_id, const std::string &meta_filename)
{
    std::cout << "meta file available for recording with id: " << session_id << std::endl;
    std::cout << "file: " << meta_filename << std::endl;
}

int record(const std::string &device_name)
{
    using namespace XeThru;
    //Acquiring DataRecorder and ModuleConnector
    ModuleConnector mc(device_name, 0);
    DataRecorder &recorder = mc.get_data_recorder();

    const DataTypes data_types = BasebandApDataType | SleepDataType; //specifying data_types
    const std::string output_directory = "."; //setting directory to current

    //start recorder
    if (recorder.start_recording(data_types, output_directory) != 0) {
        //Start recording failed
        std::cout << "Failed to start recording" << std::endl;
        return 1;
    }

    // Get interface for physical device and enable data output

    {
        DataRecorder::FileAvailableCallback callback = std::bind(&on_file_available,
                                                                 std::placeholders::_1,
                                                                 std::placeholders::_2);
        recorder.subscribe_to_file_available(BasebandApDataType | SleepDataType, callback);
    }

    {
    DataRecorder::MetaFileAvailableCallback callback = std::bind(&on_meta_file_available,
                                                                 std::placeholders::_1,
                                                                 std::placeholders::_2);
    recorder.subscribe_to_meta_file_available(callback);
    }

    //configure and run X4M200
    X4M200 &x4m200 = mc.get_x4m200();
    x4m200.load_profile(XTS_ID_APP_RESPIRATION_2);
    x4m200.set_output_control(XTS_ID_RESPIRATION_DETECTIONLIST, XTID_OUTPUT_CONTROL_ENABLE);
    x4m200.set_sensor_mode(XTID_SM_RUN, 0);
    //Sleepdata variable
    XeThru::SleepData data;

    while (!stop_recording) {
        if (x4m200.read_message_respiration_sleep(&data)) {
            //X4M200 read failed
            std::cout << "Read message failed" << '\n';
            return 1;
        }
        //print output
        std::cout << "Counter " << data.frame_counter<< "  ";
        std::cout << "RPM " << data.respiration_rate << "  ";
        std::cout << "Movement Slow " << data.movement_slow << "  ";
        std::cout << "Movement Fast " << data.movement_fast << '\n';

        usleep(100);
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
