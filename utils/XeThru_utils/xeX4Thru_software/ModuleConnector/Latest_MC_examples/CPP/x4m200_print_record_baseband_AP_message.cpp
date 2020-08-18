/* 

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

Target module: x4m200

Introduction: This is an example of how to print out application messages from x4m200 module.

prerequisite:

1. This example should be placed in ModuleConnector/examples.
2. Your computer should have a building toolchain. MinGW-w64 in MYSYS2 is suggested for Windows https://www.msys2.org/.

Command to run: 

1. Compile the source code by using make command under source code folder in terminal.
2. Run like "./x4m200_print_record_RESP_SLEEP_message.exe com3" or  "./x4m200_print_record_RESP_SLEEP_message.exe 192.168.1.100 2000" 

*/

#include "ModuleConnector.hpp"
#include "DataRecorder.hpp"
#include "functional"
#include "x4m200.hpp"
#include "signal.h"
#include "xtid.h"
#include <iostream>
#if defined(_WIN32) || defined(__MINGW32__)
#include <winsock2.h>
#endif

using namespace XeThru;

void usage()
{
    std::cout << "Example: print_x4m200_messages.exe com1 or print_x4m200_messages.exe 192.168.1.1 2000" << std::endl;
}

int handle_error(std::string message)
{
    std::cerr << "ERROR: " << message << std::endl;
    return 1;
}

int disp_module_info(ModuleConnector &mc)
{
    X4M200 &app = mc.get_x4m200();
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

int print_x4m200_messages(ModuleConnector &mc)
{
    //XeThru::SleepData respiration_sleep_message;
    //XeThru::RespirationMovingListData respiration_movinglist_message;
    XeThru::BasebandApData respiration_baseband_AP_message;
    //char respiration_sensor_state_text[5][20] = {"BREATHING", "MOVEMENT", "MOVEMENT TRACKING", "NO MOVEMENT", "INITIALIZING"};
    disp_module_info(mc);
    if (enalbe_record == 1)
    {
        DataRecorder &recorder = mc.get_data_recorder();
        const DataTypes data_types = BasebandApDataType | SleepDataType;
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
            recorder.subscribe_to_file_available(BasebandApDataType | SleepDataType, callback);
        }

        {
            DataRecorder::MetaFileAvailableCallback callback = std::bind(&on_meta_file_available,
                                                                         std::placeholders::_1,
                                                                         std::placeholders::_2);
            recorder.subscribe_to_meta_file_available(callback);
        }
    }
    //const unsigned int log_level = 0;
    X4M200 &x4m200 = mc.get_x4m200();

    // ignore status. This might fail and that is ok.
    std::cout << "Stop the module" << std::endl;
    x4m200.set_sensor_mode(XTID_SM_STOP, 0);

    std::cout << "Load presence profile" << std::endl;
    if (x4m200.load_profile(XTS_ID_APP_RESPIRATION_2))
    {
        return handle_error("load_profile failed");
    }

    x4m200.set_detection_zone(detection_zone_start, detection_zone_end);
    x4m200.set_sensitivity(sensitivity);
    x4m200.set_noisemap_control(noisemap_control);
    // x4m200.set_output_control(XTS_ID_BASEBAND_IQ, XTID_OUTPUT_CONTROL_ENABLE);
    x4m200.set_output_control(XTS_ID_BASEBAND_AMPLITUDE_PHASE, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m200.set_output_control(XTS_ID_PULSEDOPPLER_FLOAT, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m200.set_output_control(XTS_ID_PULSEDOPPLER_BYTE, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m200.set_output_control(XTS_ID_NOISEMAP_FLOAT, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m200.set_output_control(XTS_ID_NOISEMAP_BYTE, XTID_OUTPUT_CONTROL_ENABLE);
    //x4m200.set_output_control(XTS_ID_SLEEP_STATUS, XTID_OUTPUT_CONTROL_ENABLE);
    //x4m200.set_output_control(XTS_ID_RESP_STATUS, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m200.set_output_control(XTS_ID_VITAL_SIGNS, XTID_OUTPUT_CONTROL_ENABLE);
    //x4m200.set_output_control(XTS_ID_RESPIRATION_MOVINGLIST, XTID_OUTPUT_CONTROL_ENABLE);
    // x4m200.set_output_control(XTS_ID_RESPIRATION_DETECTIONLIST, XTID_OUTPUT_CONTROL_ENABLE);

    // start the module and profile

    std::cout << "Set the module in RUN state" << std::endl;
    if (x4m200.set_sensor_mode(XTID_SM_RUN, 0))
    {
        return handle_error("Set sensor mode to running failed");
    }

    std::cout << "Printout respiration sleep message:" << std::endl;

    while (!stop_recording)
    {
        // while (x4m200.peek_message_respiration_sleep() > 0)
        // {
        //     x4m200.read_message_respiration_sleep(&respiration_sleep_message);
        //     std::cout << "message_respiration_sleep: frame_counter: " << respiration_sleep_message.frame_counter << " sensor_state: " << respiration_sensor_state_text[respiration_sleep_message.sensor_state] << " respiration_rate : " << respiration_sleep_message.respiration_rate << " distance : " << respiration_sleep_message.distance << " movement_slow : " << respiration_sleep_message.movement_slow << " movement_fast : " << respiration_sleep_message.movement_fast << std::endl;
        // }
        // while (x4m200.peek_message_respiration_movinglist() > 0)
        // {
        //     x4m200.read_message_respiration_movinglist(&respiration_movinglist_message);
        //     std::cout << "message_respiration_movinglist: counter: " << respiration_movinglist_message.counter << " movement_slow_items: " << respiration_movinglist_message.movement_slow_items[1] << " movement_fast_items: " << respiration_movinglist_message.movement_fast_items[1] << std::endl;
        // }
        while (x4m200.peek_message_baseband_ap() > 0)
        {
            x4m200.read_message_baseband_ap(&respiration_baseband_AP_message);
            std::cout << "message_baseband_ap: counter: " << respiration_baseband_AP_message.frame_counter << " num_bins: " << respiration_baseband_AP_message.num_bins << " amplitude: " << respiration_baseband_AP_message.amplitude[8] << std::endl;
        }
    }

    std::cout << "Set the module in STOP state" << std::endl;
    if (x4m200.set_sensor_mode(XTID_SM_STOP, 0))
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
        return print_x4m200_messages(mc);
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
        return print_x4m200_messages(mc);
    }
}
