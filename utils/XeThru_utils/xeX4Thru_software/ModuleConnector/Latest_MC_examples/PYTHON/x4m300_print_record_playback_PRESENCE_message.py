#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example x4m300_print_record_playback_PRESENCE_message.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module: X4M300

# Introduction: This is an example of how to print out application messages from X4M300 module.

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail
# xt_modules_print_info.py should be in the same folder
# xt_modules_record_playback_messages.py should be in the same folder

# Command to run: 
# 1. Use "python x4m300_print_record_playback_PRESENCE_message.py" to print application message. If device is not be automatically recognized,add argument "-d com8" to specify device. change "com8" with your device name, using "--help" to see other args. Using TCP server address as device name is also supported by specify TCP address like "-d tcp://192.168.1.169:3000". Add "-r" to enable recording during application messages printing out.
# 2. Use "python x4m300_print_record_playback_PRESENCE_message.py -f xethru_recording_xxxx/xethru_recording_meta.dat" to play back recording file.

"""
from __future__ import print_function, division
import sys
from argparse import ArgumentParser

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import pymoduleconnector
from pymoduleconnector import ModuleConnector
from pymoduleconnector.extras.auto import auto
from pymoduleconnector.ids import *

from xt_modules_print_info import print_module_info
from xt_modules_print_info import print_sensor_settings
from xt_modules_record_playback_messages import start_recorder
from xt_modules_record_playback_messages import start_player

presence_sensor_state_text = ("NO_PRESENCE", "PRESENCE", "INITIALIZING")
# User settings introduction
"""
output control:
Only one of XTS_ID_BASEBAND_IQ and XTS_ID_BASEBAND_AMPLITUDE_PHASE can be enabled at a time. Enabling one disables the other. Disabling one, even if it is already disabled, disables the other.
Only one of XTS_ID_PULSEDOPPLER_FLOAT and XTS_ID_PULSEDOPPLER_BYTE can be enabled for each of XTID_OUTPUT_CONTROL_PD_SLOW_ENABLE and XTID_OUTPUT_CONTROL_PD_FAST_ENABLE. Same with XTS_ID_NOISEMAP_FLOAT and XTS_ID_NOISEMAP_BYTE. Turning on a float output automatically disables the byte output, and vice versa.
"""
x4m300_par_settings = {'detection_zone': (0.4, 2),
                       'sensitivity': 5,
                       'led_control': (XTID_LED_MODE_OFF, 0),
                       # initialize noisemap everytime when get start (approximately 120s)
                       'noisemap_control': 0b110,
                       #    'output_control1': (XTS_ID_BASEBAND_IQ, 0),
                       #    'output_control2': (XTS_ID_BASEBAND_AMPLITUDE_PHASE, 0),
                       #    'output_control3': (XTS_ID_PULSEDOPPLER_FLOAT, 0),
                       #    'output_control4': (XTS_ID_PULSEDOPPLER_BYTE, 0),
                       #    'output_control5': (XTS_ID_NOISEMAP_FLOAT, 0),
                       #    'output_control6': (XTS_ID_NOISEMAP_BYTE, 0),
                       'output_control7': (XTS_ID_PRESENCE_SINGLE, 1),
                       'output_control8': (XTS_ID_PRESENCE_MOVINGLIST, 1)
                       }


def configure_x4m300(device_name, record=False, x4m300_settings=x4m300_par_settings):

    mc = ModuleConnector(device_name)
    x4m300 = mc.get_x4m300()

    print('Clearing buffer')
    while x4m300.peek_message_baseband_iq():
        x4m300.read_message_baseband_iq()
    while x4m300.peek_message_baseband_ap():
        x4m300.read_message_baseband_ap()
    while x4m300.peek_message_presence_single():
        x4m300.read_message_presence_single()
    while x4m300.peek_message_presence_movinglist():
        x4m300.read_message_presence_movinglist()
    while x4m300.peek_message_pulsedoppler_byte():
        x4m300.read_message_pulsedoppler_byte()
    while x4m300.peek_message_pulsedoppler_float():
        x4m300.read_message_pulsedoppler_float()
    while x4m300.peek_message_noisemap_byte():
        x4m300.read_message_noisemap_byte()
    while x4m300.peek_message_noisemap_float():
        x4m300.read_message_noisemap_float()

    print('Start recorder if recording is enabled')
    if record:
        start_recorder(mc)  # , DataType.PresenceSingleDataType)

    print('Ensuring no Xethru profile running')
    try:
        x4m300.set_sensor_mode(XTID_SM_STOP, 0)
    except RuntimeError:
        print('Xethru module could not enter stop mode')
    print('Loading new Xethru profile')
    # Check XeThru Module Communication Protocal section 5.2.8 for more detail
    x4m300.load_profile(XTS_ID_APP_PRESENCE_2)

    print('Set specific parameters')
    for variable, value in x4m300_settings.items():
        try:
            if 'output_control' in variable:
                variable = 'output_control'
            setter = getattr(x4m300, 'set_' + variable)
        except AttributeError as e:
            print("X4M300 does not have a setter function for '%s'." % variable)
            raise e

        if isinstance(value, tuple):
            setter(*value)
        else:
            setter(value)

        print("Setting %s to %s" % (variable, value))

    print_sensor_settings(x4m300)
    print('Set module to RUN mode')
    try:
        x4m300.set_sensor_mode(XTID_SM_RUN, 0)  # RUN mode
    except RuntimeError:
        print('Xethru module cloud not enter run mode')

    return x4m300


def print_x4m300_messages(x4m300):
    try:
        while True:
            while x4m300.peek_message_presence_single():
                rdata = x4m300.read_message_presence_single()
                print("message_presence_single: frame_counter: {} presence_state: {} distance: {} signal_quality: {}".format(rdata.frame_counter, presence_sensor_state_text[rdata.presence_state], rdata.distance, rdata.signal_quality))
            while x4m300.peek_message_presence_movinglist(): # update every 1 second
                rdata = x4m300.read_message_presence_movinglist()
                print("message_presence_movinglist:\ncounter: {} \nmovement_slow_items: {} \nmovement_fast_items: {}\n".format(rdata.frame_counter, np.array(rdata.movement_slow_items), np.array(rdata.movement_fast_items)))
    except:
        print('Messages output finish!')
    sys.exit(0)


def main():
    parser = ArgumentParser()
    parser.add_argument(
        "-d",
        "--device",
        dest="device_name",
        help="Seral port name used by target XeThru sensor, i.e com8, /dev/ttyACM0",
        metavar="FILE")
    parser.add_argument(
        "-r",
        "--record",
        action="store_true",
        default=False,
        dest="record",
        help="Enable recording")
    parser.add_argument(
        "-f",
        "--file",
        dest="meta_filename",
        metavar="FILE",
        help="meta file from recording")

    args = parser.parse_args()

    if not args.meta_filename:
        if args.device_name:
            device_name = args.device_name
        else:
            try:
                device_name = auto()[0]
            except:
                print("Fail to find serial port, please specify it by use -d!")
                raise
        print_module_info(device_name)
        x4m300 = configure_x4m300(
            device_name, args.record, x4m300_par_settings)
    else:
        player = start_player(meta_filename=args.meta_filename)
        mc = ModuleConnector(player, log_level=0)
        x4m300 = mc.get_x4m300()
    print_x4m300_messages(x4m300)


if __name__ == "__main__":
    main()
