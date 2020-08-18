#include <ModuleConnector.hpp>
#include <DataPlayer.hpp>
#include <X4M300.hpp>
#include <Data.hpp>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

using namespace XeThru;

volatile sig_atomic_t stop_playback;
void handle_sigint(int num)
{
    stop_playback = 1;
}

void start_playback(const std::string &meta_filename)
{
//! [Typical usage]
    using namespace XeThru;

    DataPlayer player(meta_filename);
    player.set_filter(BasebandIqDataType | PresenceSingleDataType);

    ModuleConnector mc(player, 0);

    // Get read-only interface and receive telegrams / binary packets from recording
    X4M300 &x4m300 = mc.get_x4m300();

    // Control output
    player.play();
    // ...
    player.pause();
    // ...
    player.set_playback_rate(2.0);

//! [Typical usage]

    player.set_playback_rate(1.0);
    player.set_loop_mode_enabled(true);
    player.play();

    while (!stop_playback) {
        if (x4m300.peek_message_baseband_iq()) {
            std::cout << "received baseband iq data" << std::endl;
            BasebandIqData data;
            x4m300.read_message_baseband_iq(&data);
        } else if (x4m300.peek_message_presence_single()) {
            std::cout << "received presence single data" << std::endl;
            PresenceSingleData data;
            x4m300.read_message_presence_single(&data);
        }
        usleep(1000);
    }
}


int main(int argc, char ** argv)
{
    if (argc < 2) {
        std::cout << "usage: playback_recording <meta file (xethru_recording_meta.dat)>" << std::endl;
        return 1;
    }

    stop_playback = 0;
    signal(SIGINT, handle_sigint);
    start_playback(argv[1]);

    return 0;
}
