#!/usr/bin/env python
""" \example xt_modules_record_playback_messages.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module:
#X4M200
#X4M300
#X4M03(XEP)

# Introduction:
# This script contains function on how to record and play back XeThru Module output data. Funtions are used by other examples, please refer to other examples for how to record and playback XeThru module data.

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail

"""
from __future__ import print_function, division
import sys

import pymoduleconnector
from pymoduleconnector import ModuleConnector
from pymoduleconnector import DataType
from pymoduleconnector import DataRecorder
from pymoduleconnector import DataPlayer
from pymoduleconnector import RecordingOptions
from pymoduleconnector.extras.auto import auto
from pymoduleconnector.ids import *


def start_recorder(mc):

    def on_file_available(data_type, filename):
        print("Recording data message file available for data type: {}".format(data_type))
        print("  |- file: {}".format(filename))

    def on_meta_file_available(session_id, meta_filename):
        print("Recording meta file available with id: {}".format(session_id))
        print("  |- file: {}".format(meta_filename))
    directory = "."
    recorder = mc.get_data_recorder()
    recorder.subscribe_to_file_available(
        pymoduleconnector.AllDataTypes, on_file_available)
    recorder.subscribe_to_meta_file_available(on_meta_file_available)
    recorder.start_recording(pymoduleconnector.AllDataTypes, directory)
    return recorder
def stop_recorder(recorder):
    recorder.stop_recording(pymoduleconnector.AllDataTypes)

def start_player(meta_filename, depth=-1):
    print("start playback from: {}, depth: {}".format(meta_filename, depth))
# ! [Typical usage]
    player = DataPlayer(meta_filename, depth)
    dur = player.get_duration()
    print("Duration(ms): {}".format(dur))
    mc = ModuleConnector(player, log_level=0)
    # Control output
    player.set_filter(pymoduleconnector.AllDataTypes)
    player.set_playback_rate(1.0)
    player.set_loop_mode_enabled(True)
    player.play()
    print("Player start to palyback data. If print or plot function is configured, it should start to work!")
    return player
