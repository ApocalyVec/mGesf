/* 

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

Target module: 
#X4M200
#X4M300
#X4M03(XEP)


Introduction: XeThru modules support both RF and baseband data output. This is an example of radar raw data manipulation. Developer can use Module Connecter API to read, record radar raw data, and also playback recorded data. 

prerequisite:

1. This example should be placed in ModuleConnector/examples.
2. Your computer should have a building toolchain. MinGW-w64 in MYSYS2 is suggested for Windows https://www.msys2.org/.

Command to run: 

1. Compile the source code by using make command under source code folder in terminal.
2. Run like xt_modules_print_record_playback_radar_raw_data_message.exe\n com1\n or 192.168.1.1 2000\n or .\xethru_recording_xxx\\xethru_recording_meta.dat for different inputs.

*/

#include "ModuleConnector.hpp"
#include "DataRecorder.hpp"
#include "DataPlayer.hpp"
#include "functional"
#include "X4M200.hpp"
#include "signal.h"
#include "XEP.hpp"
#include "xtid.h"
#include <iostream>
#if defined(_WIN32) || defined(__MINGW32__)
#include <winsock2.h>
#endif

using namespace XeThru;

void usage()
{
    std::cout << "Example: xt_modules_print_record_playback_radar_raw_data_message.exe\n com1\n or 192.168.1.1 2000\n or .\xethru_recording_xxx\\xethru_recording_meta.dat" << std::endl;
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

volatile sig_atomic_t stop_output;
void handle_sigint(int num)
{
    stop_output = 1;
    std::cout << "\nStop recording!"
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

uint8_t playback = 0;
const uint8_t enalbe_record = 1;

const uint8_t downconversion = 1;
const float dac_min = 949;
const float dac_max = 1100;
const uint32_t iterations = 16;
const uint32_t pulses_per_step = 300;
const float frame_area_offset = 0.18;
const float frame_area_start = 0;
const float frame_area_stop = 5;
const float fps = 1;

int print_radar_raw_data_message(ModuleConnector &mc)
{
    XeThru::DataFloat radar_raw_data;

    if (enalbe_record == 1)
    {
        DataRecorder &recorder = mc.get_data_recorder();
        const DataTypes data_types = FloatDataType | PresenceSingleDataType;
        const std::string output_directory = ".";
        disp_module_info(mc);
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
            recorder.subscribe_to_file_available(FloatDataType | PresenceSingleDataType, callback);
        }

        {
            DataRecorder::MetaFileAvailableCallback callback = std::bind(&on_meta_file_available,
                                                                         std::placeholders::_1,
                                                                         std::placeholders::_2);
            recorder.subscribe_to_meta_file_available(callback);
        }
    }

    X4M200 &xethru_module = mc.get_x4m200();

    // ignore status. This might fail and that is ok.
    std::cout << "Set the module to Stop mode" << std::endl;
    xethru_module.set_sensor_mode(XTID_SM_STOP, 0);
    std::cout << "Set the module to Manual mode" << std::endl;
    xethru_module.set_sensor_mode(XTID_SM_MANUAL, 0);

    XEP &xep = mc.get_xep();

    xep.x4driver_set_downconversion(downconversion);
    xep.x4driver_set_tx_center_frequency(3);
    xep.x4driver_set_tx_power(1);
    xep.x4driver_set_iterations(iterations);
    xep.x4driver_set_pulses_per_step(pulses_per_step);
    xep.x4driver_set_dac_min(dac_min);
    xep.x4driver_set_dac_max(dac_max);

    // start the module and profile
    std::cout << "Set the module FPS and start to output radar raw data." << std::endl;
    if (xep.x4driver_set_fps(fps))
    {
        return handle_error("Set FPS failed");
    }

    std::cout << "Printout radar raw message:" << std::endl;

    while (!stop_output)
    {
        while (xep.peek_message_data_float() > 0)
        {
            xep.read_message_data_float(&radar_raw_data);
            std::cout << "\nradar_raw_data_message:"
                      << std::endl;
            for (auto x = std::end(radar_raw_data.data); x != std::begin(radar_raw_data.data);)
            {
                std::cout << *--x << ' ';
            }
        }
    }

    std::cout << "\n Set the module in STOP state! \n"
              << std::endl;
    if (xethru_module.set_sensor_mode(XTID_SM_STOP, 0))
    {
        return handle_error("set output controll failed");
    }

    std::cout << "Messages output finish!\n"
              << std::endl;
    return 0;
}

int playback_radar_raw_data_message(DataPlayer &player)
{
    XeThru::DataFloat radar_raw_data;
    //Filter for Float data
    player.set_filter(FloatDataType);
    player.set_position(0);
    //Set playback rate
    player.set_playback_rate(1);
    //Enable loop
    player.set_loop_mode_enabled(1);
    //Start playback
    player.play();

    ModuleConnector mc(player, 0);
    //Get application interface
    XEP &xep = mc.get_xep();
    //Start playback
    player.play();

    std::cout << "Printout radar raw message:" << std::endl;

    while (!stop_output)
    {
        while (xep.peek_message_data_float() > 0)
        {
            xep.read_message_data_float(&radar_raw_data);
            std::cout << "\nradar_raw_data_message:"
                      << std::endl;
            for (auto x = std::end(radar_raw_data.data); x != std::begin(radar_raw_data.data);)
            {
                std::cout << *--x << ' ';
            }
        }
    }

    std::cout << "\nStop player! \n"
              << std::endl;
    player.stop();
    std::cout << "Messages output finish!\n"
              << std::endl;
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
        std::string const addr_str = argv[1];
        stop_output = 0;
        signal(SIGINT, handle_sigint);
        if (addr_str.find("xethru_recording_meta.dat") != std::string::npos)
        {
            std::cout << "Found recording data!\n"
                      << std::endl;
            //Dataplayer configurations
            playback = 0;
            DataPlayer player(addr_str);
            return playback_radar_raw_data_message(player);
        }
        else
        {
            std::cout << "Building Serial/USB port connection ..." << std::endl;
            const std::string device_name = argv[1];
            ModuleConnector mc(device_name, log_level);
            return print_radar_raw_data_message(mc);
        }
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
        ModuleConnector mc(htonl(a << 24 | b << 16 | c << 8 | d), htons(port), log_level);
        return print_radar_raw_data_message(mc);
    }
}
