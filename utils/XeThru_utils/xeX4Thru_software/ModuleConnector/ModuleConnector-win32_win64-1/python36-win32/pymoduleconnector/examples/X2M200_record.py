#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example X2M200_record.py

#Target module: X2M200

#Introduction: This is an example of how to use the DataRecorder for X2M200 data recording.

#Command to run: "python X2M200_record.py -d com8" or "python3 X2M200_record.py -d com8"
                 change "com8" with your device name, using "--help" to see other options.

"""

from __future__ import print_function
from pymoduleconnector import ModuleConnector
from pymoduleconnector import DataType
from pymoduleconnector import PreferredSplitSize
from pymoduleconnector import RecordingOptions
from pymoduleconnector import DataRecorder
from pymoduleconnector import create_mc
from optparse import OptionParser
import pymoduleconnector
import sys
import time

def reset(device):
    with create_mc(device, 0) as mc:
        print("resetting module")
        x2m200 = mc.get_x2m200()
        x2m200.set_sensor_mode_idle()
        time.sleep(.5)
        x2m200.reset()

##! [Callback declare]
def on_file_available(data_type, filename):
    print("new file available for data type: {}".format(data_type))
    print("  |- file: {}".format(filename))
    if data_type == DataType.BasebandApDataType:
        print("processing baseband ap data from file")
    elif data_type == DataType.SleepDataType:
        print("processing sleep data from file")
##! [Callback declare]


##! [Callback meta declare]
def on_meta_file_available(session_id, meta_filename):
    print("new meta file available for recording with id: {}".format(session_id))
    print("  |- file: {}".format(meta_filename))
##! [Callback meta declare]

def record_baseband_ap(device):
    reset(device)
    try:
        ##! [Typical usage]
        with create_mc(device, 2) as mc:
            directory = "."
            # Start recording with default options, i.e. recorded files are stored in folder:
            # <directory>/xethru_recording_<date>_<time>_<session id>

            recorder = mc.get_data_recorder()
            ##! [Callback subscribe]
            recorder.subscribe_to_file_available(pymoduleconnector.AllDataTypes, on_file_available)
            ##! [Callback subscribe]
            ##! [Callback meta subscribe]
            recorder.subscribe_to_meta_file_available(on_meta_file_available)
            ##! [Callback meta subscribe]
            recorder.start_recording(DataType.BasebandApDataType | DataType.SleepDataType, directory)

            # Start sensor
            x2m200 = mc.get_x2m200()
            x2m200.load_sleep_profile()
            x2m200.enable_baseband_ap()
            x2m200.set_sensor_mode_run()
            while True:
                time.sleep(0.5)
        ##! [Typical usage]

            # NB! start_recording does not enable data output from module, this is enabled below

            ##########################################################################
            # For more advance recording options, use RecordingOptions()
            #
            # Example 1: Split recorded file every 15 minutes:
            # split_size = PreferredSplitSize()
            # split_size.set_duration(15 * 60)
            # options = RecordingOptions()
            # options.set_file_split_size(split_size)
            # recorder.start_recording(DataType.BasebandApDataType, directory, options)
            #
            # Example 2: Split recorded files into sub-directories every 15 minutes
            # split_size = PreferredSplitSize()
            # split_size.set_duration(15 * 60)
            # options = RecordingOptions()
            # options.set_directory_split_size(split_size)
            # recorder.start_recording(DataType.BasebandApDataType, directory, options)
            #
            # Example 3: Split recorded files into sub-directories every day at 2pm
            # split_size = PreferredSplitSize()
            # split_size.set_fixed_daily_hour(14)
            # options = RecordingOptions()
            # options.set_directory_split_size(split_size)
            # recorder.start_recording(DataType.BasebandApDataType, directory, options)
            #
            # Note: Example (1) is possible to combine with (2) or (3)
            #
            # Example 4: Set custom session id
            # options.set_session_id("<my_unique_session_id>")
            # recorder.start_recording(DataType.BasebandApDataType, directory, options)
            #
            # Example 5: Multiple configurations belonging to same session
            # # Split baseband files every 15 minutes
            # split_size = PreferredSplitSize()
            # split_size.set_duration(15 * 60)
            # options = RecordingOptions()
            # options.set_file_split_size(split_size)
            # recorder.start_recording(DataType.BasebandApDataType, directory, options)
            #
            # # No splitting of sleep data (csv) files
            # # Reset file split to default (no splitting)
            # options.set_file_split_size(PreferredSplitSize())
            # # Note that the same options object is used, i.e. same session id as
            # # previous call to start_recording.
            # recorder.start_recording(DataType.SleepDataType, directory, options)
            #
            # See RecordingOptions doc for more options
            ##########################################################################

    except (KeyboardInterrupt, SystemExit):
        raise

def main():
    parser = OptionParser()
    parser.add_option(
        "-d",
        "--device",
        dest="device_name",
        help="device file to use",
        metavar="FILE")

    (options, args) = parser.parse_args()

    if not options.device_name:
        print("you have to specify device, e.g.: python record.py -d /dev/ttyACM0")
        sys.exit(1)

    record_baseband_ap(options.device_name)

if __name__ == "__main__":
    main()
