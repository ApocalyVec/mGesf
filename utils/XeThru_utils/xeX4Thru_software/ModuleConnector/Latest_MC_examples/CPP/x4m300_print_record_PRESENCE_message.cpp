/* 

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

Target module: X4M300

Introduction: This is an example of how to print out application messages from X4M300 module.

prerequisite:

1. This example should be placed in ModuleConnector/examples.
2. Your computer should have a building toolchain. MinGW-w64 in MYSYS2 is suggested for Windows https://www.msys2.org/.

Command to run: 

1. Compile the source code by using make command under source code folder in terminal.
2. Run like "./x4m300_print_record_PRESENCE_message.exe com3" or  "./x4m300_print_record_PRESENCE_message.exe 192.168.1.100 2000" 

*/

#include "ModuleConnector.hpp"
#include "DataRecorder.hpp"
#include "functional"
#include "X4M300.hpp"
#include "signal.h"
#include "xtid.h"
#include <iostream>
#if defined(_WIN32) || defined(__MINGW32__)
#include <winsock2.h>
#endif

using namespace XeThru;

void usage()
{
    std::cout << "Example: print_x4m300_messages.exe com1 or print_x4m300_messages.exe 192.168.1.1 2000" << std::endl;
}

int handle_error(std::string message)
{
    std::cerr << "ERROR: " << message << std::endl;
    return 1;
}

int disp_module_info(ModuleConnector &mc)
{
    X4M300 &app = mc.get_x4m300();
    std::cout << "*****************XeThru Module Information*****************" << std::endl;
    unsigned int pong = 0;
    int status = app.ping(&pong);
    if (status != 0)
    {
        std::cout << "Something went wrong - error code: " << status << std::endl;
        return status;
    }
    else
    {
        std::cout << "Received pong= " << std::hex << std::uppercase << pong << "  Connection build!" << std::endl;
    }
    std::string info = "";
    status = app.get_system_info(XTID_SSIC_FIRMWAREID, &info);
    std::cout << "FirmWareID = " << info << std::endl;
    status = app.get_system_info(XTID_SSIC_VERSION, &info);
    std::cout << "Version = " << info << std::endl;
    status = app.get_system_info(XTID_SSIC_BUILD, &info);
    std::cout << "Build = " << info << std::endl;
    status = app.get_system_info(XTID_SSIC_VERSIONLIST, &info);
    std::cout << "VersionList = " << info << std::endl;
    status = app.get_system_info(XTID_SSIC_ORDERCODE, &info);
    std::cout << "OrderCode = " << info << std::endl;
    status = app.get_system_info(XTID_SSIC_ITEMNUMBER, &info);
    std::cout << "ItemNumber = " << info << std::endl;
    status = app.get_system_info(XTID_SSIC_SERIALNUMBER, &info);
    std::cout << "SerialNumber = " << info << std::endl;
    std::cout << "*****************END!*****************" << std::endl;
    return 0;
}

volatile sig_atomic_t stop_recording;
void handle_sigint(int num)
{
    stop_recording = 1;
    std::cout << "\n Stop recording!\n"
              << std::endl;
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

const uint8_t enalbe_record = 1;

// sensor settings
const float detection_zone_start = 0.4;
const float detection_zone_end = 5;
const uint32_t sensitivity = 5;
const uint32_t noisemap_control = 6;

int print_x4m300_messages(ModuleConnector &mc)
{
    XeThru::PresenceSingleData presence_signal;
    char presence_sensor_state_text[3][15] = {"NO_PRESENCE", "PRESENCE", "INITIALIZING"};
    disp_module_info(mc);
    if (enalbe_record == 1)
    {
        DataRecorder &recorder = mc.get_data_recorder();
        const DataTypes data_types = BasebandApDataType | PresenceSingleDataType;
        const std::string output_directory = ".";
        if (recorder.start_recording(data_types, output_directory) != 0)
        {
            std::cout << "Failed to start recording" << std::endl;
            return 1;
        }
        // Get interface for physical device and enable data output

        {
            DataRecorder::FileAvailableCallback callback = std::bind(&on_file_available,
                                                                     std::placeholders::_1,
                                                                     std::placeholders::_2);
            recorder.subscribe_to_file_available(BasebandApDataType | PresenceSingleDataType, callback);
        }

        {
            DataRecorder::MetaFileAvailableCallback callback = std::bind(&on_meta_file_available,
                                                                         std::placeholders::_1,
                                                                         std::placeholders::_2);
            recorder.subscribe_to_meta_file_available(callback);
        }
    }

    X4M300 &x4m300 = mc.get_x4m300();

    // ignore status. This might fail and that is ok.
    std::cout << "Stop the module" << std::endl;
    x4m300.set_sensor_mode(XTID_SM_STOP, 0);

    std::cout << "Load presence profile" << std::endl;
    if (x4m300.load_profile(XTS_ID_APP_PRESENCE_2))
    {
        return handle_error("load_profile failed");
    }

    x4m300.set_detection_zone(detection_zone_start, detection_zone_end);
    x4m300.set_sensitivity(sensitivity);
    x4m300.set_noisemap_control(noisemap_control);
    // x4m300.set_output_control(XTS_ID_BASEBAND_IQ, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m300.set_output_control(XTS_ID_BASEBAND_AMPLITUDE_PHASE, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m300.set_output_control(XTS_ID_PULSEDOPPLER_FLOAT, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m300.set_output_control(XTS_ID_PULSEDOPPLER_BYTE, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m300.set_output_control(XTS_ID_NOISEMAP_FLOAT, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m300.set_output_control(XTS_ID_NOISEMAP_BYTE, XTID_OUTPUT_CONTROL_ENABLE);
    x4m300.set_output_control(XTS_ID_PRESENCE_SINGLE, XTID_OUTPUT_CONTROL_ENABLE);
    //x4m300.set_output_control(XTS_ID_PRESENCE_MOVINGLIST, XTID_OUTPUT_CONTROL_ENABLE);

    // start the module and profile
    std::cout << "Set the module in RUN state" << std::endl;
    if (x4m300.set_sensor_mode(XTID_SM_RUN, 0))
    {
        return handle_error("Set sensor mode to running failed");
    }

    std::cout << "Printout presence message:" << std::endl;

    while (!stop_recording)
    {
        while (x4m300.peek_message_presence_single() > 0)
        {
            x4m300.read_message_presence_single(&presence_signal);
            std::cout << "message_presence_single: frame_counter: " << presence_signal.frame_counter << " presence_state: " << presence_sensor_state_text[presence_signal.presence_state] << " distance: " << presence_signal.distance << " direction: " << static_cast<unsigned int>(presence_signal.direction) << " signal_quality: " << presence_signal.signal_quality << std::endl;
        }
    }

    std::cout << "Set the module in STOP state" << std::endl;
    if (x4m300.set_sensor_mode(XTID_SM_STOP, 0))
    {
        return handle_error("set output controll failed");
    }

    std::cout << "Messages output finish!" << std::endl;
    return 0;
}

int main(int argc, char **argv)
{
    const unsigned int log_level = 0;
    if (argc < 2)
    {
        usage();
        return -1;
    }
    else if (argc == 2)
    {
        std::cout << "Building Serial/USB port connection ..." << std::endl;
        const std::string device_name = argv[1];
        stop_recording = 0;
        signal(SIGINT, handle_sigint);
        ModuleConnector mc(device_name, log_level);
        return print_x4m300_messages(mc);
    }
    else if (argc == 3)
    {
        std::cout << "Building TCP/IP connection ..." << std::endl;
        int a, b, c, d;
        if (sscanf(argv[1], "%d.%d.%d.%d", &a, &b, &c, &d) != 4)
        {
            usage();
            return 1;
        };
        short int port = atoi(argv[2]);
        stop_recording = 0;
        signal(SIGINT, handle_sigint);
        ModuleConnector mc(htonl(a << 24 | b << 16 | c << 8 | d), htons(port), log_level);
        return print_x4m300_messages(mc);
    }
}
