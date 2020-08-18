#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example X4M300_playback_recording.py

#Target module: X4M300 (only data recorded by X4M300 needed) 

#Introduction: This is an example of how to playback recoreded data.

#Command to run: "python X4M300_plot_movementlist.py -f xethru_recording_meta.dat" or "python3 X4M300_plot_movementlist.py -f xethru_recording_meta.dat".
"""

from __future__ import print_function
from pymoduleconnector import ModuleConnector
from pymoduleconnector import DataPlayer
from pymoduleconnector import DataType

from optparse import OptionParser
from datetime import datetime

import sys
import time

def start_playback(meta_filename, depth):
    print("start playback from: {}, depth: {}".format(meta_filename, depth))
##! [Typical usage]
    player = DataPlayer(meta_filename, depth)

    mc = ModuleConnector(player, log_level=0)

    # Get read-only interface and receive telegrams / binary packets from recording
    x4m300 = mc.get_x4m300()

    # Control output
    player.set_filter(DataType.BasebandIqDataType | DataType.PresenceSingleDataType);
    player.play()
    # ...
    player.pause();
    # ...
    player.set_playback_rate(2.0);

##! [Typical usage]
    player.set_playback_rate(1.0);
    player.set_loop_mode_enabled(True);
    player.play();

    try:
        while True:
            if x4m300.peek_message_baseband_iq():
                data = x4m300.read_message_baseband_iq()
                print("received baseband iq data, frame counter: {}".format(data.frame_counter))
            if x4m300.peek_message_presence_single():
                data = x4m300.read_message_presence_single()
                print("received presence single data, frame counter: {}".format(data.frame_counter))
            time.sleep(0.02) # Sleep 20 ms
    except (KeyboardInterrupt, SystemExit):
        del mc
        raise

def main():
    parser = OptionParser()
    parser.add_option("-f", "--file", dest="meta_filename", metavar="FILE",\
                      help="meta file from recording")
    parser.add_option("--depth", dest="depth", metavar="NUMBER", type="int", default=-1,\
                      help="number of meta files to read in chained mode")

    (options, args) = parser.parse_args()
    if not options.meta_filename:
        print("Please specify a 'xethru_recording_meta.dat' file (use -f <file> or --file <file>)")
        sys.exit(1)

    start_playback(options.meta_filename, options.depth)

if __name__ == "__main__":
    main()
