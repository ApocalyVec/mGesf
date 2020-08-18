#include "ModuleConnector.hpp"
#include "XEP.hpp"
#include "DataRecorder.hpp"
#include "X2M200.hpp"
#include "X4M300.hpp"
#include "iostream"
#include "xtid.h"
#include "unistd.h"
#include "stdio.h"
#include "signal.h"
#include "functional"

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
    ModuleConnector mc(device_name, 3);
    DataRecorder &recorder = mc.get_data_recorder();

    const DataTypes data_types = AllDataTypes;
    const std::string output_directory = ".";


    if (recorder.start_recording(data_types, output_directory) != 0) {
        std::cout << "Failed to start recording" << std::endl;
        return 1;
    }

    {
        DataRecorder::FileAvailableCallback callback = std::bind(&on_file_available,
                                                                 std::placeholders::_1,
                                                                 std::placeholders::_2);
        recorder.subscribe_to_file_available(AllDataTypes, callback);
    }

    {
        DataRecorder::MetaFileAvailableCallback callback = std::bind(&on_meta_file_available,
                                                                     std::placeholders::_1,
                                                                     std::placeholders::_2);
        recorder.subscribe_to_meta_file_available(callback);
    }

    std::string FWID;
    XEP & xep = mc.get_xep();
    xep.get_system_info(0x02, &FWID);

    if (FWID == "XEP") {
        //Do nothing
    } else if (FWID == "xtapplication_m4") {
        // If Module X2M200, record baseband
        std::cout << "Program not supported for X2M200." << '\n';
    } else {
        std::string Module;
        X4M300 & x4m300=mc.get_x4m300();
        x4m300.set_sensor_mode(XTID_SM_STOP,0);
        x4m300.set_sensor_mode(XTID_SM_MANUAL,0);
        xep.get_system_info(0x01, &Module);
        std::cout << "Start XEP recording for " << Module << '\n';
    }

    // start streaming data
    xep.x4driver_set_fps(5);
    while (!stop_recording) {
        usleep(1000);
    }

    return 0;
}

int main(int argc, char ** argv){
    if (argc < 2) {
        std::cout << "record <com port or device file>" << std::endl;
        return 1;
    }
    stop_recording = 0;
    signal(SIGINT, handle_sigint);
    return record(argv[1]);
}
